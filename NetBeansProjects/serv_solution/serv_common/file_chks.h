#ifndef FILE_CHKS_H_
#define FILE_CHKS_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_macro.h"
#include "checksum.h"

    //

    //struct simple_head {
    //    unsigned int count;
    //    struct simple_chks *chks;
    //};
    //
    //struct complex_head {
    //    unsigned int count;
    //    struct complex_chks *chks;
    //};

#pragma	pack(1)

    struct simple_chks {
        uint64 offset; // posit in file
        unsigned char md5_chks[MD5_DIGEST_LEN];
    };

    struct complex_chks {
        uint32 offset; // posit in chunk
        uint32 adler32_chk;
        unsigned char md5_chks[MD5_DIGEST_LEN];
    };
#pragma	pack()
    //
    uint32 creat_file_complex_chks(FILE **fpchks, char *file_name, uint64 offset);
    uint32 creat_chunk_complex_chks(FILE **fpchks, char *file_name, uint64 offset);
#define _CHKS_CLOSE_(fptr) if(fptr) fclose(fptr);
    //
    int chks_text_stre(char *chks_str, FILE *file_stre, uint64 file_size);
    int file_chks_name(unsigned char *chks, char *file_name, uint64 file_size);

#ifdef	__cplusplus
}
#endif

#endif /* FILE_CHKS_H_ */
