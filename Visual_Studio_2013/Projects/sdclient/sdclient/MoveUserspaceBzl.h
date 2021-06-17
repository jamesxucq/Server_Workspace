#pragma once

class CMoveUserspace {
public:
	CMoveUserspace(void);
	~CMoveUserspace(void);
private:
	DWORD m_dwMoveInitFlags;
public:
	DWORD MoveInitialize();
	DWORD MoveFinalize(TCHAR *szLocation);
};
extern class CMoveUserspace objMoveUserspace;


#define INIT_MOVE_FLAGS(FLAGS)					FLAGS = 0x0000010F;

#define MOUNT_INIT_OKAY(FLAGS)					FLAGS |= 0x00000001
#define ANCH_INIT_OKAY(FLAGS)					FLAGS |= 0x00000002
#define SYSLOG_INIT_OKAY(FLAGS)					FLAGS |= 0x00000004
#define RIVER_INIT_OKAY(FLAGS)					FLAGS |= 0x00000008
#define DWATCH_INIT_OKAY(FLAGS)					FLAGS |= 0x00000100
//#define WORKER_INIT_OKAY(FLAGS)				FLAGS |= 0x00000008

#define MOUNT_FINAL_OKAY(FLAGS)					FLAGS &= 0xFFFFFFFE
#define ANCH_FINAL_OKAY(FLAGS)					FLAGS &= 0xFFFFFFFD
#define SYSLOG_FINAL_OKAY(FLAGS)				FLAGS &= 0xFFFFFFFB
#define RIVER_FINAL_OKAY(FLAGS)					FLAGS &= 0xFFFFFFF7
#define DWATCH_FINAL_OKAY(FLAGS)				FLAGS &= 0xFFFFFEFF
//#define WORKER_FINAL_OKAY(FLAGS)				FLAGS &= 0xFFFFFFF7

#define MOUNT_CHECK_INIT(FLAGS)					(FLAGS & 0x00000001)
#define ANCH_CHECK_INIT(FLAGS)					(FLAGS & 0x00000002)
#define SYSLOG_CHECK_INIT(FLAGS)				(FLAGS & 0x00000004)
#define RIVER_CHECK_INIT(FLAGS)					(FLAGS & 0x00000008)
#define DWATCH_CHECK_INIT(FLAGS)				(FLAGS & 0x00000100)
//#define WORKER_CHECK_INIT(FLAGS)				(FLAGS & 0x00000008)

//
namespace NMoveSpaceBzl {
	//
	DWORD MoveFileSWD(VOID *stcProgressCtrl, TCHAR *szNewDirectory, TCHAR *szExistDirectory);
	DWORD MoveUserspace(CWnd* pDisplayWnd, TCHAR *szExistLocation, TCHAR *szNewLocation, TCHAR *szFileName, DWORD dwFileSize,  const TCHAR *szDriveLetter);
};