#include"reader.h"

namespace sstore {

    Reader::Reader() {
        hdr_ptr = (long*) hdrFile.readMmapFile(std::string(META_FILE));
        data_ptr = (long*) dataFile.readMmapFile(std::string(DATA_FILE));
    }

    Reader::~Reader() {
        hdrFile.munmapAndCloseFile();
        dataFile.munmapAndCloseFile();
    }

    std::vector<long> Reader::read(long index) {
        long pos = hdr_ptr[index];
        long dist = hdr_ptr[index + 1] - hdr_ptr[index];
        std::vector<long> vec(data_ptr + pos, data_ptr + pos + dist);
        return vec;
    }

}
