// AnchorTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAnchorTestApp:
// �йش����ʵ�֣������ AnchorTest.cpp
//

class CAnchorTestApp : public CWinApp
{
public:
	CAnchorTestApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAnchorTestApp theApp;