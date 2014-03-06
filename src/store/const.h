/* 
 * File:   const.h
 * Author: root
 *
 * Created on September 24, 2013, 9:26 PM
 */

#ifndef CONST_H
#define	CONST_H
namespace sstore {
    /**
     IO_SIZE=THREADS_BUFF_SIZE=BLOCK_SIZE/THREADS_COUNT
     */
    //#define IO_SIZE		        512


#define BLOCK_SIZE		1024 * 8

    //#define THREADS_COUNT           512 
    //
    //#define THREADS_BUFF_SIZE       512 
    /**
     The number of blocks in the input file(after insert into our sstore).
     */
#define INDEX_SIZE       512 

    
#define BUFFER_SIZE       2 

#define FETCH_SIZE       2 

#define KEY_LEN       4 

#define FILL_CHAR       '0' 

#define CACHE_SIZE       100000

#define DATA_FILE   "/media/ssd/MMap/vec"

#define META_FILE   "/media/ssd/MMap/hdr"

#define INPUT_FILE   "/media/ssd/MMap/pg_google"

#define KEYS_FILE   "/media/ssd/MMap/keys"
    
#define INFLU_FILE   "/media/ssd/MMap/influ"

    typedef long block_addr;
}

#endif	/* CONST_H */

