/* 
 * File:   sstore_exception.h
 * Author: root
 *
 * Created on February 9, 2014, 2:28 PM
 */

#ifndef SSTORE_EXCEPTION_H
#define	SSTORE_EXCEPTION_H
#include <stdexcept>
#include<stdio.h>
namespace sstore {

    class SStoreException : public std::runtime_error {
    public:

        SStoreException(const char *msg_param) throw () :
        std::runtime_error(msg_param) {
            fprintf(stderr, "Throwing exception %s\n", msg_param);
        }

        SStoreException(std::string msg_param) throw () :
        std::runtime_error(msg_param) {
            fprintf(stderr, "Throwing exception %s\n", msg_param.c_str());
        }

        virtual ~SStoreException(void) throw () {
        }
    private:
    };
}


#endif	/* SSTORE_EXCEPTION_H */

