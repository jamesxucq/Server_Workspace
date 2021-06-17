// sdclient.cpp : 定义应用程序的类行为。
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


// CSDClientApp 构造

CSDClientApp::CSDClientApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CSDClientApp 对象

CSDClientApp theApp;


// CSDClientApp 初始化

BOOL CSDClientApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
//
	CWinApp::InitInstance();
//
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	//
	NPrivilege::DrivVoluPrivi();
	TCHAR szDriveLetter[32];
	DWORD dwExecuteType = NApplicaBzl::CheckEnviron(szDriveLetter);
	switch(dwExecuteType) {
	case EXECUTE_FAULT:
		AfxMessageBox(_T("软件出错,退出运行!"), MB_OK|MB_ICONSTOP|MB_SYSTEMMODAL); 
		return FALSE;
		break;
	case EXECUTE_ONE_INSTANCE:
		AfxMessageBox(_T("软件已经在运行!"), MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL);
		return FALSE;
		break;
	case EXECUTE_CONFIGURATION:
		AfxMessageBox(_T("没有配置用户和数据文件!\n重新运行 login wizard"), MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
		NApplicaBzl::RunLoginWizard(); 
		return FALSE;
		break;
	case EXECUTE_NOACCOUNT:
		AfxMessageBox(_T("此计算机没有配置用户!\n重新运行 login wizard"), MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL);
		NApplicaBzl::RunLoginWizard(); 
		return FALSE;
		break;
	default:
		break;	
	}
	//
	if(EXECUTE_USER_SPACE & dwExecuteType) {
		AfxMessageBox(_T("用户数据有误!\n重新运行 login wizard"), MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL);
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
		_stprintf_s(szPromptTyxt, _T("%s被占用,自动选用其它盘符."), szDriveLetter);
		if(IDYES != AfxMessageBox(szPromptTyxt, MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL))
			NApplicaBzl::ShiftDriveLetter();
	}
	if(EXECUTE_DRIVE_MOUNTED & dwExecuteType) {
		_stprintf_s(szPromptTyxt, _T("%s已经挂载,是否使用挂载的数据?"), szDriveLetter);
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
			// TODO: 在此放置处理何时用“确定”来关闭对话框的代码
			CClientDlg dlg;
			m_pMainWnd = &dlg;
			INT_PTR nResponse = dlg.DoModal();
			if (nResponse == IDOK) {
				// TODO: 在此放置处理何时用“确定”来关闭对话框的代码
			} else if (nResponse == IDCANCEL) {
				// TODO: 在此放置处理何时用“取消”来关闭对话框的代码
			}
		} else if (nResponse == IDCANCEL) {
			// TODO: 在此放置处理何时用“取消”来关闭对话框的代码
		}
	} else {
		CClientDlg dlg;
		m_pMainWnd = &dlg;
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK) {
			// TODO: 在此放置处理何时用“确定”来关闭对话框的代码
		} else if (nResponse == IDCANCEL) {
			// TODO: 在此放置处理何时用“取消”来关闭对话框的代码
		}	
	}
	// NApplicaBzl::ClearEnviron(); // disable by james 20130506
	//
	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}