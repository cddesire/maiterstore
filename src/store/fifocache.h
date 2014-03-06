/* 
 * File:   fifocache.h
 * Author: root
 *
 * Created on October 9, 2013, 9:29 PM
 */

#ifndef FIFOCACHE_H
#define	FIFOCACHE_H
#include"kvmap.h"
#include<queue>
#include"const.h"
using namespace std;
namespace sstore {

    template <class K, class V>
    class FIFOCache {
    public:

        FIFOCache();

        ~FIFOCache();

        V getValue(const K& key);

        void addCache(const K& key, const V& val);

        bool containsKey(const K& key) const;

        void show();

    private:
        KVMap<K, V> _elements;
        std::queue<K> keys;
    };

    template<class K, class V>
    FIFOCache<K, V>::FIFOCache() {

    }

    template<class K, class V>
    FIFOCache<K, V>::~FIFOCache() {

    }

    template<class K, class V>
    V FIFOCache<K, V>::getValue(const K& key) {
        return this->_elements[key];
    }

    template<class K, class V>
    bool FIFOCache<K, V>::containsKey(const K& key) const {
        return this->_elements.containsKey(key);
    }

    template<class K, class V>
    void FIFOCache<K, V>::addCache(const K& key, const V& val) {
        if (this->containsKey(key)) {
            return;
        } else //miss 
        {
            if (keys.size() > CACHE_SIZE - 1) {
                K k = this->keys.front();
                this->keys.pop();
                this->_elements.remove(k);
            }
            this->_elements.add(key, val);
            this->keys.push(key);
        }

    }

    template<class K, class V>
    void FIFOCache<K, V>::show() {
        while (!this->keys.empty()) {
            cout << this->keys.front() << "\t";
            this->keys.pop();
        }
    }

}

#endif	/* FIFOCACHE_H */

