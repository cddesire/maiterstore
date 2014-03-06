/* 
 * File:   sstore.h
 * Author: root
 *
 * Created on September 27, 2013, 2:02 PM
 */

#ifndef SSTORE_H
#define	SSTORE_H
#include"const.h"
#include "cache.h"
#include"record.h"
#include"kvreader.h"
#include"lrucache.h"
#include"fifocache.h"
#include "reader.h"
#include<string>
#include<fstream>
#include"../kernel/table.h"
using namespace std;
using namespace dsm;

namespace sstore {

    template <class K, class V, class D>
    class SStore {
    public:

        SStore();

        ~SStore();

        bool getData(K k, D& d, V priority, IterateKernel<K, V, D>* inmaiter);

        //void batchedPut();

    private:
        Cache<K, D> cache;

        Reader reader;
    };

    template <class K, class V, class D>
    SStore<K, V, D>::SStore() {
       
    }

    template <class K, class V, class D>
    SStore<K, V, D>::~SStore() {

    }


    template <class K, class V, class D>
    bool SStore<K, V, D>::getData(K k, D& d, V priority, IterateKernel<K, V, D>* inmaiter) {

        bool existCache = this->cache.containsKey(k);

        if (existCache) {
            //cout <<k<< "\tin cache" << endl;
            d = cache.getValue(k);
            if (!cache.containsInfluKey(k)) {
                cache.addCache(k, d, priority);
            }
            return true;
        }

        d = reader.read(k);
        cache.addCache(k, d, priority);
        return true;
    }

    //        template <class K, class V, class D>
    //        bool SStore<K, V, D>::getData(K k, D& d, V priority, IterateKernel<K, V, D>* inmaiter) {
    //            bool existCache = this->cache.contains(k);
    //            if (existCache) {
    //                cout << k << "\tin cache." << endl;
    //                d = cache.get(k);
    //                return true;
    //            }
    //            bool existKVMap = this->prefecher.containsKey(k);
    //            if (existKVMap) {
    //                //  cout <<k<< "\tin prefecther" << endl;
    //                d = this->prefecher[k];
    //                if (priority > 3) {
    //                    cache.insert(k, d);
    //                }
    //                return true;
    //            }
    //            bool existBlock = this->getBlock(k, d, inmaiter);
    //            if (existBlock) {
    //                if (priority > 3) {
    //                    cache.insert(k, d);
    //                }
    //                return true;
    //            }
    //            return false;
    //        }

    //    template <class K, class V, class D>
    //    bool SStore<K, V, D>::getData(K k, D& d, V priority, IterateKernel<K, V, D>* inmaiter) {
    //        bool existCache = this->cache.containsKey(k);
    //        if (existCache) {
    //            cout << k << "\tin cache." << endl;
    //            d = cache.getValue(k);
    //            return true;
    //        }
    //        bool existKVMap = this->prefecher.containsKey(k);
    //        if (existKVMap) {
    //            //  cout <<k<< "\tin prefecther" << endl;
    //            d = this->prefecher[k];
    //            if (priority > 3) {
    //                cache.addCache(k,d);
    //            }
    //            return true;
    //        }
    //        bool existBlock = this->getBlock(k, d, inmaiter);
    //        if (existBlock) {
    //            if (priority > 3) {
    //                cache.addCache(k,d);
    //            }
    //            return true;
    //        }
    //        return false;
    //    }

}

#endif	/* SSTORE_H */

