#include"writer.h"
#include<boost/lexical_cast.hpp>
#include<boost/algorithm/string.hpp>
using namespace boost;
namespace sstore {

    Writer::Writer() :
    vec(std::string(DATA_FILE)),
    hdr(std::string(META_FILE)) {
        in = new fstream;
        in->open(std::string(INPUT_FILE).c_str(), ios::in);
    }

    Writer::~Writer() {
        in->close();
        delete in;
    }

    void Writer::write() {
        string line;
        long total = 0;
        unsigned int size = 0;
        hdr.flatwrite(total);
        while (std::getline(*in, line, '\n')) {
            std::vector<long> adjvec = this->handleLine(line);
            size = adjvec.size();
            total += size;
            long* ptr = &adjvec[0];
            hdr.flatwrite(total);
            vec.flatwrite(ptr, size);
        }
    }
    
    std::vector<long> Writer::handleLine(std::string value){
        std::vector<long> vec;
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



}

