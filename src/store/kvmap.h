#ifndef KVMAP_H
#define KVMAP_H
#include <map>
using namespace std;
namespace sstore {

    template <class K, class V>
    class KVMap {
    public:
        KVMap();
        virtual ~KVMap();
        KVMap(const KVMap& other);
        
        void add(const K& key, const V& val);

        void remove(const K& key);

        bool containsKey(const K& key) const;

        typedef typename map<K, V>::iterator iterator;

        iterator begin();

        iterator end();

        void erase(iterator position);

        void clear();

        typedef typename map<K, V>::size_type size_type;

        size_type size() const;

        V& operator[](const K& key);

        void show();
        
    protected:
        
    private:
        map<K, V> _elements;
    };

    template <class K, class V>
    KVMap<K, V>::KVMap() {}

    template <class K, class V>
    KVMap<K, V>::~KVMap() {}

    template <class K, class V>
    KVMap<K, V>::KVMap(const KVMap& other) {
        this->_elements = other._elements;
    }

    template <class K, class V>
    typename KVMap<K, V>::iterator KVMap<K, V>::begin() {
        return _elements.begin();
    }

    template <class K, class V>
    typename KVMap<K, V>::iterator KVMap<K, V>::end() {
        return _elements.end();
    }

    template <class K, class V>
    void KVMap<K, V>::show() {
        iterator it = this->_elements.begin();
        for (; it != this->_elements.end(); it++) {
            std::cout << (*it).first << " => "<< '\n';
        }
    }

    template <class K, class V>
    void KVMap<K, V>::erase(iterator position) {
        return _elements.erase(position);
    }

    template <class K, class V>
    void KVMap<K, V>::clear() {
        return _elements.clear();
    }

    template <class K, class V>
    typename KVMap<K, V>::size_type KVMap<K, V>::size() const {
        return _elements.size();
    }

    template <class K, class V>
    void KVMap<K, V>::remove(const K& key) {
        typename map<K, V>::iterator i = _elements.find(key);
        if (i != _elements.end()) {
            _elements.erase(i);
        }
    }

    template <class K, class V>
    void KVMap<K, V>::add(const K& key, const V& val) {
        typename map<K, V>::const_iterator i = _elements.find(key);
        if (i != _elements.end()) {
            _elements.erase(key);
        }
        _elements.insert(pair<K, V> (key, val));
    }

    template <class K, class V>
    V& KVMap<K, V>::operator [](const K& key) {
        typename map<K, V>::iterator i = _elements.find(key);
        //if (i != _elements.end()) {
        return i->second;
        // }
    }

    template <class K, class V>
    bool KVMap<K, V>::containsKey(const K& key) const {
        typename map<K, V>::const_iterator i = _elements.find(key);
        if (i != _elements.end()) {
            return true;
        } else {
            return false;
        }
    }

}
#endif 
