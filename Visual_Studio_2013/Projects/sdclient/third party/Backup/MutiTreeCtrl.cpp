// MutiTreeCtrl.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MutiTreeCtrl.h"


// CMutiTreeCtrl

IMPLEMENT_DYNAMIC(CMutiTreeCtrl, CTreeCtrl)

CMutiTreeCtrl::CMutiTreeCtrl():
    m_uFlags(0)
{

}

CMutiTreeCtrl::~CMutiTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CMutiTreeCtrl, CTreeCtrl)
    ON_WM_LBUTTONDOWN()
    ON_NOTIFY_REFLECT(NM_CLICK, OnStateIconClick)
    ON_NOTIFY_REFLECT(TVN_KEYDOWN, OnKeydown)
    ON_WM_KEYDOWN()
    ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, &CMutiTreeCtrl::OnTvnItemexpanded)
END_MESSAGE_MAP()



// CMutiTreeCtrl ��Ϣ�������
BOOL CMutiTreeCtrl::SetItemState(HTREEITEM hItem, UINT nState, UINT nStateMask, BOOL bSearch)
{
    Select(hItem, TVGN_CARET);
    BOOL bSetValue=CTreeCtrl::SetItemState( hItem, nState, nStateMask );

    UINT state = nState >> 12;
    if(state!=0) {
        if(bSearch) TravelChild(hItem, state);
        TravelSiblingAndParent(hItem,state);
    }
    return bSetValue;
}


void CMutiTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    HTREEITEM hItem =HitTest(point, &m_uFlags);
    if (( m_uFlags&TVHT_ONITEMSTATEICON )) {
        // state: 0->��ѡ��ť 1->û��ѡ�� 2->����ѡ�� 3->ȫ��ѡ��
        UINT state = GetItemState( hItem, TVIS_STATEIMAGEMASK ) >> 12;
        state=(state==3)?1:3;
        SetItemState(hItem,INDEXTOSTATEIMAGEMASK(state),TVIS_STATEIMAGEMASK);
    } else CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CMutiTreeCtrl::OnStateIconClick(NMHDR* pNMHDR, LRESULT* pResult)
{
    if(m_uFlags&TVHT_ONITEMSTATEICON) *pResult=1;
    else *pResult = 0;
}

void CMutiTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    // ����ո��
    if(0x20==nChar) {
        HTREEITEM hItem =GetSelectedItem();
        UINT state = GetItemState( hItem, TVIS_STATEIMAGEMASK ) >> 12;
        if(state!=0) {
            state=(state==3)?1:3;
            SetItemState( hItem, INDEXTOSTATEIMAGEMASK(state), TVIS_STATEIMAGEMASK );
        }
    } else CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

    CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMutiTreeCtrl::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult)
{
    TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
    *pResult = 0;
}

void CMutiTreeCtrl::TravelChild(HTREEITEM hItem, int state)
{
    HTREEITEM hChildItem,hBrotherItem;

    // �����ӽڵ㣬û�оͽ���
    hChildItem=GetChildItem(hItem);
    if(NULL!=hChildItem) {
        // �����ӽڵ��״̬�뵱ǰ�ڵ��״̬һ��
        CTreeCtrl::SetItemState(hChildItem,INDEXTOSTATEIMAGEMASK(state),
                                TVIS_STATEIMAGEMASK );
        // �ٵݹ鴦���ӽڵ���ӽڵ���ֵܽڵ�
        TravelChild(hChildItem, state);

        // �����ӽڵ���ֵܽڵ�����ӽڵ�
        hBrotherItem=GetNextSiblingItem(hChildItem);
        while (hBrotherItem) {
            // �����ӽڵ���ֵܽڵ�״̬�뵱ǰ�ڵ��״̬һ��
            int iState1 = GetItemState( hBrotherItem, TVIS_STATEIMAGEMASK ) >> 12;
            if(iState1!=0) {
                CTreeCtrl::SetItemState( hBrotherItem,
                                         INDEXTOSTATEIMAGEMASK(state),TVIS_STATEIMAGEMASK );
            }
            // �ٵݹ鴦���ӽڵ���ֵܽڵ���ӽڵ���ֵܽڵ�
            TravelChild(hBrotherItem, state);
            hBrotherItem=GetNextSiblingItem(hBrotherItem);
        }
    }
}

void CMutiTreeCtrl::TravelSiblingAndParent(HTREEITEM hItem, int state)
{
    HTREEITEM hNextSiblingItem,hPrevSiblingItem,hParentItem;

    // ���Ҹ��ڵ㣬û�оͽ���
    hParentItem=GetParentItem(hItem);
    if(NULL!=hParentItem) {
        int iState1=state;//���ʼֵ����ֹû���ֵܽڵ�ʱ����

        // ���ҵ�ǰ�ڵ�������ֵܽڵ��״̬
        hNextSiblingItem=GetNextSiblingItem(hItem);
        while(NULL!=hNextSiblingItem) {
            iState1 = GetItemState( hNextSiblingItem, TVIS_STATEIMAGEMASK ) >> 12;
            if(iState1!=state && iState1!=0) break;
            else hNextSiblingItem=GetNextSiblingItem(hNextSiblingItem);
        }

        // if(iState1==state)
        if(iState1==state || 0==iState1) {
            // ���ҵ�ǰ�ڵ�������ֵܽڵ��״̬
            hPrevSiblingItem=GetPrevSiblingItem(hItem);
            while(NULL!=hPrevSiblingItem) {
                iState1 = GetItemState(hPrevSiblingItem,TVIS_STATEIMAGEMASK)>> 12;
                if(iState1!=state && iState1!=0) break;
                else hPrevSiblingItem=GetPrevSiblingItem(hPrevSiblingItem);
            }
        }

        if(iState1==state || 0==iState1) {
            iState1 = GetItemState( hParentItem, TVIS_STATEIMAGEMASK ) >> 12;
            if(iState1!=0) {
                // ���״̬һ�£��򸸽ڵ��״̬�뵱ǰ�ڵ��״̬һ��
                CTreeCtrl::SetItemState( hParentItem,
                                         INDEXTOSTATEIMAGEMASK(state), TVIS_STATEIMAGEMASK );
            }
            // �ٵݹ鴦���ڵ���ֵܽڵ���丸�ڵ�
            TravelSiblingAndParent(hParentItem,state);
        } else {
            // ״̬��һ�£���ǰ�ڵ�ĸ��ڵ㡢���ڵ�ĸ��ڵ㡭��״̬��Ϊ����̬
            hParentItem=GetParentItem(hItem);
            while(NULL!=hParentItem) {
                iState1 = GetItemState( hParentItem, TVIS_STATEIMAGEMASK ) >> 12;
                if(iState1!=0) {
                    CTreeCtrl::SetItemState( hParentItem,
                                             INDEXTOSTATEIMAGEMASK(2), TVIS_STATEIMAGEMASK );
                }
                hParentItem=GetParentItem(hParentItem);
            }
        }
    }
}

void CMutiTreeCtrl::OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    *pResult = 0;
    HTREEITEM hItem = pNMTreeView->itemNew.hItem;

    // ����ͼ�����Ӧ��ͼ��
    if(TVIS_EXPANDED == (GetItemState(hItem,TVIS_EXPANDED) & TVIS_EXPANDED)) { // �Ƿ��Ѿ�չ��
        // ����չ��ͼ��
        SetItemImage(hItem,0,0);
    } else SetItemImage(hItem,1,1);
}
