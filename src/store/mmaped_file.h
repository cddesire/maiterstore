/* 
 * File:   mmaped_file.h
 * Author: root
 *
 * Created on February 9, 2014, 2:33 PM
 */

#ifndef MMAPED_FILE_H
#define	MMAPED_FILE_H
#include <sys/types.h>
#include <sys/stat.h>
#include<sys/mman.h>
#include <fcntl.h>  
#include <unistd.h>
#include <string>
#include<iostream>
#include"sstore_exception.h"
#include"access_mode.h"
namespace sstore {

    class MMapedFile {
    public:

        MMapedFile() :
        fd(-1),
        memory_area(NULL),
        size_mapped(0),
        offset_mapped(0),
        reference_count(0){}

        void *getMemoryArea(void) {
            return memory_area;
        }
        
        int getFd(){
            return fd;
        }

        void* openAndMmapFile(std::string filename, enum access_mode access_mode, 
             off_t offset, size_t length, bool map_whole_file, bool allow_remap);
        
        //void* writeMmapFile(std::string filename, int size);
        
        void* readMmapFile(std::string filename);
        
        bool munmapAndCloseFile();

    private:
        int fd;
        void *memory_area;
        size_t size_mapped;
        off_t offset_mapped;
        int reference_count;
    };
    
}


#endif	/* MMAPED_FILE_H */

