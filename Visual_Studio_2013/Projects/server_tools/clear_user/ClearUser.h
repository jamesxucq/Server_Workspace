// ClearUser.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CClearUserApp:
// �йش����ʵ�֣������ ClearUser.cpp
//

class CClearUserApp : public CWinApp
{
public:
	CClearUserApp();

	// ��д
public:
	virtual BOOL InitInstance();

	// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CClearUserApp theApp;