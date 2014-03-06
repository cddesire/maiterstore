/* 
 * File:   keyvalue.h
 * Author: root
 *
 * Created on October 1, 2013, 9:25 AM
 */

#ifndef KEY_H
#define	KEY_H
#include<string>
using namespace std;
namespace sstore {

    template <class K, class V, class D>
    class KeyValue {
    public:
        virtual K getKey(string key) = 0;
        virtual D getValue(string value) = 0;
    };

}

#endif	/* KEY_H */

