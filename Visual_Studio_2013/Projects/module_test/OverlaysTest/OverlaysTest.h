
// OverlaysTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// COverlaysTestApp:
// �йش����ʵ�֣������ OverlaysTest.cpp
//

class COverlaysTestApp : public CWinAppEx
{
public:
	COverlaysTestApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern COverlaysTestApp theApp;