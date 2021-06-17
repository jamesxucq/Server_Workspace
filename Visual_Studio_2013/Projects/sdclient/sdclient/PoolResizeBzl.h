#pragma once
#include "clientcom/clientcom.h"
#include "LocalBzl.h"

class CPoolResize {
public:
	CPoolResize(void);
	~CPoolResize(void);
private:
	DWORD m_dwResizInitFlags;
public:
	DWORD ResizeInitialize();
	DWORD ResizeFinalize();
};
extern class CPoolResize objPoolResize;


#define INIT_PRESIZ_FLAGS(FLAGS)				FLAGS = 0;

#define MOUNT_INIT_OKAY(FLAGS)					FLAGS |= 0x00000001
#define RIVE_INIT_OKAY(FLAGS)					FLAGS |= 0x00000002
#define WATCH_INIT_OKAY(FLAGS)					FLAGS |= 0x00000004
//#define WORKER_INIT_OKAY(FLAGS)				FLAGS |= 0x00000008

#define MOUNT_FINAL_OKAY(FLAGS)					FLAGS &= 0xFFFFFFFE
#define RIVE_FINAL_OKAY(FLAGS)					FLAGS &= 0xFFFFFFFD
#define WATCH_FINAL_OKAY(FLAGS)					FLAGS &= 0xFFFFFFFB
//#define WORKER_FINAL_OKAY(FLAGS)				FLAGS &= 0xFFFFFFF7

#define MOUNT_CHECK_INIT(FLAGS)					(FLAGS & 0x00000001)
#define RIVE_CHECK_INIT(FLAGS)					(FLAGS & 0x00000002)
#define WATCH_CHECK_INIT(FLAGS)					(FLAGS & 0x00000004)
//#define WORKER_CHECK_INIT(FLAGS)				(FLAGS & 0x00000008)


//
//
namespace NPoolResizeBzl {
	//
	// 本地数据空间操纵
	static DWORD ShrinkPoolLength(TCHAR *szFileName, int dwLocalNewSize);
	static DWORD ExpandPoolLength(TCHAR *szFileName, int dwLocalNewSize);
	//
	DWORD AdjustPoolLength(struct ClientConfig *pClientConfig, int dwLocalExistSize, int dwLocalNewSize);
};
