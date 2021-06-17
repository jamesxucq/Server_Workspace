// ImgButton.cpp : implementation file
//

#include "stdafx.h"
#include "ImgButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImgButton

CImgButton::CImgButton()
{
	m_smallRect=CRect(0,0,0,0);
	m_curX=0;
	m_curY=0;
	m_bNormal=TRUE;
	m_style=0;
}

CImgButton::~CImgButton()
{
}


BEGIN_MESSAGE_MAP(CImgButton, CButton)
	//{{AFX_MSG_MAP(CImgButton)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImgButton message handlers

void CImgButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	BOOL selected=lpDrawItemStruct->itemState & ODS_SELECTED;
	BOOL disabled=lpDrawItemStruct->itemState & ODS_DISABLED;
	BOOL focused=lpDrawItemStruct->itemState & ODS_FOCUS;
   
	CDC	 * pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
    CRect rect=lpDrawItemStruct->rcItem;
	CString strTitle;

	pDC->SetBkMode(TRANSPARENT);
	//pDC->SetBkColor(RGB(0,0,0));

	if(selected && m_normalBmp.m_hObject)
	{
		if(m_bNormal)
		{
			m_bNormal=FALSE;
		}
		else
		{
			m_bNormal=TRUE;
		}
	}

	if(selected)
	{
		if(m_selectBmp.m_hObject)
		{
			BITMAP bm; 
			CDC selecteDc;
			selecteDc.CreateCompatibleDC(pDC);
			selecteDc.SelectObject(m_selectBmp);
			m_selectBmp.GetObject(sizeof(BITMAP), &bm); 
			pDC->StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),&selecteDc,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
			if(m_style & IBS_TEXT)
			{
				GetWindowTextW(strTitle);
				pDC->DrawTextW(strTitle, rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			}
		}
		else
		{

		}
	}
	else if(focused )
	{
		if(m_bNormal)
		{
			if(m_focuseBmp.m_hObject)
			{
				BITMAP bm; 
				CDC foucseDc;
				foucseDc.CreateCompatibleDC(pDC);
				foucseDc.SelectObject(m_focuseBmp);
				m_focuseBmp.GetObject(sizeof(BITMAP), &bm); 
				pDC->StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),&foucseDc,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
				if(m_style & IBS_TEXT)
				{
					GetWindowTextW(strTitle);
					pDC->DrawTextW(strTitle, rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				}
			}
			else
			{
				pDC->FillRect(rect, &CBrush(::GetSysColor(COLOR_BTNFACE)));
				GetWindowTextW(strTitle);
				pDC->DrawTextW(strTitle, rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				CRect rectTmp;
				rectTmp=rect;
				rectTmp.left += 2;
				rectTmp.right -=2;
				rectTmp.top +=3;
				rectTmp.bottom -=2;
				pDC->Draw3dRect(&rectTmp,RGB(30,170,250),RGB(30,170,250));

				pDC->MoveTo(0, 0);
				pDC->LineTo(rect.right-1, 0);
				pDC->LineTo(rect.right-1, rect.bottom);

			}
			
		}
		if(!m_bNormal)
		{
			if(m_disableBmp.m_hObject)
			{
				m_disableBmp.m_hObject;
				BITMAP bm; 
				CDC disableDc;
				disableDc.CreateCompatibleDC(pDC);
				disableDc.SelectObject(m_disableBmp);
				m_disableBmp.GetObject(sizeof(BITMAP), &bm); 
				pDC->StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),&disableDc,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
				if(m_style & IBS_TEXT)
				{
					GetWindowTextW(strTitle);
					pDC->DrawTextW(strTitle, rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				}
			}
			else
			{
		    	pDC->FillRect(rect, &CBrush(RGB(255,255,255)));
				GetWindowTextW(strTitle);
				pDC->DrawTextW(strTitle, rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER );
				CRect rectTmp;
				rectTmp=rect;
				rectTmp.left += 2;
				rectTmp.right -=2;
				rectTmp.top +=3;
				rectTmp.bottom -=2;
				pDC->Draw3dRect(&rectTmp,RGB(30,170,250),RGB(30,170,250));

				pDC->MoveTo(rect.right-1, 0);
				pDC->LineTo(rect.right-1, rect.bottom);
			}

		}
		if( m_style & BTN_STYLE_FOCUSELINE )
		{
			//pDC->Draw3dRect(&rect,RGB(30,170,250),RGB(30,170,250));
			pDC->Draw3dRect(&rect,RGB(0,200,0),RGB(0,200,0));
		}
	}
	else if(disabled && m_disableBmp.m_hObject)
	{
		m_disableBmp.m_hObject;
		BITMAP bm; 
		CDC disableDc;
		disableDc.CreateCompatibleDC(pDC);
		disableDc.SelectObject(m_disableBmp);
		m_disableBmp.GetObject(sizeof(BITMAP), &bm); 
		pDC->StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),&disableDc,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
		if(m_style & IBS_TEXT)
		{
			GetWindowTextW(strTitle);
			pDC->DrawTextW(strTitle, rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}
	}
	else
	{
		if(m_bNormal)
	    {
			if(m_normalBmp.m_hObject)
			{
				m_normalBmp.m_hObject;
				BITMAP bm; 
				CDC normalDc;
				normalDc.CreateCompatibleDC(pDC);
				normalDc.SelectObject(m_normalBmp);
				m_normalBmp.GetObject(sizeof(BITMAP), &bm); 
				pDC->StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),&normalDc,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
				if(m_style & IBS_TEXT)
				{
					GetWindowTextW(strTitle);
					pDC->DrawTextW(strTitle, rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				}
			}
			else
			{
				pDC->FillRect(rect, &CBrush(::GetSysColor(COLOR_BTNFACE)));
				GetWindowTextW(strTitle);
				pDC->DrawTextW(strTitle, rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				pDC->MoveTo(0, 0);
				pDC->LineTo(rect.right-1, 0);
				pDC->LineTo(rect.right-1, rect.bottom);
				//pDC->MoveTo(0, 0);
				//pDC->LineTo(0, rect.bottom);

			}
	    }
		if(!m_bNormal)
		{
			if(m_selectBmp.m_hObject)
			{
				BITMAP bm; 
				CDC selecteDc;
				selecteDc.CreateCompatibleDC(pDC);
				selecteDc.SelectObject(m_selectBmp);
				m_selectBmp.GetObject(sizeof(BITMAP), &bm); 
				pDC->StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),&selecteDc,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
				if(m_style & IBS_TEXT)
				{
					GetWindowTextW(strTitle);
					pDC->DrawTextW(strTitle, rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				}
			}
			else
			{
				pDC->FillRect(rect, &CBrush(RGB(255,255,255)));
				GetWindowTextW(strTitle);
				pDC->DrawTextW(strTitle, rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER );
				pDC->MoveTo(rect.right-1, 0);
				pDC->LineTo(rect.right-1, rect.bottom);
			}
		}
	}
}

BOOL CImgButton::SetImgs(UINT normalImg, UINT focuseImg, UINT selectImg, UINT disableImg)
{

	if(normalImg>0)
	{
		m_normalBmp.LoadBitmap(normalImg);
	}
    if(focuseImg>0)
	{
	    m_focuseBmp.LoadBitmap(focuseImg);
	}
	if(selectImg>0)
	{
	    m_selectBmp.LoadBitmap(selectImg);
	}
	if(disableImg>0)
	{
	    m_disableBmp.LoadBitmap(disableImg);
	}
    return TRUE;
}

BOOL CImgButton::SetImgs(HBITMAP normalImg, HBITMAP focuseImg, HBITMAP selectImg, HBITMAP disableImg)
{
	if(NULL!=normalImg)
	{
		m_normalBmp.Attach(normalImg);
	}
    if(NULL!=focuseImg>0)
	{
	    m_focuseBmp.Attach(focuseImg);
	}
	if(NULL!=selectImg>0)
	{
	    m_selectBmp.Attach(selectImg);
	}
	if(NULL!=disableImg>0)
	{
	    m_disableBmp.Attach(disableImg);
	}
    return TRUE;
}


void CImgButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CButton::OnLButtonDblClk(nFlags, point);
	//this->PostMessage(BN_CLICKED
	SendMessage(WM_LBUTTONDOWN,NULL,NULL);   
    SendMessage(WM_LBUTTONUP,NULL,NULL);   
}

void CImgButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_curX=point.x;
	m_curY=point.y;
	//if(m_smallRect.PtInRect(point))
	{
	}
	//else
	{
	    CButton::OnLButtonDown(nFlags, point);
	}
}

BOOL CImgButton::SetStyle(UINT style)
{
	m_style = style;
	return TRUE;
}
