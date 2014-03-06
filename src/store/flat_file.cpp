#include "mmaped_file.h"
namespace sstore{
 off_t filesize(int fd, std::string fname) {
        struct stat buf;
        if (fstat(fd, &buf) < 0) {
            std::cerr << "stat \n";
            throw SStoreException("Cannot stat file" + fname);
        }
        return buf.st_size;
    }

    void* MMapedFile::readMmapFile(std::string filename){
        if (filename.c_str()[0] == '\0') {
            throw SStoreException("filename is empty.");
        }

        if (fd == -1) {
            fd = open(filename.c_str(), O_RDONLY);
            if (fd < 0) {
                throw SStoreException("Error opening file " + filename);
            }
        }
        this->size_mapped = filesize(fd, filename);
        memory_area = mmap(0, size_mapped, PROT_READ, MAP_SHARED, fd, 0);

        if (memory_area == MAP_FAILED) {
            throw SStoreException("Error in mmap " + filename);
        }
        return this->memory_area;
    }
    
//    void* MMapedFile::writeMmapFile(std::string filename, int size) {
//        std::clog << "open and mmap file\n";
//        if (filename.c_str()[0] == '\0') {
//            throw SStoreException("filename is empty.");
//        }
//
//        if (fd == -1) {
//            fd = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, (mode_t) 0600);
//            if (fd < 0) {
//                throw SStoreException("Error opening file " + filename);
//            }
//        }
//
//        
//        memory_area = mmap(0, size * sizeof (int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
//
//        if (memory_area == MAP_FAILED) {
//            throw SStoreException("Error in mmap " + filename);
//        }
//        return this->memory_area;
//    }

    void *MMapedFile::openAndMmapFile(std::string filename, enum access_mode access_mode,
            off_t offset, size_t length, bool map_whole_file, bool allow_remap) {
        if (filename.c_str()[0] == '\0') {
            throw SStoreException("filename is empty.");
        }
        int mode;
        int prot;
        int mmap_mode = 0;
        off_t offset_to_map;
        size_t length_to_map;
        void *address_to_map = NULL;

        if (memory_area != NULL) {
            address_to_map = memory_area;

            /* do not use MAP_FIXED, since that may invalidate other memory
               areas in the process, such as shared libraries, which would 
               lead to a mystic Segfault. */
        }
        switch (access_mode) {
            case READ_ONLY: mode = O_RDONLY;
                prot = PROT_READ;
                mmap_mode |= MAP_SHARED;
                break;
            case READ_WRITE_SHARED: mode = O_RDWR | O_APPEND | O_CREAT;
                prot = PROT_READ | PROT_WRITE;
                mmap_mode |= MAP_SHARED;
                break;
            case READ_WRITE_PRIVATE: mode = O_RDONLY;
                prot = PROT_READ | PROT_WRITE;
                mmap_mode |= MAP_PRIVATE;
                break;
            default: throw SStoreException("Internal error");
                break;
        }

        if (fd == -1) {
            fd = open(filename.c_str(), mode);
            if (fd < 0) {
                throw SStoreException("Error opening file " + filename);
            }
        }
        if (map_whole_file) {
            offset_to_map = 0;
            length_to_map = filesize(fd, filename);
        } else {
            offset_to_map = ALIGN_TO_PAGE(offset);
            length_to_map = UPPER_ALIGN_TO_PAGE(length);
        }

        if (offset_to_map == offset_mapped && length_to_map == size_mapped) {
            reference_count++;
            return ((char*) memory_area)+offset - offset_mapped;
        }
        if (offset_to_map >= offset_mapped && length_to_map + offset_to_map - offset_mapped <= size_mapped) {
            reference_count++;
            return ((char*) memory_area)+offset - offset_mapped;
        }

        if (memory_area != NULL) {
            if (munmap(memory_area, size_mapped) < 0) {
                throw SStoreException("Error in munmap file " + filename);
            }
        }

        memory_area = mmap(address_to_map, length_to_map, prot, mmap_mode, fd, offset_to_map);
        if (address_to_map != NULL && !allow_remap && memory_area != MAP_FAILED && memory_area != address_to_map) {
            if (munmap(memory_area, length_to_map) < 0) {
                throw SStoreException("Error in munmap" + filename);
            }
            throw SStoreException("Request to remap area but allow_remap is not given (remapping " + filename + ")");
        }

        if (memory_area == MAP_FAILED) {
            throw SStoreException("Error in mmap " + filename);
        }
        offset_mapped = offset_to_map;
        size_mapped = length_to_map;
        reference_count++;

        void *ret = ((char*) memory_area) + offset - offset_to_map;
        // assert(ret >= memory_area && ret < (char*)memory_area+size_mapped);

        return ret;
    }

    bool MMapedFile::munmapAndCloseFile() {
        if (munmap(memory_area, size_mapped) < 0) {
            throw SStoreException("Error in munmap");
        }
        if (close(fd)) {
            throw SStoreException("Error in close");
        }
        fd = -1;
        return true;
    }

}
