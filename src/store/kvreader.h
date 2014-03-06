/* 
 * File:   blockreader.h
 * Author: root
 *
 * Created on September 24, 2013, 7:46 PM
 */

#ifndef KVREADER_H
#define	KVREADER_H
#include <vector>
#include <string>
#include <sstream>
#include <list>
#include<map>
#include <algorithm>
#include <iostream>
#include <iterator>
#include "record.h"
#include"kvmap.h"
#include"../kernel/table.h"
#include <boost/algorithm/string.hpp>
using namespace std;
using namespace boost::algorithm;
using namespace dsm;
namespace sstore {

    template<class K, class V, class D>
    class KVReader {
    public:

        KVReader();

        ~KVReader();

        list<sstore::Record> getRecordList(string text);

        KVMap<K, D> getKeyValue(string text, IterateKernel<K, V, D>* inmaiter);

    private:
        vector<string> splitString(string text, string delimiter);
    };

    template<class K, class V, class D>
    vector<string> KVReader<K, V, D>::splitString(string text, string delimiter) {
        
        while (!boost::ends_with(text, "####")) {
            boost::erase_tail(text, 1);
        }
        
        trim_right_if(text, !is_digit());
        vector<string> toRet;
        split(toRet, text, is_any_of(delimiter), token_compress_on);
        return toRet;
    }

    template<class K, class V, class D>
    KVReader<K, V, D>::KVReader() {

    }

    template<class K, class V, class D>
    KVReader<K, V, D>::~KVReader() {

    }

    template<class K, class V, class D>
    list<sstore::Record> KVReader<K, V, D>::getRecordList(string text) {
        vector<string> result = splitString(text, "#");
        list<sstore::Record> list;
        for (int i = 0; i < result.size(); i++) {
            sstore::Record r(result.at(i));
            list.push_back(r);
        }
        return list;
    }

    template<class K, class V, class D>
    KVMap<K, D> KVReader<K, V, D>::getKeyValue(string text, struct IterateKernel<K, V, D>* inmaiter) {
        list<sstore::Record> rs = this->getRecordList(text);
        KVMap<K, D> kvmap;
        list<sstore::Record>::iterator iterator;
        for (iterator = rs.begin(); iterator != rs.end(); iterator++) {
            K key = inmaiter->getKey(iterator->getKey());
            D val = inmaiter->getValue(iterator->getValue());
            kvmap.add(key, val);
        }
        return kvmap;
    }

}
#endif	/* KVREADER_H */

