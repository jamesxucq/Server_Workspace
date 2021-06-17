// Normal.h : CNormal ������

#pragma once
#include "resource.h"       // ������

#include "SWDOverlays_i.h"
#include "ShlObj.h"

#include "CommonHeader.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free"��ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
#endif



// CNormal

class ATL_NO_VTABLE CNormal :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CNormal, &CLSID_Normal>,
	public IDispatchImpl<INormal, &IID_INormal, &LIBID_SWDOverlaysLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IShellIconOverlayIdentifier
{
public:
	CNormal()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_NORMAL)


	BEGIN_COM_MAP(CNormal)
		COM_INTERFACE_ENTRY(INormal)
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

OBJECT_ENTRY_AUTO(__uuidof(Normal), CNormal)