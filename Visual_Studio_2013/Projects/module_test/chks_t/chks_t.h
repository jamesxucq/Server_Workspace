// chks_t.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Cchks_tApp:
// �йش����ʵ�֣������ chks_t.cpp
//

class Cchks_tApp : public CWinApp
{
public:
	Cchks_tApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Cchks_tApp theApp;