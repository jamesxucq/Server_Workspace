/* 
 * File:   iattb_type.h
 * Author: Administrator
 */

#ifndef IATTB_TYPE_H
#define	IATTB_TYPE_H

#ifdef	__cplusplus
extern "C" {
#endif

//
#define INVA_ATTR		0x00000000
//
// file real status
#define ADDI                    0x00000001 // addition
#define DELE                    0x00000002 // delete
#define MODIFY                  0x00000004 // modify
#define EXIST                   0x00000008 // exist
#define COPY                    0x00000010 // copy
#define MOVE                    0x00000020 // move
//
// list status
#define RECURSIV                0x00000040 // recursive
#define LIST                    0x00000080 // list
//
// dir virtual status
#define DIRECTORY	        0x00001000 // directory
//
#define PROPERTY                0x00010000 // property
//
#define READONLY                PROPERTY | 0x0001 // read only
#define ALOCK			PROPERTY | 0x0002 // lock 
#define WRITEABLE		PROPERTY | 0x0004 // writeable
#define SHARED			PROPERTY | 0x0008 // shared
#define EXCLUSIVE		PROPERTY | 0x0010 // exclusive
//
// #define ERASE	                0x00020000 // erase

//
#define REALITY_STAT(ACTION_STATUS)	(0x00000FFF&ACTION_STATUS)

#ifdef	__cplusplus
}
#endif

#endif	/* IATTB_TYPE_H */

