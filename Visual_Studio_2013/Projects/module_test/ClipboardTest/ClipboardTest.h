// ClipboardTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CClipboardTestApp:
// �йش����ʵ�֣������ ClipboardTest.cpp
//

class CClipboardTestApp : public CWinApp
{
public:
	CClipboardTestApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CClipboardTestApp theApp;