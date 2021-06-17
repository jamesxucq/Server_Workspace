#include "StdAfx.h"
#include "LoginWizdObj.h"

CLoginWizdObj::CLoginWizdObj(void):
m_pCVolWizardDlg(NULL),
m_dwCVolStatus(0x0000),
m_hCurrentPageHandle(NULL),
m_dwCurrentNumbe(UNUSE_PAGE),
m_dwFormatStatus(FORMAT_STATUS_WAITING)
{
}

CLoginWizdObj::~CLoginWizdObj(void)
{
}

CLoginWizdObj objLoginWizdObj;