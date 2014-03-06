#include "record.h"

namespace sstore{
Record::Record() {
        this->key = "";
        this->value = "";
    }

    Record::Record(const string record) {
        this->set(record);
    }

    Record::Record(Record* record) {
        this->key = record->key;
        this->value = record->value;
    }

    Record::~Record() {

    }

    string Record::getKey() const {
        return this->key;
    }

    string Record::getValue() const {
        return this->value;
    }

    void Record::setKey(const string key) {
        this->key = key;
    }

    void Record::setValue(const string value) {
        this->value = value;
    }

    void Record::set(const string record) {
        size_t pos = record.find('=');
        this->key = record.substr(0, pos);
        this->value = record.substr(pos + 1, record.length());
    }

    bool Record::equals(const Record& record) const {
        int result = this->key.compare(record.key);
        if (result == 0) {
            return true;
        } else {
            return false;
        }
    }

    bool Record::operator ==(const Record& other) const {
        return this->equals(other);
    }

    string Record::toString() {
        stringstream convert;
        convert << "key: " << key << "\t value:  " << value;
        string toRet = convert.str();
        return toRet;
    }

}
