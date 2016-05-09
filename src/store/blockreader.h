/* 
 * File:   blockreader.h
 * Author: root
 *
 * Created on September 25, 2013, 10:34 AM
 */

#ifndef BLOCKREADER_H
#define	BLOCKREADER_H
#include"const.h"
#include<string>
#include <pthread.h> 
#include<stdlib.h>
#include <dirent.h>  
#include <fcntl.h>  
#include <sys/types.h>  
#include <sys/stat.h> 
#include <unistd.h> 
#include<iostream>
#include <string>  
#include <cstring> 
#include <errno.h> 
#include<fstream>
#include<stdio.h>
#include<error.h>
#include<sys/mman.h>
namespace sstore {

    class BlockReader {
        
    private:
        
        int fd;
        
        char* buffer;
        
    public:

        BlockReader();

        BlockReader(int blockIndex);

        ~BlockReader();

        std::string getBlockText(std::string filename, int blockIndex);


    };

    BlockReader::BlockReader() {
        if ((fd = open(DATA_FILE, O_RDWR)) < 0) {
            perror("open file error");
        }
    }

    BlockReader::~BlockReader() {
        close(fd);
    }

    std::string BlockReader::getBlockText(std::string filename, int blockIndex) {
        buffer = (char*) mmap(NULL, BLOCK_SIZE, PROT_READ, MAP_SHARED, fd, blockIndex * BLOCK_SIZE);
        if (buffer == MAP_FAILED) {
            perror("mmap error");
            close(fd);
            return "";
        }
        std::string texter(buffer);
        munmap(buffer, BLOCK_SIZE);
        return texter;
    }
    
}

#endif	/* BLOCKREADER_H */
