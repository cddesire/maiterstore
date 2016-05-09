/* 
 * File:   .h
 * Author: root
 *
 * Created on September 26, 2013, 9:06 PM
 */

#ifndef PRORITYMAP_H
#define	PRORITYMAP_H

#include <map>
#include"kvmap.h"
#include"const.h"
#include<iostream>
#include <fstream>
#include<string>
using namespace std;
namespace sstore {

    template <class K, class V>
    class Cache {
    public:
        Cache();

        virtual ~Cache();

        Cache(const Cache& other);

        void addCache(const K& key, const V& val, const float& priority);

        bool containsKey(const K& key) const;

        bool containsInfluKey(const K& key) const;

        V getValue(const K& key);

        void show();

        typedef typename std::multimap<float, K>::iterator iterator;

    protected:
        void remove(const K& key);

        void add(const K& key, const V& val, const float& priority);

        bool deleteMin(const float& priority);

    private:
        void updatePrority(const K& key, const float& priority);

        std::multimap<float, K> pk;

        KVMap<K, float> kp;

        KVMap<K, V> kv;

        KVMap<long, vector<long> > influMap;
    };

    template <class K, class V>
    Cache<K, V>::Cache() {
        ifstream fin(INFLU_FILE);
        string line;
        while (getline(fin, line)) {
            string linestr(line);
            int pos = linestr.find("\t");
            if (pos == -1) return;
            long source = boost::lexical_cast<long>(linestr.substr(0, pos));
            vector<long> linkvec;
            string links = linestr.substr(pos + 1);
            if (*links.end() != ' ') {
                links = links + " ";
            }
            int spacepos = 0;
            while ((spacepos = links.find_first_of(" ")) != links.npos) {
                int to;
                if (spacepos > 0) {
                    to = boost::lexical_cast<long>(links.substr(0, spacepos));
                }
                links = links.substr(spacepos + 1);
                linkvec.push_back(to);
            }
            influMap.add(source, linkvec);
        }
        fin.close();
    }

    template <class K, class V>
    Cache<K, V>::~Cache() {


    }

    template <class K, class V>
    void Cache<K, V>::show() {
        cout << "pk cache" << endl;
        typename std::multimap<float, K>::iterator it = pk.begin();
        for (; it != pk.end(); it++) {
            std::cout << (*it).first << " => " << (*it).second << '\n';
        }
        //    cout << "kp cache" << endl;
        //    this->kp.show();
        //    cout << "kv cache" << endl;
        //    this->kv.show();

    }

    template <class K, class V>
    V Cache<K, V>::getValue(const K& key) {
        if (this->containsInfluKey(key)) {
            //cout << "in influMap" << endl;
            return influMap[key];
        } else {
            //cout << "in hotMap" << endl;
            return kv[key];
        }
    }

    template <class K, class V>
    bool Cache<K, V>::containsKey(const K& key) const {
        return kv.containsKey(key) || influMap.containsKey(key);
    }

    template <class K, class V>
    bool Cache<K, V>::containsInfluKey(const K& key) const {
        return influMap.containsKey(key);
    }

    template <class K, class V>
    Cache<K, V>::Cache(const Cache& other) {
        this->kp = other.kp;
        this->kv = other.kv;
        this->pk = other.pk;
    }

    template <class K, class V>
    void Cache<K, V>::remove(const K& key) {
        this->kp.remove(key);
        this->kv.remove(key);
    }

    template <class K, class V>
    void Cache<K, V>::add(const K& key, const V& val, const float& priority) {
        kv.add(key, val);
        kp.add(key, priority);
        pk.insert(std::pair<float, K>(priority, key));
    }

    template <class K, class V>
    void Cache<K, V>::updatePrority(const K& key, const float& priority) {

        float proir = kp[key];
        std::pair<iterator, iterator> iterpair
                = this->pk.equal_range(proir);
        iterator it = iterpair.first;
        for (; it != iterpair.second; ++it) {
            if (it->second == key) {
                pk.erase(it);
                break;
            }
        }//end for

        pk.insert(std::pair<float, K>(priority, key));
        kp.remove(key);
        kp.add(key, priority);

    }

    template <class K, class V>
    bool Cache<K, V>::deleteMin(const float& priority) {
        iterator it = pk.begin();
        float val = it->first;
        if (val + 0.02 < priority) {
            K key = it->second;
            pk.erase(it);
            kp.remove(key);
            kv.remove(key);
            return true;
        } else {
            return false;
        }
    }

    template <class K, class V>
    void Cache<K, V>::addCache(const K& key, const V& val, const float& priority) {
        //hit ,update priority
        if (kv.containsKey(key)) {
            this->updatePrority(key, priority);
        }
        else 
        {
            if (kv.size() < CACHE_SIZE) {
                this->add(key, val, priority);
            } else {
                if (this->deleteMin(priority)) {
                    this->add(key, val, priority);
                }
            }
        }
    }

}


#endif	/* PRORITYMAP_H */

