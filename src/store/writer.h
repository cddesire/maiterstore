/* 
 * File:   writer.h
 * Author: root
 *
 * Created on February 10, 2014, 8:16 AM
 */

#ifndef WRITER_H
#define	WRITER_H
#include "flat_file.h"
#include "const.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;
namespace sstore {

    class Writer {
    public:
        Writer();

        ~Writer();

        void write();

        std::vector<long> handleLine(std::string value);
        
    private:
        FlatFile vec;
        FlatFile hdr;
        fstream* in;
    };
}



#endif	/* WRITER_H */

