#ifndef _AR_H
#define _AR_H

#include <sys/cdefs.h>

/* Archive files start with the ARMAG identifying string.  Then follows a
   `struct ar_hdr', and as many bytes of member file data as its `ar_size'
   member indicates, for each member file.  */

#define ARMAG        "!<arch>\n"        /* String that begins an archive file.  */
#define SARMAG        8                /* Size of that string.  */

#define ARFMAG        "`\n"                /* String in ar_fmag at end of each header.  */

#define AR_NAME_SIZE 16
#define AR_DATE_SIZE 12
#define AR_UID_SIZE 6
#define AR_GID_SIZE 6
#define AR_MODE_SIZE 8 
#define AR_FSIZE_SIZE 10
#define AR_FMAG_SIZE 2
#define AR_HEADER_SIZE 60
typedef struct Ar_hdr {
    char ar_name[AR_NAME_SIZE];                /* Member file name, sometimes / terminated. */
    char ar_date[AR_DATE_SIZE];                /* File date, decimal seconds since Epoch. Same as mtime */ 
    char ar_uid[AR_UID_SIZE], ar_gid[AR_GID_SIZE];       /* User and group IDs, in ASCII decimal.  */
    char ar_mode[AR_MODE_SIZE];                /* File mode, in ASCII octal.  */
    char ar_size[AR_FSIZE_SIZE];                /* File size, in ASCII decimal.  */
    char ar_fmag[AR_FMAG_SIZE];                /* Always contains ARFMAG.  */
}Ar_hdr_t;



#endif /* ar.h */