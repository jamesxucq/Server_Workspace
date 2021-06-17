// dllmain.h : 模块类的声明。

class CSWDOverlaysModule : public CAtlDllModuleT< CSWDOverlaysModule >
{
public :
	DECLARE_LIBID(LIBID_SWDOverlaysLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SWDOVERLAYS, "{2B247C23-BE67-4A99-A40D-4885E87CD19F}")
};

extern class CSWDOverlaysModule _AtlModule;
