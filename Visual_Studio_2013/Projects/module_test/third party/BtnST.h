#pragma once

#define NORMAL 0x00000001
#define FOCUSE 0x00000002
#define SELECT 0x00000004

//Normal¡¢Selected¡¢Focused¡¢Disabled
class CButtonST : public CButton {
	// Construction
public:
	CButtonST(void);
	~CButtonST(void);
	// Attributes
public:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
public:
	DWORD m_dwDisplayFlags;
	CBitmap  m_normalBmp;
	CBitmap  m_focuseBmp;
	CBitmap  m_selectBmp;
	// Implementation
public:
	BOOL SetImgs(UINT normalImg, UINT focuseImg, UINT selectImg);
	BOOL SetImgs(HBITMAP normalImg, HBITMAP focuseImg, HBITMAP selectImg);
public:
	void DrawImage(DWORD dwStatus);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};

