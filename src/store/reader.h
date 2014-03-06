/* 
 * File:   reader.h
 * Author: root
 *
 * Created on February 10, 2014, 9:17 AM
 */

#ifndef READER_H
#define	READER_H
#include <vector>
#include "mmaped_file.h"
#include "const.h"
using namespace std;
namespace sstore {

    class Reader {
    public:
        Reader();

        ~Reader();

        std::vector<long> read(long index);

    private:
        MMapedFile dataFile;
        MMapedFile hdrFile;
        long* data_ptr;
        long* hdr_ptr;
    };
}


#endif	/* READER_H */

