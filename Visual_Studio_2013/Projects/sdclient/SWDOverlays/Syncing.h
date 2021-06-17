// Syncing.h : CSyncing 的声明

#pragma once
#include "resource.h"       // 主符号

#include "SWDOverlays_i.h"
#include "ShlObj.h"

#include "CommonHeader.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free"，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif



// CSyncing

class ATL_NO_VTABLE CSyncing :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSyncing, &CLSID_Syncing>,
	public IDispatchImpl<ISyncing, &IID_ISyncing, &LIBID_SWDOverlaysLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IShellIconOverlayIdentifier
{
public:
	CSyncing()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_SYNCING)


	BEGIN_COM_MAP(CSyncing)
		COM_INTERFACE_ENTRY(ISyncing)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IShellIconOverlayIdentifier)
	END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	// IShellIconOverlayIdentifier Methods
	STDMETHOD(GetOverlayInfo)(LPWSTR pszIconFile,int cchMax,int *pIndex,DWORD* pdwFlags);
	STDMETHOD(GetPriority)(int* pPriority);
	STDMETHOD(IsMemberOf)(LPCWSTR pszPath,DWORD dwAttrib);

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}
public:

};

OBJECT_ENTRY_AUTO(__uuidof(Syncing), CSyncing)
