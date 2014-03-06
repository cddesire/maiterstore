#include "client/client.h"


using namespace dsm;

//DECLARE_string(graph_dir);
DECLARE_string(result_dir);
DECLARE_int64(num_nodes);
DECLARE_double(portion);

struct PagerankIterateKernel : public IterateKernel<long, float, vector<long> > {
    float zero;

    PagerankIterateKernel() : zero(0){}

    void read_data(string& line, long& k, vector<long>& data){
        string linestr(line);
        int pos = linestr.find("\t");
        if(pos == -1) return;
        
        long source = boost::lexical_cast<long>(linestr.substr(0, pos));

        vector<long> linkvec;
        string links = linestr.substr(pos+1);
        if(*links.end()!=' '){
            links=links+" ";
        }
        int spacepos = 0;
        while((spacepos = links.find_first_of(" ")) != links.npos){
            int to;
            if(spacepos > 0){
                to = boost::lexical_cast<long>(links.substr(0, spacepos));
            }
            links = links.substr(spacepos+1);
            linkvec.push_back(to);
        }

        k = source;
        data = linkvec;
    }

    void init_c(const long& k, float& delta,vector<long>& data){
        delta = 0.2;
    }

    void init_v(const long& k,float& v,vector<long>& data){
        v=0;  
    }
    
    void accumulate(float& a, const float& b){
            a = a + b;
    }

    void priority(float& pri, const float& value, const float& delta){
            pri = delta;
    }

    void g_func(const long& k,const float& delta, const float&value, const vector<long>& data, vector<pair<long, float> >* output){
            int size = (int) data.size();
            float outv = delta * 0.8 / size;
            for(vector<long>::const_iterator it=data.begin(); it!=data.end(); it++){
                    long target = *it;
                    output->push_back(make_pair(target, outv));
            }
    }

    const float& default_v() const {
            return zero;
    }
       //By vagrant
    long getKey(string key) {
       return lexical_cast<long>(key);
    }

    vector<long> getValue(string value) {
        vector<long> vec;
        trim_right(value);
        vector<string> strs;
        boost::split(strs, value, boost::is_any_of(" "));
        vector<string>::iterator it = strs.begin();
        for (; it != strs.end(); it++) {
            long val = lexical_cast<long>(*it);
            vec.push_back(val);
        }
        return vec;
    }
    
    
};


static int Pagerank(ConfigData& conf) {
    MaiterKernel<long, float, vector<long> >* kernel = new MaiterKernel<long, float, vector<long> >(
                                        conf, FLAGS_num_nodes, FLAGS_portion, FLAGS_result_dir,
                                        new Sharding::Mod,
                                        new PagerankIterateKernel,
                                        new TermCheckers<long, float>::Diff);
    kernel->registerMaiter();

    if (!StartWorker(conf)) {
        Master m(conf);
        m.run_maiter(kernel);
    }
    
    delete kernel;
    return 0;
}

REGISTER_RUNNER(Pagerank);