// ProxySocketsT.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CProxySocketsTApp:
// �йش����ʵ�֣������ ProxySocketsT.cpp
//

class CProxySocketsTApp : public CWinApp
{
public:
	CProxySocketsTApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CProxySocketsTApp theApp;