#include <fstream>
#include <iostream>
#include <cerrno>
#include"const.h"
#include"filemanager.h"
using namespace std;
namespace sstore {

    FileManager::FileManager(std::string filename) {
        this->filename = filename;
    }

    FileManager::FileManager() {
        this->filename = "";
    }

    FileManager::~FileManager() {

    }

    void FileManager::readFully(std::string &buffer) {
        std::ifstream fin(this->filename.c_str());
        getline(fin, buffer, char(-1));
        fin.close();
    }

    void FileManager::create() {
        // Create blank pages
        char buffer[BLOCK_SIZE] = {};
        const char* fileName = this->filename.c_str();
        // Write blank pages to file
        fstream out(fileName, ios::out | ios::binary);
        out.write(buffer, 4 * 4096);
        out.close();
    }

    void FileManager::read(int blockStart, int blockCount, char* buffer) {
        // Translate from block to byte offset and size
        int offset = blockStart * BLOCK_SIZE;
        int size = blockCount * BLOCK_SIZE;
        const char* fileName = this->filename.c_str();
        // Read from file to buffer
        fstream in(fileName, ios::in);
        in.seekg(offset);
        in.read(buffer, size);
        in.close();
    }

    void FileManager::write(int blockStart, int blockCount, char* buffer) {
        // Translate from block to byte offset and size
        int offset = blockStart * BLOCK_SIZE;
        int size = blockCount * BLOCK_SIZE;
        const char* fileName = this->filename.c_str();
        // Write from buffer to file
        fstream out(fileName, ios::in | ios::out);
        out.seekp(offset);
        out.write(buffer, size);
        out.close();
    }

    void FileManager::appWrite(char* buffer) {
        const char* fileName = this->filename.c_str();
        fstream out(fileName, ios::in | ios::out | ios::app);
        out.write(buffer, BLOCK_SIZE);
        out.close();
    }

}