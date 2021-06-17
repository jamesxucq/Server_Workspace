#pragma once

class CMountVolume {
public:
	CMountVolume(void);
	~CMountVolume(void);
private:
	DWORD m_dwMountInitFlags;
	DWORD m_dwProcessStatus;
public:
	DWORD MountInitialize(const TCHAR *szDriveLetter);
	DWORD MountFinalize(const TCHAR *szNewLetter);
};
extern class CMountVolume objMountVolume;


#define INIT_MOUNT_FLAGS(FLAGS)					FLAGS = 0x0000010F;

#define SYNCING_INIT_OKAY(FLAGS)				FLAGS |= 0x00000001
#define RIVER_INIT_OKAY(FLAGS)					FLAGS |= 0x00000002
#define DWATCH_INIT_OKAY(FLAGS)					FLAGS |= 0x00000004
#define MLINK_INIT_OKAY(FLAGS)					FLAGS |= 0x00000008
#define MOUNT_INIT_OKAY(FLAGS)					FLAGS |= 0x00000100

#define SYNCING_FINAL_OKAY(FLAGS)				FLAGS &= 0xFFFFFFFE
#define RIVER_FINAL_OKAY(FLAGS)					FLAGS &= 0xFFFFFFFD
#define DWATCH_FINAL_OKAY(FLAGS)				FLAGS &= 0xFFFFFFFB
#define MLINK_FINAL_OKAY(FLAGS)					FLAGS &= 0xFFFFFFF7
#define MOUNT_FINAL_OKAY(FLAGS)					FLAGS &= 0xFFFFFEFF

#define SYNCING_CHECK_INIT(FLAGS)				(FLAGS & 0x00000001)
#define RIVER_CHECK_INIT(FLAGS)					(FLAGS & 0x00000002)
#define DWATCH_CHECK_INIT(FLAGS)				(FLAGS & 0x00000004)
#define MLINK_CHECK_INIT(FLAGS)					(FLAGS & 0x00000008)
#define MOUNT_CHECK_INIT(FLAGS)					(FLAGS & 0x00000100)

//
namespace NMountVolumeBzl {
	inline DWORD MountInitialize(const TCHAR *szDriveLetter) 
	{ return objMountVolume.MountInitialize(szDriveLetter); }
	inline DWORD MountFinalize(const TCHAR *szNewLetter) 
	{ return objMountVolume.MountFinalize(szNewLetter); }
	//
	DWORD RemountVolume(TCHAR *szDriveLetter, const TCHAR *szNewLetter);
};
