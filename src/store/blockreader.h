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

        //std::string getText()const;

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

    //    pthread_mutex_t lock;
    //
    //    struct thread_block {
    //        int infd; //source file handler  
    //        size_t start; // the start position of block
    //        int index; // the number of block
    //    };

    //    void copy_array(int start, const char* src, int readLen) {
    //                memcpy(buffer + start, src, readLen);
    //    }

    //   void* thread_copy_fn(void *arg) {
    //        struct thread_block *block = (struct thread_block *) arg;
    //        char buf[THREADS_BUFF_SIZE];
    //        int ret;
    //        size_t offset = block->index * BLOCK_SIZE;
    //        pthread_mutex_lock(&lock);
    //        ret = lseek(block->infd, offset + block->start, SEEK_SET);
    //        int bytes_read = read(block->infd, buf, sizeof (buf));
    //        if (bytes_read > 0) {
    //            copy_array(block->start, buf, bytes_read);
    //        }
    //        pthread_mutex_unlock(&lock);
    //        pthread_exit(NULL);
    //    }

    std::string BlockReader::getBlockText(std::string filename, int blockIndex) {
        //        int infd = open(filename.c_str(), O_RDONLY);
        //        if (infd == -1) {
        //            std::cout << "error while open file " << filename << std::endl;
        //            return "";
        //        }
        //
        //        if (pthread_mutex_init(&lock, NULL) != 0) {
        //            std::cout << "\n mutex init failed\n";
        //            return "";
        //        }
        //
        //        //texter.clear();
        //        struct thread_block *blocks = (struct thread_block *)
        //                malloc(sizeof (struct thread_block)* THREADS_COUNT);
        //
        //        int i = 0;
        //        //init-thread-block  
        //        for (; i < THREADS_COUNT; ++i) {
        //            blocks[i].infd = infd;
        //            blocks[i].start = i * THREADS_BUFF_SIZE;
        //            blocks[i].index = blockIndex;
        //        }
        //
        //        pthread_t ptid[THREADS_COUNT];
        //
        //        for (i = 0; i < THREADS_COUNT; ++i) {
        //            pthread_create(&ptid[i], NULL, thread_copy_fn, &(blocks[i]));
        //        }
        //        ///Join  
        //        for (i = 0; i < THREADS_COUNT; ++i) {
        //            pthread_join(ptid[i], NULL);
        //        }
        //        ///release  
        //        free(blocks);
        //        close(infd);
        //        pthread_mutex_destroy(&lock);
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
