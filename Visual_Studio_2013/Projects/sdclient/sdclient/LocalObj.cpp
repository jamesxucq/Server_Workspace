#include "StdAfx.h"
#include "LocalObj.h"

CLocalObj::CLocalObj(void):
m_hNotifyWnd(NULL),
m_bShutdown(FALSE)
{
}

CLocalObj::~CLocalObj(void)
{
}

class CLocalObj	objLocalObj;