/* 
 * File:   record.h
 * Author: root
 *
 * Created on September 24, 2013, 7:14 PM
 */

#ifndef RECORD_H
#define	RECORD_H
#include <string>
#include<sstream>
#include<vector>
using namespace std;
namespace sstore {

    class Record {
    private:
        string key;
        string value;
    public:
        Record();
        Record(const string record);
        Record(Record* record);
        ~Record();
        string getKey() const;
        string getValue() const;
        void setKey(const string key);
        void setValue(const string value);
        void set(const string record);
        bool equals(const Record& record) const;
        string toString();
        bool operator==(const Record &other) const;
    };
    
}

#endif	/* RECORD_H */

