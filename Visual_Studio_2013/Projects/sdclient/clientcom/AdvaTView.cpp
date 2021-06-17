// AdvaTreeView.cpp : ʵ���ļ�
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



// CAdvaTView ��Ϣ�������
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
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    HTREEITEM hItem = HitTest(point, &m_uFlags);
    if ( m_uFlags&TVHT_ONITEMSTATEICON ) {
        // state: 0->��ѡ��ť 1->û��ѡ�� 2->����ѡ�� 3->ȫ��ѡ��
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
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    // ����ո��
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
    // �����ӽڵ㣬û�оͽ���
    hChildItem = GetChildItem(hItem);
    if(NULL != hChildItem) {
        // �����ӽڵ��״̬�뵱ǰ�ڵ��״̬һ��
        CTreeCtrl::SetItemState(hChildItem, INDEXTOSTATEIMAGEMASK(state),
                                TVIS_STATEIMAGEMASK );
        // �ٵݹ鴦���ӽڵ���ӽڵ���ֵܽڵ�
        TravelChild(hChildItem, state);
        // �����ӽڵ���ֵܽڵ�����ӽڵ�
        hBrotherItem = GetNextSiblingItem(hChildItem);
        while (hBrotherItem) {
            // �����ӽڵ���ֵܽڵ�״̬�뵱ǰ�ڵ��״̬һ��
            UINT nState1 = GetItemState( hBrotherItem, TVIS_STATEIMAGEMASK ) >> 12;
            if(0 != nState1) {
                CTreeCtrl::SetItemState( hBrotherItem,
                                         INDEXTOSTATEIMAGEMASK(state), TVIS_STATEIMAGEMASK );
            }
            // �ٵݹ鴦���ӽڵ���ֵܽڵ���ӽڵ���ֵܽڵ�
            TravelChild(hBrotherItem, state);
            hBrotherItem = GetNextSiblingItem(hBrotherItem);
        }
    }
}

void CAdvaTView::TravelSiblingAndParent(HTREEITEM hItem, UINT state) {
    HTREEITEM hNextSiblingItem, hPrevSiblingItem, hParentItem;
	DWORD dwSibli = 0x00;
//
    // ���Ҹ��ڵ㣬û�оͽ���
    hParentItem = GetParentItem(hItem);
    if(NULL != hParentItem) {
        UINT nState1 = state; // ���ʼֵ����ֹû���ֵܽڵ�ʱ����
        // ���ҵ�ǰ�ڵ�������ֵܽڵ��״̬
        hNextSiblingItem = GetNextSiblingItem(hItem);
		if(hNextSiblingItem) dwSibli = 0x01;
        while(NULL != hNextSiblingItem) {
            nState1 = GetItemState( hNextSiblingItem, TVIS_STATEIMAGEMASK ) >> 12;
            if(state!=nState1 && 0!=nState1) break;
            hNextSiblingItem = GetNextSiblingItem(hNextSiblingItem);
        }
//
        if(nState1==state || 0==nState1) {
            // ���ҵ�ǰ�ڵ�������ֵܽڵ��״̬
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
					// ���״̬һ�£��򸸽ڵ��״̬�뵱ǰ�ڵ��״̬һ��
					CTreeCtrl::SetItemState( hParentItem,
											 INDEXTOSTATEIMAGEMASK(2), TVIS_STATEIMAGEMASK );
				}
				// �ٵݹ鴦���ڵ���ֵܽڵ���丸�ڵ�
				TravelSiblingAndParent(hParentItem, 2);		
			} else {
				if(0 != nState1) {
					// ���״̬һ�£��򸸽ڵ��״̬�뵱ǰ�ڵ��״̬һ��
					CTreeCtrl::SetItemState( hParentItem,
											 INDEXTOSTATEIMAGEMASK(state), TVIS_STATEIMAGEMASK );
				}
				// �ٵݹ鴦���ڵ���ֵܽڵ���丸�ڵ�
				TravelSiblingAndParent(hParentItem, state);			
			}
        } else {
            // ״̬��һ�£���ǰ�ڵ�ĸ��ڵ㡢���ڵ�ĸ��ڵ㡭��״̬��Ϊ����̬
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
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    *pResult = 0;
    HTREEITEM hItem = pNMTreeView->itemNew.hItem;
//
    // ����ͼ�����Ӧ��ͼ��
    if(TVIS_EXPANDED == (GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED)) { // �Ƿ��Ѿ�չ��
        // ����չ��ͼ��
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
    // ��ʼ����
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

