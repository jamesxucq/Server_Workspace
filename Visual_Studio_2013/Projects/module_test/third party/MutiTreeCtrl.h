#pragma once


// CMutiTreeCtrl

class CMutiTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CMutiTreeCtrl)

public:
	CMutiTreeCtrl();
	virtual ~CMutiTreeCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	BOOL SetItemState( HTREEITEM hItem, UINT nState, UINT nStateMask, BOOL bSearch=TRUE);
protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnStateIconClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
private:
	UINT m_uFlags;
	void TravelSiblingAndParent(HTREEITEM hItem, int iState);
	void TravelChild(HTREEITEM hItem,int iState);
public:
	afx_msg void OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult);
};


