#pragma once

#include "DiresVec.h"
// CAdvaTView

class CAdvaTView : public CTreeCtrl {
    DECLARE_DYNAMIC(CAdvaTView)
public:
    CAdvaTView();
    virtual ~CAdvaTView();
protected:
    DECLARE_MESSAGE_MAP()
public:
    BOOL SetItemState(HTREEITEM hItem, UINT nState, UINT nStateMask);
protected:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnStateIconClick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
private:
    UINT m_uFlags;
    void TravelSiblingAndParent(HTREEITEM hItem, UINT state);
    void TravelChild(HTREEITEM hItem, UINT state);
public:
    afx_msg void OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult);
// mod by james 20141231
private:
	DWORD m_dwTViewType;
	CRITICAL_SECTION m_csTViewType;
	DiresHmap *m_mpTViewHmap;
	HTREEITEM m_hRootItem;
public:
	VOID RegistDisplayTView(DiresHmap *mpTViewHmap)
	{ m_mpTViewHmap = mpTViewHmap; }
	VOID RedrawTreeView(DWORD dwTViewType);
};

#define NORMAL_TVIEW	0x0000
#define ADVANCED_TVIEW	0x0001

