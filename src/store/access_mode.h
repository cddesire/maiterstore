/* 
 * File:   access_mode.h
 * Author: root
 *
 * Created on February 9, 2014, 3:20 PM
 */

#ifndef ACCESS_MODE_H
#define	ACCESS_MODE_H

#define ALIGN_TO_PAGE(x) ((x) & ~(getpagesize() - 1))
#define UPPER_ALIGN_TO_PAGE(x) ALIGN_TO_PAGE((x)+(getpagesize()-1))
#define OFFSET_INTO_PAGE(x) ((x) & (getpagesize() - 1))


namespace sstore {

    enum access_mode {
        READ_ONLY, /* Readonly modus */
        READ_WRITE_PRIVATE, /* Read/write access, writes are not propagated to disk */
        READ_WRITE_SHARED /* Read/write access, writes are propagated to disk (file is modified) */
    };
}

#endif	/* ACCESS_MODE_H */

