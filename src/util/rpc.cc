#include "util/rpc.h"
#include "util/common.h"
#include "util/common.pb.h"
#include <signal.h>

DECLARE_bool(localtest);
DECLARE_double(sleep_time);
DEFINE_bool(rpc_log, false, "");

namespace dsm {

static void CrashOnMPIError(MPI_Comm * c, int * errorCode, ...) {
  static dsm::SpinLock l;
  l.lock();

  char buffer[1024];
  int size = 1024;
  MPI_Error_string(*errorCode, buffer, &size);
  LOG(FATAL) << "MPI function failed: " << buffer;
}

struct Header {
  Header() : is_reply(false) {}
  bool is_reply;
};

// Represents an active RPC to a remote peer.
struct RPCRequest : private boost::noncopyable {
  int target;
  int rpc_type;
  int failures;

  string payload;
  MPI::Request mpi_req;
  MPI::Status status;
  double start_time;

  RPCRequest(int target, int method, const Message& msg, Header h=Header());
  RPCRequest(int target, int method, Header h=Header());
  ~RPCRequest();

  bool finished();
  double elapsed();
};

RPCRequest::~RPCRequest() {}

bool RPCRequest::finished() { return mpi_req.Test(status); }
double RPCRequest::elapsed() { return Now() - start_time; }

// Send the given message type and data to this peer.
RPCRequest::RPCRequest(int tgt, int method, const Message& ureq, Header h) {
  failures = 0;
  target = tgt;
  rpc_type = method;

  payload.append((char*)&h, sizeof(Header));
  ureq.AppendToString(&payload);
}

RPCRequest::RPCRequest(int tgt, int method, Header h) {
  failures = 0;
  target = tgt;
  rpc_type = method;

  payload.append((char*)&h, sizeof(Header));
}

NetworkThread::NetworkThread() {
  if (!getenv("OMPI_COMM_WORLD_RANK")) {
    world_ = NULL;
    id_ = -1;
    running = false;
    return;
  }

  MPI::Init_thread(MPI_THREAD_SINGLE);

  MPI_Errhandler handler;
  MPI_Errhandler_create(&CrashOnMPIError, &handler);
  MPI::COMM_WORLD.Set_errhandler(handler);

  world_ = &MPI::COMM_WORLD;
  running = 1;
  t_ = new boost::thread(&NetworkThread::Run, this);
  id_ = world_->Get_rank();

  for (int i = 0; i < kMaxMethods; ++i) {
    callbacks_[i] = NULL;
  }
}

bool NetworkThread::active() const {
  return active_sends_.size() + pending_sends_.size() > 0;
}

int NetworkThread::size() const {
  return world_->Get_size();
}

int64_t NetworkThread::pending_bytes() const {
  boost::recursive_mutex::scoped_lock sl(send_lock);
  int64_t t = 0;

  for (unordered_set<RPCRequest*>::const_iterator i = active_sends_.begin(); i != active_sends_.end(); ++i) {
    t += (*i)->payload.size();
  }

  for (int i = 0; i < pending_sends_.size(); ++i) {
    t += pending_sends_[i]->payload.size();
  }

  return t;
}

void NetworkThread::CollectActive() {
  if (active_sends_.empty())
    return;

  boost::recursive_mutex::scoped_lock sl(send_lock);
  unordered_set<RPCRequest*>::iterator i = active_sends_.begin();
  VLOG(3) << "Pending sends: " << active_sends_.size();
  while (i != active_sends_.end()) {
    RPCRequest *r = (*i);
    VLOG(3) << "Pending: " << MP(id(), MP(r->target, r->rpc_type));
    if (r->finished()) {
      if (r->failures > 0) {
        LOG(INFO) << "Send " << MP(id(), r->target) << " of size " << r->payload.size()
                  << " succeeded after " << r->failures << " failures.";
      }
      VLOG(3) << "Finished send to " << r->target << " of size " << r->payload.size();
      delete r;
      i = active_sends_.erase(i);
      continue;
    }
    ++i;
  }
}

void NetworkThread::InvokeCallback(CallbackInfo *ci, RPCInfo rpc) {
  ci->call(rpc);
  Header reply_header;
  reply_header.is_reply = true;
  Send(new RPCRequest(rpc.source, rpc.tag, *ci->resp, reply_header));
}

void NetworkThread::Run() {
  while (running) {
    MPI::Status st;

    if (world_->Iprobe(MPI::ANY_SOURCE, MPI::ANY_TAG, st)) {
      int tag = st.Get_tag();
      int source = st.Get_source();
      int bytes = st.Get_count(MPI::BYTE);

      string data;
      data.resize(bytes);

      world_->Recv(&data[0], bytes, MPI::BYTE, source, tag, st);

      Header *h = (Header*)&data[0];

      stats["bytes_received"] += bytes;
      stats[StringPrintf("received.%s", MessageTypes_Name((MessageTypes)tag).c_str())] += 1;
      CHECK_LT(source, kMaxHosts);

      VLOG(2) << "Received packet - source: " << source << " tag: " << tag;
      if (h->is_reply) {
        boost::recursive_mutex::scoped_lock sl(q_lock[tag]);
        replies[tag][source].push_back(data);
      } else {
        if (callbacks_[tag] != NULL) {
          CallbackInfo *ci = callbacks_[tag];
          ci->req->ParseFromArray(&data[0] + sizeof(Header), data.size() - sizeof(Header));
          VLOG(2) << "Got incoming: " << ci->req->ShortDebugString();

          RPCInfo rpc = { source, id(), tag };
          if (ci->spawn_thread) {
            new boost::thread(boost::bind(&NetworkThread::InvokeCallback, this, ci, rpc));
          } else {
            ci->call(rpc);
            Header reply_header;
            reply_header.is_reply = true;
            Send(new RPCRequest(source, tag, *ci->resp, reply_header));
          }
        } else {
          boost::recursive_mutex::scoped_lock sl(q_lock[tag]);
          requests[tag][source].push_back(data);
        }
      }
    } else {
      Sleep(FLAGS_sleep_time);
    }

    while (!pending_sends_.empty()) {
      boost::recursive_mutex::scoped_lock sl(send_lock);
      RPCRequest* s = pending_sends_.back();
      pending_sends_.pop_back();
      s->start_time = Now();
      s->mpi_req = world_->Isend(
          s->payload.data(), s->payload.size(), MPI::BYTE, s->target, s->rpc_type);
      active_sends_.insert(s);
    }

    CollectActive();

    PERIODIC(10., { DumpProfile(); });
  }
}

bool NetworkThread::check_request_queue(int src, int type, Message* data) {
  CHECK_LT(src, kMaxHosts);
  CHECK_LT(type, kMaxMethods);

  Queue& q = requests[type][src];
  if(q.size()%10==0 && q.size()!=0) VLOG(1)<<"REQUEST QUEUE SIZE for type "<< type << " src " << src << " is " << q.size();
  if (!q.empty()) {
    boost::recursive_mutex::scoped_lock sl(q_lock[type]);
    if (q.empty())
      return false;

    const string& s = q.front();
    if (data) {
      data->ParseFromArray(s.data() + sizeof(Header), s.size() - sizeof(Header));
    }

    q.pop_front();
    return true;
  }
  return false;
}

bool NetworkThread::check_reply_queue(int src, int type, Message* data) {
  CHECK_LT(src, kMaxHosts);
  CHECK_LT(type, kMaxMethods);

  Queue& q = replies[type][src];
  if(q.size()%10==0 && q.size()!=0) VLOG(1)<<"REPLY QUEUE SIZE for type "<< type << " src " << src << " is " << q.size();
  if (!q.empty()) {
    boost::recursive_mutex::scoped_lock sl(q_lock[type]);
    if (q.empty())
      return false;

    const string& s = q.front();
    if (data) {
      data->ParseFromArray(s.data() + sizeof(Header), s.size() - sizeof(Header));
    }

    q.pop_front();
    return true;
  }
  return false;
}

  // Blocking read for the given source and message type.
void NetworkThread::Read(int desired_src, int type, Message* data, int *source) {
  Timer t;
  while (!TryRead(desired_src, type, data, source)) {
    Sleep(FLAGS_sleep_time);
  }
  stats["network_time"] += t.elapsed();
}

bool NetworkThread::TryRead(int src, int type, Message* data, int *source) {
  if (src == MPI::ANY_SOURCE) {
    for (int i = 0; i < world_->Get_size(); ++i) {
      if (TryRead(i, type, data, source)) {
        return true;
      }
    }
  } else {
    if (check_request_queue(src, type, data)) {
      if (source) { *source = src; }
      return true;
    }
  }

  return false;
}

void NetworkThread::Call(int dst, int method, const Message &msg, Message *reply) {
  Send(dst, method, msg);
  Timer t;
  while (!check_reply_queue(dst, method, reply)) {
    Sleep(FLAGS_sleep_time);
  }
}

  // Enqueue the given request for transmission.
void NetworkThread::Send(RPCRequest *req) {
  boost::recursive_mutex::scoped_lock sl(send_lock);
//    LOG(INFO) << "Sending... " << MP(req->target, req->rpc_type);
  //LOG(INFO) << "Sending... from " << id() << " to " << req->target << " type: " << req->rpc_type << " size: " << req->payload.size();
  stats["bytes_sent"] += req->payload.size();
  stats[StringPrintf("sends.%s", MessageTypes_Name((MessageTypes)(req->rpc_type)).c_str())] += 1;
  pending_sends_.push_back(req);
}

int NetworkThread::Send(int dst, int method, const Message &msg) {
  RPCRequest *r = new RPCRequest(dst, method, msg);
  int size = r->payload.size();
  Send(r);
  return size;
}

void NetworkThread::ObjectCreate(int dst, int method) {
  RPCRequest *r = new RPCRequest(dst, method);
  delete r;
}

void NetworkThread::DSend(int dst, int method, const Message &msg) {
	boost::recursive_mutex::scoped_lock sl(send_lock);
  RPCRequest *r = new RPCRequest(dst, method, msg);

  stats["bytes_sent"] += r->payload.size();
  stats[StringPrintf("sends.%s", MessageTypes_Name((MessageTypes)(r->rpc_type)).c_str())] += 1;

  r->start_time = Now();
  r->mpi_req = world_->Isend(
      r->payload.data(), r->payload.size(), MPI::BYTE, r->target, r->rpc_type);
  active_sends_.insert(r);
}

void NetworkThread::Shutdown() {
  if (running) {
    Flush();
    running = false;
    MPI_Finalize();
  }
}

void NetworkThread::Flush() {
  while (active()) {
    Sleep(FLAGS_sleep_time);
  }
}

void NetworkThread::Broadcast(int method, const Message& msg) {
  for (int i = 1; i < world_->Get_size(); ++i) {
    Send(i, method, msg);
  }
}

void NetworkThread::SyncBroadcast(int method, const Message& msg) {
  VLOG(2) << "Sending: " << msg.ShortDebugString();
  Broadcast(method, msg);
  WaitForSync(method, world_->Get_size() - 1);
}

void NetworkThread::WaitForSync(int method, int count) {
  EmptyMessage empty;
  while (count > 0) {
    for (int i = 0; i < world_->Get_size(); ++i) {
      if (check_reply_queue(i, method, NULL))
        --count;
    }
    Sleep(FLAGS_sleep_time);
  }
}

void NetworkThread::_RegisterCallback(int message_type, Message *req, Message* resp, Callback cb) {
  CallbackInfo *cbinfo = new CallbackInfo;

  cbinfo->spawn_thread = false;
  cbinfo->req = req;
  cbinfo->resp = resp;
  cbinfo->call = cb;

  CHECK_LT(message_type, kMaxMethods) << "Message type: " << message_type << " over limit.";
  callbacks_[message_type] =  cbinfo;
}

void NetworkThread::SpawnThreadFor(int req_type) {
  callbacks_[req_type]->spawn_thread = true;
}

static NetworkThread* net = NULL;
NetworkThread* NetworkThread::Get() {
  return net;
}

static void ShutdownMPI() {
  NetworkThread::Get()->Shutdown();
}

void NetworkThread::Init() {
  VLOG(1) << "Initializing network...";
  CHECK(net == NULL);
  net = new NetworkThread();
  atexit(&ShutdownMPI);
}

}

