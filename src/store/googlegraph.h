/* 
 * File:   googlegraph.h
 * Author: root
 *
 * Created on October 10, 2013, 9:52 AM
 */

#ifndef GOOGLEGRAPH_H
#define	GOOGLEGRAPH_H
#include"keyvalue.h"
#include<vector>
#include<boost/lexical_cast.hpp>
#include<boost/algorithm/string.hpp>
using namespace boost;
namespace sstore {

    class GraphData : public KeyValue<string, vector<long> > {

        string getKey(string key) {
            //return lexical_cast<long>(key);
            return key;
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
}
#endif	/* GOOGLEGRAPH_H */

