/* 
 * File:   filemanager.h
 * Author: root
 *
 * Created on September 24, 2013, 8:54 PM
 */

#ifndef FILEMANAGER_H
#define	FILEMANAGER_H
#include<string>
namespace sstore {

    class FileManager {
    private:
        std::string filename;
    public:
        FileManager(std::string filename);
        FileManager();
        ~FileManager();
        void create();
        void read(int blockStart, int blockCount, char* buffer);
        void readFully(std::string &buffer);
        void write(int blockStart, int blockCount, char* buffer);
        void appWrite(char* buffer);
    };

}
#endif	/* FILEMANAGER_H */

