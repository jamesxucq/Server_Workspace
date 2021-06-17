#pragma once

//
#define INVA_ATTR				0x00000000
//
// file real status
#define ADDI                    0x00000001 // addition
#define DELE                    0x00000002 // delete
#define MODIFY                  0x00000004 // modify
#define EXIST					0x00000008 // exist
#define COPY                    0x00000010 // copy
#define MOVE                    0x00000020 // move
//
// list status
#define RECURSIVE               0x00000040 // recursive
#define LIST                    0x00000080 // list
//
// dir virtual status
#define DIRECTORY	            0x00001000 // directory
//
#define PROPERTY                0x00010000 // property
//
#define READONLY				PROPERTY | 0x0001 // read only
#define ALOCK					PROPERTY | 0x0002 // lock 
#define WRITEABLE				PROPERTY | 0x0004 // writeable
#define SHARED					PROPERTY | 0x0008 // shared
#define EXCLUSIVE				PROPERTY | 0x0010 // exclusive
//
// #define ERASE	                0x00020000 // erase
//
// file real status
#define	FILE_REAL_STAT(ATTRIB)		(0x00000FFF & ATTRIB)
#define	SETF_REAL_STAT(IATT_TYPE, ATTRIB) \
	IATT_TYPE = ((0xFFFFF000 & IATT_TYPE) | ATTRIB)
#define ATTRB_IS_DIRECTORY(ATTRIB)	(DIRECTORY & ATTRIB) // is directory
