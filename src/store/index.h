/* 
 * File:   index.h
 * Author: root
 *
 * Created on September 26, 2013, 6:01 PM
 */

#ifndef INDEX_H
#define	INDEX_H
#include "const.h"
#include <algorithm>   
#include <functional>   
#include <vector> 
#include <string>
#include "filemanager.h"
#include <iomanip>
#include <sstream>
#include <boost/lexical_cast.hpp>
using namespace boost;
namespace sstore {

    class Index {
    private:
        std::vector<long> index;
        //LCTrie lcTrie;
        std::string getFixedWidthString(std::string strSrc, int iLen, char cFill);

        char* convert(std::string str);
    public:

        Index();

        virtual ~Index();

        std::vector<long> splitString(std::string line, char delimiter);

        int getBlockAddr(long key);

        void initIndex(std::string keys);

        bool updateIndex();

        void show();

    };
    
    Index::Index() {
        FileManager fm(META_FILE);
        std::string keys;
        fm.readFully(keys);
        this->initIndex(keys);
    }

    Index::~Index() {
        // TODO Auto-generated destructor stub
    }

    std::vector<long> Index::splitString(std::string line, char delimiter) {
        std::vector<long> toRet;
        if (std::string::npos == line.find(delimiter)) {
            return toRet;
        }
        std::string word;
        std::stringstream stream(line);
        while (getline(stream, word, delimiter)) {
            toRet.push_back(boost::lexical_cast<long>(word));
        }
        return toRet;
    }

    int Index::getBlockAddr(long key) {
        std::vector<long>::iterator start;
        std::vector<long>::iterator position;
        start = this->index.begin();
        position = upper_bound(start, start + this->index.size(), key);
        //std::cout<<*position<<std::endl;
        //block_addr dis = this->lcTrie.lookup(this->convert(*(position-1)));
        //int dist = (int)(std::distance(start, position) - 1);
        int dist = (int)std::distance(start, position) -1;
        return dist ;
    }

    void Index::initIndex(std::string keys) {
        // std::cout<<keys<<std::endl;
        std::vector<long> res = splitString(keys, '#');
        std::vector<long>::iterator start = res.begin();
        std::vector<long>::iterator end = res.end();
        this->index.assign(start, end);
        //    long i = 0;
        //    for (; start != end; start++) {
        //        this->lcTrie.insert(this->convert(*start), i++);
        //    }
    }

    bool Index::updateIndex() {
        return true;
    }

    std::string Index::getFixedWidthString(std::string strSrc, int iLen, char cFill) {
        std::stringstream strStream;
        strStream << std::setw(iLen) << std::setfill(cFill) << strSrc;
        return strStream.str();
    }

    void Index::show() {
//        std::vector<int>::iterator it;
//        for (it = index.begin(); it != this->index.end(); it++) {
//            std::cout << *it << "\t";
//        }
//        std::cout << std::endl;
    }

    char* Index::convert(std::string str) {
        std::vector<char> writable(str.size() + 1);
        std::copy(str.begin(), str.end(), writable.begin());
        return &*writable.begin();
    }


}
#endif	/* INDEX_H */

