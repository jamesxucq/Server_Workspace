#if !defined(AFX_IMGBUTTON_H__DB19AD2C_DCD3_4019_9700_D1FD1F15E43C__INCLUDED_)
#define AFX_IMGBUTTON_H__DB19AD2C_DCD3_4019_9700_D1FD1F15E43C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImgButton.h : header file
//

#define      BTN_STYLE_FOCUSELINE    1
#define      IBS_TEXT                2
/////////////////////////////////////////////////////////////////////////////
// CImgButton window

class CImgButton : public CButton
{
// Construction
public:
	CRect m_smallRect;
	CImgButton();
    BOOL m_bNormal;
// Attributes
public:
	UINT     m_style;
	int      m_curX;
	int      m_curY;
    CBitmap  m_focuseBmp;
	CBitmap  m_selectBmp;
	CBitmap  m_normalBmp;
	CBitmap  m_disableBmp;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImgButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SetImgs(UINT normalImg, UINT focuseImg, UINT selectImg, UINT disableImg);
	BOOL SetImgs(HBITMAP normalImg, HBITMAP focuseImg, HBITMAP selectImg, HBITMAP disableImg);

	virtual ~CImgButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CImgButton)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	BOOL SetStyle(UINT style);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMGBUTTON_H__DB19AD2C_DCD3_4019_9700_D1FD1F15E43C__INCLUDED_)
