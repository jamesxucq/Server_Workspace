#pragma once

//
typedef		UINT	ErrCode;
typedef		UINT	ec;

//
#define		ERR_SUCCESS                                             0
#define		ERR_FAULT												-1
#define		ERR_EXCEPTION                                           1

/************************************************************************/
/*                    View Error Code Define                            */
/************************************************************************/
#define  VIEW_BASE											0x00010000
#define  ERR_INIT_ENV										VIEW_BASE | 0x0001
#define  ERR_FINAL_ENV										VIEW_BASE | 0x0002
#define  ERR_LOAD_CONFIG									VIEW_BASE | 0x0003
#define  ERR_GET_SESSION									VIEW_BASE | 0x0005

#define  ERR_CREAT_TRAYICON									VIEW_BASE | 0x0007
#define  ERR_GET_SYNCH										VIEW_BASE | 0x0009
//#define  ERR_GET_SESSION									VIEW_BASE | 0x0005

/************************************************************************/
/*                   Ctrl Error Code Define                             */
/************************************************************************/
#define  CTRL_BASE											0x00030000
#define  ERR_UNMOUNT										CTRL_BASE | 0x0001
#define  ERR_MOUNT											CTRL_BASE | 0x0003
#define  ERR_COPY_DATA										CTRL_BASE | 0x0005
#define  ERR_NEW_MOUNT										CTRL_BASE | 0x0007
#define  ERR_EXIST_MOUNT									CTRL_BASE | 0x0009
#define  ERR_FREE_SPACE										CTRL_BASE | 0x000B
#define  ERR_DISK_RESIZE									CTRL_BASE | 0x000F
#define  ERR_SHRINK_FILE									CTRL_BASE | 0x0011
#define  ERR_EXPAND_FILE									CTRL_BASE | 0x0013
#define  ERR_CHECK_REDUCE									CTRL_BASE | 0x0015
#define  ERR_REMOUNT										CTRL_BASE | 0x0017

/************************************************************************/
/*                    Mode Error Code Define                            */
/************************************************************************/
#define  MODE_BASE											0x00050000
#define  ERR_CONFIG											MODE_BASE | 0x0001
//#define  ERR_LOAD_CONFIG									MODE_BASE | 0x0003

/************************************************************************/
/*                                                                      */
/************************************************************************/