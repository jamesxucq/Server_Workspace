/* 
 * File:   main.h
 * Author: Divad
 *
 * Created on 2011
 */

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

#define MD5_DIGEST_LEN 16
#define MD5_DIGEST_LEN 16
// #define MD5_TEXT_LENGTH	(MD5_DIGEST_LEN * 2 + 1)
#define MD5_TEXT_LENGTH	33

#define CSUM_CHUNK 4096
#define MAX_MAP_SIZE                    0x80000 // 512K
#ifndef uchar
#define uchar unsigned char
#endif    

#ifndef uint32
#define uint32 unsigned int
#endif  

#ifndef int32
#define int32 int
#endif  

#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

