// recursive.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CrecursiveApp:
// �йش����ʵ�֣������ recursive.cpp
//

class CrecursiveApp : public CWinApp
{
public:
	CrecursiveApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CrecursiveApp theApp;