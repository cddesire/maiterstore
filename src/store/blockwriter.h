/* 
 * File:   blockwriter.h
 * Author: root
 *
 * Created on September 28, 2013, 3:26 PM
 */

#ifndef BLOCKWRITER_H
#define	BLOCKWRITER_H
#include<string>
#include<boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include"const.h"
#include<list>
#include<vector>
using namespace std;
using namespace boost;
namespace sstore {

    class BlockWriter {
    private:
        /**
         master1#master2#.........
         block1#block2#.....
         */
        string masters;
        string buffer;
        string keys;
        fstream* in;
        fstream* out;
    public:

        BlockWriter(string infile, string outfile);

        ~BlockWriter();

        void write(int blockStart, const char* buf, int len);

        void fillBuffer();

        string getMasters()const;

        void setMasters(string buf);

        void saveMasters(string metafile);

        void saveKeys(string keysfile);

        void batchedWrite();

        string handleLine(string line);

    };

    BlockWriter::BlockWriter(string infile, string outfile) {
        in = new fstream;
        in->open(infile.c_str(), ios::in);
        out = new fstream;
        out->open(outfile.c_str(), ios::out);

    }

    BlockWriter::~BlockWriter() {
        in->close();
        out->close();
        delete in;
        delete out;
    }

    string BlockWriter::getMasters() const {
        return this->masters;
    }

    void BlockWriter::write(int blockStart, const char* buf, int len) {
        int offset = blockStart * BLOCK_SIZE;
        out->seekp(offset, ios::beg);
        out->write(buf, len);
        out->flush();
    }

    void BlockWriter::fillBuffer() {
        while (buffer.size() < BLOCK_SIZE) {
            buffer.append("#");
        }
    }

    void BlockWriter::batchedWrite() {
        string line;
        int i = 0;
        while (std::getline(*in, line, '\n')) {
            if ((buffer.size() + line.size()) > BLOCK_SIZE) {
                this->setMasters(buffer);
                fillBuffer();
                this->write(i, buffer.c_str(), buffer.size());
                buffer.clear();
                i++;
            }
            string hl = this->handleLine(line);
            buffer.append(hl);
            line.clear();
        }//end while

        //last block
        if (buffer.size() != 0) {
            this->setMasters(buffer);
            this->write(i, buffer.c_str(), buffer.size());
             fillBuffer();
        }
    }

    string BlockWriter::handleLine(string line) {
        trim_right(line);
        vector<string> strs;
        boost::split(strs, line, boost::is_any_of("\t"));
        line = strs[0] + "=" + strs[1] + "#";
        this->keys.append(strs[0] + "#");
        return line;
    }

    void BlockWriter::setMasters(string buf) {
        iterator_range<string::iterator> rge;
        rge = find_first(buf, "=");
        string master = buf.substr(0, rge.begin() - buf.begin()) + "#";
        this->masters.append(master);
    }

    void BlockWriter::saveMasters(string metafile) {
        fstream* out1 = new fstream;
        out1->open(metafile.c_str(), ios::out);
        out1->write(this->masters.c_str(), masters.length());
        out1->flush();
        out1->close();
        delete out1;
    }

    void BlockWriter::saveKeys(string keysfile) {
        fstream* out1 = new fstream;
        out1->open(keysfile.c_str(), ios::out);
        out1->write(this->keys.c_str(), keys.length());
        out1->flush();
        out1->close();
        delete out1;
    }
    
    
}
#endif	/* BLOCKWRITER_H */

