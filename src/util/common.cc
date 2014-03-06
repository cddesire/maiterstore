#include "util/common.h"
#include "util/file.h"
#include "util/static-initializers.h"
#include "util/rpc.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <execinfo.h>
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>

#include <asm/msr.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <lzo/lzo1x.h>
#include <mpi.h>

#ifdef CPUPROF
#include <google/profiler.h>
DEFINE_bool(cpu_profile, false, "");
#endif

#ifdef HEAPPROF
#include <google/heap-profiler.h>
#include <google/malloc_extension.h>
#endif

DEFINE_bool(dump_stacktrace, true, "");
DEFINE_bool(localtest, false, "");
DEFINE_bool(run_tests, false, "");

DEFINE_string(hostfile, "conf/mpi-cluster", "");
DEFINE_int32(workers, 2, "");


namespace dsm {

const double Histogram::kMinVal = 1e-9;
const double Histogram::kLogBase = 1.1;


int Histogram::bucketForVal(double v) {
  if (v < kMinVal) { return 0; }

  v /= kMinVal;
  v += kLogBase;

  return 1 + static_cast<int>(log(v) / log(kLogBase));
}

double Histogram::valForBucket(int b) {
  if (b == 0) { return 0; }
  return exp(log(kLogBase) * (b - 1)) * kMinVal;
}

void Histogram::add(double val) {
  int b = bucketForVal(val);
//  LOG_EVERY_N(INFO, 1000) << "Adding... " << val << " : " << b;
  if (buckets.size() <= b) { buckets.resize(b + 1); }
  ++buckets[b];
  ++count;
}

void DumpProfile() {
#ifdef CPUPROF
  ProfilerFlush();
#endif
}

string Histogram::summary() {
  string out;
  int total = 0;
  for (int i = 0; i < buckets.size(); ++i) { total += buckets[i]; }
  string hashes = string(100, '#');

  for (int i = 0; i < buckets.size(); ++i) {
    if (buckets[i] == 0) { continue; }
    out += StringPrintf("%-20.3g %6d %.*s\n", valForBucket(i), buckets[i], buckets[i] * 80 / total, hashes.c_str());
  }
  return out;
}

uint64_t get_memory_total() {
  uint64_t m = -1;
  FILE* procinfo = fopen(StringPrintf("/proc/meminfo", getpid()).c_str(), "r");
  while (fscanf(procinfo, "MemTotal: %llu kB", &m) != 1) {
    if (fgetc(procinfo) == EOF) { break; }
  }
  fclose(procinfo);

  return m * 1024;
}

uint64_t get_memory_rss() {
  uint64_t m = -1;
  FILE* procinfo = fopen(StringPrintf("/proc/%d/status", getpid()).c_str(), "r");
  while (fscanf(procinfo, "VmRSS: %llu kB", &m) != 1) {
    if (fgetc(procinfo) == EOF) { break; }
  }
  fclose(procinfo);

  return m * 1024;
}

double get_processor_frequency() {
  double freq;
  int a, b;
  FILE* procinfo = fopen("/proc/cpuinfo", "r");
  while (fscanf(procinfo, "cpu MHz : %d.%d", &a, &b) != 2) {
    fgetc(procinfo);
  }

  freq = a * 1e6 + b * 1e-4;
  fclose(procinfo);
  return freq;
}

void Sleep(double t) {
  timespec req;
  req.tv_sec = (int)t;
  req.tv_nsec = (int64_t)(1e9 * (t - (int64_t)t));
  nanosleep(&req, NULL);
}

void SpinLock::lock() volatile {
  while (!__sync_bool_compare_and_swap(&d, 0, 1));
}

void SpinLock::unlock() volatile {
  d = 0;
}

static void FatalSignalHandler(int sig) {
  fprintf(stderr, "Fatal error; signal %d occurred.\n", sig);
  static SpinLock lock;
  static void* stack[128];

  lock.lock();


  if (!FLAGS_dump_stacktrace) {
    _exit(1);
  }

  int count = backtrace(stack, 128);
  backtrace_symbols_fd(stack, count, STDERR_FILENO);

  static char cmdbuffer[1024];
  snprintf(cmdbuffer, 1024,
           "gdb "
           "-p %d "
           "-ex 'set print pretty' "
           "-ex 'set pagination 0' "
           "-ex 'thread apply all bt ' "
           "-batch ", getpid());

  fprintf(stderr, "Calling gdb with: %s", cmdbuffer);

  system(cmdbuffer);
  _exit(1);
}

void Init(int argc, char** argv) {
  FLAGS_logtostderr = true;
  FLAGS_logbuflevel = -1;

  CHECK_EQ(lzo_init(), 0);

  google::SetUsageMessage(StringPrintf("%s: invoke from mpirun, using --runner to select control function.", argv[0]));
  google::ParseCommandLineFlags(&argc, &argv, false);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();

#ifdef CPUPROF
  if (FLAGS_cpu_profile) {
    mkdir("profile/", 0755);
    char buf[100];
    gethostname(buf, 100);
    ProfilerStart(StringPrintf("profile/cpu.%s.%d", buf, getpid()).c_str());
  }
#endif

#ifdef HEAPPROF
  char buf[100];
  gethostname(buf, 100);
  HeapProfilerStart(StringPrintf("profile/heap.%s.%d", buf, getpid()).c_str());
#endif

  RunInitializers();

  if (FLAGS_run_tests) {
    RunTests();
    exit(0);
  }

  // If we are not running in the context of MPI, go ahead and invoke
  // mpirun to start ourselves up.
  if (!getenv("OMPI_UNIVERSE_SIZE")) {
    string cmd = StringPrintf("mpirun "
                              " -hostfile %s"
                              " -bycore"
                              " -nooversubscribe"
                              " -n %d"
                              " %s"
                              " --log_prefix=true ",
                              FLAGS_hostfile.c_str(),
                              FLAGS_workers,
                              JoinString(&argv[0], &argv[argc]).c_str()
                              );

    LOG(INFO) << "Invoking MPI..." << cmd;
    system(cmd.c_str());
    exit(0);
  }

  NetworkThread::Init();

  srandom(time(NULL));
}
}
