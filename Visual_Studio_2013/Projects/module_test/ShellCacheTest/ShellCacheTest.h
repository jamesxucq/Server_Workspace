// ShellCacheTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CShellCacheTestApp:
// �йش����ʵ�֣������ ShellCacheTest.cpp
//

class CShellCacheTestApp : public CWinApp
{
public:
	CShellCacheTestApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CShellCacheTestApp theApp;