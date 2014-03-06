/* 
 * File:   flat_file.h
 * Author: root
 *
 * Created on February 9, 2014, 7:53 PM
 */

#ifndef FLAT_FILE_H
#define	FLAT_FILE_H
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
namespace sstore {

    class FlatFile {
    public:
        FlatFile();
        
        FlatFile(std::string filename);

        ~FlatFile();
        /**
         * write a long value to file.
         * @param value
         */
        void flatwrite(long value);

        /**
         * write a array of type long to file, the length of
         * array is length.
         * @param value
         * @param size
         */
        void flatwrite(long* value, int size);

    private:
        int fd;

    };
    
     FlatFile::FlatFile(){
        fd = -1;
    }
    
    FlatFile::FlatFile(std::string filename){
        fd = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
        if(fd < 0){
            std::clog<<"open file "<< filename <<" error\n";
        }
    }
    
    FlatFile::~FlatFile(){
        close(fd);
    }
    
    void FlatFile::flatwrite(long value){
        write(fd, &value, sizeof(long));
    }
    
    void FlatFile::flatwrite(long* value, int size){
        write(fd, value, size * sizeof(long));
    }

}


#endif	/* FLAT_FILE_H */

