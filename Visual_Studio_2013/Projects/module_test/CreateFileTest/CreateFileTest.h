// CreateFileTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCreateFileTestApp:
// �йش����ʵ�֣������ CreateFileTest.cpp
//

class CCreateFileTestApp : public CWinApp
{
public:
	CCreateFileTestApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCreateFileTestApp theApp;