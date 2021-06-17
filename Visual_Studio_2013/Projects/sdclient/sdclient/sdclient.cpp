// sdclient.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"

#include "sdclient.h"
#include "ApplicaBzl.h"
#include "ValidateDlg.h"
#include "ClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSDClientApp

BEGIN_MESSAGE_MAP(CSDClientApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSDClientApp ����

CSDClientApp::CSDClientApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CSDClientApp ����

CSDClientApp theApp;


// CSDClientApp ��ʼ��

BOOL CSDClientApp::InitInstance()
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
//
	CWinApp::InitInstance();
//
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
	TCHAR szDriveLetter[32];
	DWORD dwExecuteType = NApplicaBzl::CheckEnviron(szDriveLetter);
	switch(dwExecuteType) {
	case EXECUTE_FAULT:
		AfxMessageBox(_T("�������,�˳�����!"), MB_OK|MB_ICONSTOP|MB_SYSTEMMODAL); 
		return FALSE;
		break;
	case EXECUTE_ONE_INSTANCE:
		AfxMessageBox(_T("����Ѿ�������!"), MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL);
		return FALSE;
		break;
	case EXECUTE_CONFIGURATION:
		AfxMessageBox(_T("û�������û��������ļ�!\n�������� login wizard"), MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
		NApplicaBzl::RunLoginWizard(); 
		return FALSE;
		break;
	case EXECUTE_NOACCOUNT:
		AfxMessageBox(_T("�˼����û�������û�!\n�������� login wizard"), MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL);
		NApplicaBzl::RunLoginWizard(); 
		return FALSE;
		break;
	default:
		break;	
	}
	//
	if(EXECUTE_USER_SPACE & dwExecuteType) {
		AfxMessageBox(_T("�û���������!\n�������� login wizard"), MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL);
		NApplicaBzl::RunLoginWizard();
		return FALSE;	
	}
	if((EXECUTE_ACTIVE_UPDATE|EXECUTE_LIVE_UPDATE) & dwExecuteType) {
		NApplicaBzl::RunLiveUpdate();
		if(EXECUTE_ACTIVE_UPDATE & dwExecuteType) 
			return FALSE;	
	}
	TCHAR szPromptTyxt[64];
	if(EXECUTE_LETTER_OCCPUY &dwExecuteType) {
		_stprintf_s(szPromptTyxt, _T("%s��ռ��,�Զ�ѡ�������̷�."), szDriveLetter);
		if(IDYES != AfxMessageBox(szPromptTyxt, MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL))
			NApplicaBzl::ShiftDriveLetter();
	}
	if(EXECUTE_DRIVE_MOUNTED & dwExecuteType) {
		_stprintf_s(szPromptTyxt, _T("%s�Ѿ�����,�Ƿ�ʹ�ù��ص�����?"), szDriveLetter);
		if(IDYES != AfxMessageBox(szPromptTyxt, MB_YESNO|MB_ICONWARNING|MB_DEFBUTTON1|MB_SYSTEMMODAL)) {
			NApplicaBzl::RunLoginWizard();	
			return FALSE;	
		}
	}
	//
	if(EXECUTE_PASSWD_PROTECT & dwExecuteType) {
		CValidateDlg dlg;
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK) {
			// TODO: �ڴ˷��ô����ʱ�á�ȷ�������رնԻ���Ĵ���
			CClientDlg dlg;
			m_pMainWnd = &dlg;
			INT_PTR nResponse = dlg.DoModal();
			if (nResponse == IDOK) {
				// TODO: �ڴ˷��ô����ʱ�á�ȷ�������رնԻ���Ĵ���
			} else if (nResponse == IDCANCEL) {
				// TODO: �ڴ˷��ô����ʱ�á�ȡ�������رնԻ���Ĵ���
			}
		} else if (nResponse == IDCANCEL) {
			// TODO: �ڴ˷��ô����ʱ�á�ȡ�������رնԻ���Ĵ���
		}
	} else {
		CClientDlg dlg;
		m_pMainWnd = &dlg;
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK) {
			// TODO: �ڴ˷��ô����ʱ�á�ȷ�������رնԻ���Ĵ���
		} else if (nResponse == IDCANCEL) {
			// TODO: �ڴ˷��ô����ʱ�á�ȡ�������رնԻ���Ĵ���
		}	
	}
	// NApplicaBzl::ClearEnviron(); // disable by james 20130506
	//
	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}