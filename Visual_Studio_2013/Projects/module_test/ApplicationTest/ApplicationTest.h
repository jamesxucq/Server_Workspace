// ApplicationTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CApplicationTestApp:
// �йش����ʵ�֣������ ApplicationTest.cpp
//

class CApplicationTestApp : public CWinApp
{
public:
	CApplicationTestApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CApplicationTestApp theApp;