// LoginWizard.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "logiwiza.h"

#include "LoginWizdBzl.h"
#include "LoginWizdDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLoginWizardApp

BEGIN_MESSAGE_MAP(CLoginWizardApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CLoginWizardApp ����

CLoginWizardApp::CLoginWizardApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CLoginWizardApp ����

CLoginWizardApp theApp;


// CLoginWizardApp ��ʼ��

BOOL CLoginWizardApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	//
	NPrivilege::DrivVoluPrivi();
	DWORD dwExecuteType = LoginWizdBzl::ApplicaExecuteType();
	switch(dwExecuteType) {
	case EXECUTE_FAULT:
		AfxMessageBox(_T("����ڲ�����!"), MB_OK|MB_ICONSTOP|MB_SYSTEMMODAL); return FALSE;
		break;
	case EXECUTE_ONE_INSTANCE:
		AfxMessageBox(_T("����Ѿ�������!"), MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL);	return FALSE;
		break;
	default:
		break;	
	}
//
	CLoginWizdDlg dlg;
	m_pMainWnd = &dlg;
	LoginWizdBzl::SetApplicaMainDlg(&dlg);
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
