// dllmain.h : 模块类的声明。

class CSDOverlaysModule : public CAtlDllModuleT< CSDOverlaysModule >
{
public :
	DECLARE_LIBID(LIBID_SDOverlaysLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SDOVERLAYS, "{2B247C23-BE67-4A99-A40D-4885E87CD19F}")
};

extern class CSDOverlaysModule _AtlModule;
