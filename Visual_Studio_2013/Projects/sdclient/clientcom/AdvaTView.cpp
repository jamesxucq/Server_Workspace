// AdvaTreeView.cpp : 实现文件
//

#include "stdafx.h"
#include "Logger.h"
#include "TVUtility.h"

#include "AdvaTView.h"


// CAdvaTView

IMPLEMENT_DYNAMIC(CAdvaTView, CTreeCtrl)

CAdvaTView::CAdvaTView():
    m_uFlags(0),
	m_dwTViewType(0x00),
	m_mpTViewHmap(NULL),
	m_hRootItem(NULL)
{
    InitializeCriticalSection(&m_csTViewType);
}

CAdvaTView::~CAdvaTView() {
    DeleteCriticalSection(&m_csTViewType);
}


BEGIN_MESSAGE_MAP(CAdvaTView, CTreeCtrl)
    ON_WM_LBUTTONDOWN()
    ON_NOTIFY_REFLECT(NM_CLICK, OnStateIconClick)
    ON_NOTIFY_REFLECT(TVN_KEYDOWN, OnKeydown)
    ON_WM_KEYDOWN()
    ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, &CAdvaTView::OnTvnItemexpanded)
END_MESSAGE_MAP()



// CAdvaTView 消息处理程序
BOOL CAdvaTView::SetItemState(HTREEITEM hItem, UINT nState, UINT nStateMask) {
    Select(hItem, TVGN_CARET);
    BOOL bSetValue = CTreeCtrl::SetItemState( hItem, nState, nStateMask );
//
    UINT state = nState >> 12;
    if(0 != state) {
        TravelChild(hItem, state);
        TravelSiblingAndParent(hItem, state);
    }
    return bSetValue;
}


void CAdvaTView::OnLButtonDown(UINT nFlags, CPoint point) {
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    HTREEITEM hItem = HitTest(point, &m_uFlags);
    if ( m_uFlags&TVHT_ONITEMSTATEICON ) {
        // state: 0->无选择钮 1->没有选择 2->部分选择 3->全部选择
        UINT state = GetItemState( hItem, TVIS_STATEIMAGEMASK ) >> 12;
        state = (3==state)? 1:3;
        SetItemState(hItem, INDEXTOSTATEIMAGEMASK(state), TVIS_STATEIMAGEMASK);
    } else CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CAdvaTView::OnStateIconClick(NMHDR* pNMHDR, LRESULT* pResult) {
    if(m_uFlags&TVHT_ONITEMSTATEICON) *pResult = 1;
    else *pResult = 0;
}

void CAdvaTView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    // 处理空格键
    if(0x20 == nChar) {
        HTREEITEM hItem = GetSelectedItem();
        UINT state = GetItemState( hItem, TVIS_STATEIMAGEMASK ) >> 12;
        if(0 != state) {
            state = (3==state)? 1:3;
            SetItemState( hItem, INDEXTOSTATEIMAGEMASK(state), TVIS_STATEIMAGEMASK );
        }
    } else CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
//
    CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CAdvaTView::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) {
    TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
    *pResult = 0;
}

void CAdvaTView::TravelChild(HTREEITEM hItem, UINT state) {
    HTREEITEM hChildItem, hBrotherItem;
    // 查找子节点，没有就结束
    hChildItem = GetChildItem(hItem);
    if(NULL != hChildItem) {
        // 设置子节点的状态与当前节点的状态一致
        CTreeCtrl::SetItemState(hChildItem, INDEXTOSTATEIMAGEMASK(state),
                                TVIS_STATEIMAGEMASK );
        // 再递归处理子节点的子节点和兄弟节点
        TravelChild(hChildItem, state);
        // 处理子节点的兄弟节点和其子节点
        hBrotherItem = GetNextSiblingItem(hChildItem);
        while (hBrotherItem) {
            // 设置子节点的兄弟节点状态与当前节点的状态一致
            UINT nState1 = GetItemState( hBrotherItem, TVIS_STATEIMAGEMASK ) >> 12;
            if(0 != nState1) {
                CTreeCtrl::SetItemState( hBrotherItem,
                                         INDEXTOSTATEIMAGEMASK(state), TVIS_STATEIMAGEMASK );
            }
            // 再递归处理子节点的兄弟节点的子节点和兄弟节点
            TravelChild(hBrotherItem, state);
            hBrotherItem = GetNextSiblingItem(hBrotherItem);
        }
    }
}

void CAdvaTView::TravelSiblingAndParent(HTREEITEM hItem, UINT state) {
    HTREEITEM hNextSiblingItem, hPrevSiblingItem, hParentItem;
	DWORD dwSibli = 0x00;
//
    // 查找父节点，没有就结束
    hParentItem = GetParentItem(hItem);
    if(NULL != hParentItem) {
        UINT nState1 = state; // 设初始值，防止没有兄弟节点时出错
        // 查找当前节点下面的兄弟节点的状态
        hNextSiblingItem = GetNextSiblingItem(hItem);
		if(hNextSiblingItem) dwSibli = 0x01;
        while(NULL != hNextSiblingItem) {
            nState1 = GetItemState( hNextSiblingItem, TVIS_STATEIMAGEMASK ) >> 12;
            if(state!=nState1 && 0!=nState1) break;
            hNextSiblingItem = GetNextSiblingItem(hNextSiblingItem);
        }
//
        if(nState1==state || 0==nState1) {
            // 查找当前节点上面的兄弟节点的状态
            hPrevSiblingItem = GetPrevSiblingItem(hItem);
			if(hPrevSiblingItem) dwSibli = 0x01;
            while(NULL != hPrevSiblingItem) {
                nState1 = GetItemState(hPrevSiblingItem, TVIS_STATEIMAGEMASK) >> 12;
                if(state!=nState1 && 0!=nState1) break;
                hPrevSiblingItem = GetPrevSiblingItem(hPrevSiblingItem);
            }
        }
		//
		if(nState1==state || 0==nState1) {
			nState1 = GetItemState( hParentItem, TVIS_STATEIMAGEMASK ) >> 12;
			if(!dwSibli && 1==state) {
				if(0 != nState1) {
					// 如果状态一致，则父节点的状态与当前节点的状态一致
					CTreeCtrl::SetItemState( hParentItem,
											 INDEXTOSTATEIMAGEMASK(2), TVIS_STATEIMAGEMASK );
				}
				// 再递归处理父节点的兄弟节点和其父节点
				TravelSiblingAndParent(hParentItem, 2);		
			} else {
				if(0 != nState1) {
					// 如果状态一致，则父节点的状态与当前节点的状态一致
					CTreeCtrl::SetItemState( hParentItem,
											 INDEXTOSTATEIMAGEMASK(state), TVIS_STATEIMAGEMASK );
				}
				// 再递归处理父节点的兄弟节点和其父节点
				TravelSiblingAndParent(hParentItem, state);			
			}
        } else {
            // 状态不一致，则当前节点的父节点、父节点的父节点……状态均为第三态
            hParentItem = GetParentItem(hItem);
            while(NULL != hParentItem) {
                nState1 = GetItemState( hParentItem, TVIS_STATEIMAGEMASK ) >> 12;
                if(0 != nState1) {
                    CTreeCtrl::SetItemState( hParentItem,
                                             INDEXTOSTATEIMAGEMASK(2), TVIS_STATEIMAGEMASK );
                }
                hParentItem = GetParentItem(hParentItem);
            }
        }
    }
	//
}

// add by james 20141231 grand child
#define CHILDREN_EXPANDED { \
	EnterCriticalSection(&m_csTViewType); \
	DWORD dwTViewType = m_dwTViewType; \
	LeaveCriticalSection(&m_csTViewType); \
	/* \\ */ \
	if(ADVANCED_TVIEW == dwTViewType) { \
		HTREEITEM hNewItem; \
		struct DiINode *pDiINode; \
		DiresHmap::iterator hiter; \
		TCHAR path[MAX_PATH]; \
		/* \\ */ \
		HTREEITEM hSlibItem = GetChildItem( hItem ); \
		while( hSlibItem && !ItemHasChildren(hSlibItem) ) { \
			NTVUtility::GetDirectoryPath(path, this, hSlibItem, m_hRootItem); \
			/* \\ */ \
			hiter = m_mpTViewHmap->find(path); \
			if(m_mpTViewHmap->end() != hiter) { \
				pDiINode = ((struct DiINode *)hiter->second)->child; \
				while(pDiINode) { \
					hNewItem = NTVUtility::AddChildrenNode(this, hSlibItem, pDiINode->szINodeName); \
					NTVUtility::SetNodeChecked(this, hNewItem, pDiINode->selected); \
					pDiINode = pDiINode->sibling; \
				} \
			} \
/* \\ */ \
			hSlibItem = this->GetNextSiblingItem(hSlibItem); \
		} \
	} \
}

void CAdvaTView::OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult) {
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    *pResult = 0;
    HTREEITEM hItem = pNMTreeView->itemNew.hItem;
//
    // 设置图标成相应的图标
    if(TVIS_EXPANDED == (GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED)) { // 是否已经展开
        // 设置展开图标
        SetItemImage(hItem, 1, 1);
		// add by james 20150104
		CHILDREN_EXPANDED
    } else SetItemImage(hItem, 0, 0);
//
}

/*
static VOID AddGrandChildItem(CAdvaTView *tvDiresTView, HTREEITEM hRootItem, DiresHmap *mpTViewHmap) {
	HTREEITEM hNewItem;
	struct DiINode *pDiINode;
	DiresHmap::iterator hiter;
//
	TCHAR path[MAX_PATH];
    HTREEITEM hSlibItem = tvDiresTView->GetChildItem( hRootItem );
    while( hSlibItem ) {
		NTVUtility::GetDirectoryPath(path, tvDiresTView, hSlibItem, hRootItem);
		//
		hiter = mpTViewHmap->find(path);
		if(mpTViewHmap->end() != hiter) {
			pDiINode = ((struct DiINode *)hiter->second)->child;
			while(pDiINode) {
				hNewItem = NTVUtility::AddChildrenNode(tvDiresTView, hSlibItem, pDiINode->szINodeName);
				NTVUtility::SetNodeChecked(tvDiresTView, hNewItem, pDiINode->selected);
				pDiINode = pDiINode->sibling;
			}
		}
//
        hSlibItem = tvDiresTView->GetNextSiblingItem(hSlibItem);
    }
}
*/

VOID CAdvaTView::RedrawTreeView(DWORD dwTViewType) {
	EnterCriticalSection(&m_csTViewType);
	m_dwTViewType = dwTViewType;
	LeaveCriticalSection(&m_csTViewType);
    // 初始化树
	DeleteAllItems();
// SetRedraw(FALSE);
	//
	HTREEITEM hNewItem;
	struct DiINode *pDiINode;
	DiresHmap::iterator hiter;
	//
	hiter = m_mpTViewHmap->find(_T("\\"));
	if(m_mpTViewHmap->end() != hiter) {
		// addition root item
		pDiINode = (struct DiINode *)hiter->second;
		m_hRootItem = NTVUtility::AddChildrenNode(this, NULL, pDiINode->szINodeName);
		NTVUtility::SetNodeChecked(this, m_hRootItem, pDiINode->selected);
// _LOG_DEBUG(_T("root pDiINode->szINodeName:%s"), pDiINode->szINodeName);
		// addition children item
		pDiINode = ((struct DiINode *)hiter->second)->child;
		while(pDiINode) {
			hNewItem = NTVUtility::AddChildrenNode(this, m_hRootItem, pDiINode->szINodeName);
			NTVUtility::SetNodeChecked(this, hNewItem, pDiINode->selected);
// _LOG_DEBUG(_T("pDiINode->szINodeName:%s"), pDiINode->szINodeName);
			pDiINode = pDiINode->sibling;
		}
	}
    //
// SetRedraw(TRUE);
// RedrawWindow();
	Expand(m_hRootItem, TVE_EXPAND);
	// addition grandchildren item
//	if(ADVANCED_TVIEW == dwTViewType) {
//		AddGrandChildItem(this, m_hRootItem, m_mpTViewHmap);
//	}
}

