// logiwiza.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLoginWizardApp:
// �йش����ʵ�֣������ LoginWizard.cpp
//

class CLoginWizardApp : public CWinApp
{
public:
	CLoginWizardApp();

	// ��д
public:
	virtual BOOL InitInstance();

	// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CLoginWizardApp theApp;