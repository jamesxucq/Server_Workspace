#include "StdAfx.h"
#include "BtnST.h"

CButtonST::CButtonST(void):
m_dwDisplayFlags(NORMAL)
{

}

CButtonST::~CButtonST(void)
{
}

BOOL CButtonST::SetImgs(UINT normalImg, UINT focuseImg, UINT selectImg)
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

	return TRUE;
}

BOOL CButtonST::SetImgs(HBITMAP normalImg, HBITMAP focuseImg, HBITMAP selectImg)
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

	return TRUE;
}


void  CButtonST::DrawImage(DWORD dwStatus)
{
	m_dwDisplayFlags = dwStatus;
}
void CButtonST::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO:  添加您的代码以绘制指定项
	CDC	 * pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rect=lpDrawItemStruct->rcItem;

	pDC->SetBkMode(TRANSPARENT);

	if (NORMAL & m_dwDisplayFlags)
	{
		if(FOCUSE & m_dwDisplayFlags){
			if(m_focuseBmp.m_hObject)
			{
				BITMAP bm; 
				CDC foucseDc;
				foucseDc.CreateCompatibleDC(pDC);
				foucseDc.SelectObject(m_focuseBmp);
				m_focuseBmp.GetObject(sizeof(BITMAP), &bm); 
				pDC->StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),&foucseDc,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
			}		
		} else {
			if(m_normalBmp.m_hObject)
			{
				m_normalBmp.m_hObject;
				BITMAP bm; 
				CDC normalDc;
				normalDc.CreateCompatibleDC(pDC);
				normalDc.SelectObject(m_normalBmp);
				m_normalBmp.GetObject(sizeof(BITMAP), &bm); 
				pDC->StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),&normalDc,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
			}		
		}
	}
	else
	{
		if(m_selectBmp.m_hObject)
		{
			BITMAP bm; 
			CDC selectDc;
			selectDc.CreateCompatibleDC(pDC);
			selectDc.SelectObject(m_selectBmp);
			m_selectBmp.GetObject(sizeof(BITMAP), &bm); 
			pDC->StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),&selectDc,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
		}
	}
}
BEGIN_MESSAGE_MAP(CButtonST, CButton)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

void CButtonST::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if((m_dwDisplayFlags & NORMAL) && !(m_dwDisplayFlags & FOCUSE)) { // 鼠标第一次移入窗口时，请求一个WM_MOUSELEAVE消息 
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		//监控鼠标离开
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = this->m_hWnd;

		if (::_TrackMouseEvent(&tme))
		{
			//鼠标进入时的其他操作
			m_dwDisplayFlags |= FOCUSE;
			Invalidate();
		}
	}

	CButton::OnMouseMove(nFlags, point);
}

void CButtonST::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_dwDisplayFlags & NORMAL) {
		m_dwDisplayFlags &= ~FOCUSE;
		Invalidate();
	}

	CButton::OnMouseLeave();
}
