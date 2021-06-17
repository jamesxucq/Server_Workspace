// MutiTreeCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "MutiTreeCtrl.h"


// CMutiTreeCtrl

IMPLEMENT_DYNAMIC(CMutiTreeCtrl, CTreeCtrl)

CMutiTreeCtrl::CMutiTreeCtrl()
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



// CMutiTreeCtrl 消息处理程序
BOOL CMutiTreeCtrl::SetItemState(HTREEITEM hItem, UINT nState, UINT nStateMask, BOOL bSearch)
{
	Select(hItem, TVGN_CARET);
	BOOL bSetValue=CTreeCtrl::SetItemState( hItem, nState, nStateMask );

	UINT iState = nState >> 12;
	if(iState!=0)
	{
		if(bSearch) TravelChild(hItem, iState);
		TravelSiblingAndParent(hItem,iState);
	}
	return bSetValue;
}


void CMutiTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	HTREEITEM hItem =HitTest(point, &m_uFlags);
	if (( m_uFlags&TVHT_ONITEMSTATEICON ))
	{
		//iState: 0->无选择钮 1->没有选择 2->部分选择 3->全部选择
		UINT iState = GetItemState( hItem, TVIS_STATEIMAGEMASK ) >> 12;
		iState=(iState==3)?1:3;
		SetItemState(hItem,INDEXTOSTATEIMAGEMASK(iState),TVIS_STATEIMAGEMASK);
	}
	else CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CMutiTreeCtrl::OnStateIconClick(NMHDR* pNMHDR, LRESULT* pResult)  
{ 
	if(m_uFlags&TVHT_ONITEMSTATEICON) *pResult=1; 
	else *pResult = 0; 
} 

void CMutiTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//处理空格键
	if(nChar==0x20)
	{
		HTREEITEM hItem =GetSelectedItem();
		UINT iState = GetItemState( hItem, TVIS_STATEIMAGEMASK ) >> 12;
		if(iState!=0)
		{
			iState=(iState==3)?1:3;
			SetItemState( hItem, INDEXTOSTATEIMAGEMASK(iState), TVIS_STATEIMAGEMASK );
		}
	}
	else CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMutiTreeCtrl::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult)  
{ 
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR; 
	*pResult = 0; 
} 

void CMutiTreeCtrl::TravelChild(HTREEITEM hItem, int iState)
{
	HTREEITEM hChildItem,hBrotherItem;

	//查找子节点，没有就结束
	hChildItem=GetChildItem(hItem);
	if(hChildItem!=NULL)
	{
		//设置子节点的状态与当前节点的状态一致
		CTreeCtrl::SetItemState(hChildItem,INDEXTOSTATEIMAGEMASK(iState),
			TVIS_STATEIMAGEMASK );
		//再递归处理子节点的子节点和兄弟节点
		TravelChild(hChildItem, iState);

		//处理子节点的兄弟节点和其子节点
		hBrotherItem=GetNextSiblingItem(hChildItem);
		while (hBrotherItem)
		{
			//设置子节点的兄弟节点状态与当前节点的状态一致
			int iState1 = GetItemState( hBrotherItem, TVIS_STATEIMAGEMASK ) >> 12;
			if(iState1!=0)
			{
				CTreeCtrl::SetItemState( hBrotherItem,
					INDEXTOSTATEIMAGEMASK(iState),TVIS_STATEIMAGEMASK );
			}
			//再递归处理子节点的兄弟节点的子节点和兄弟节点
			TravelChild(hBrotherItem, iState);
			hBrotherItem=GetNextSiblingItem(hBrotherItem);
		}
	}
}

void CMutiTreeCtrl::TravelSiblingAndParent(HTREEITEM hItem, int iState)
{
	HTREEITEM hNextSiblingItem,hPrevSiblingItem,hParentItem;

	//查找父节点，没有就结束
	hParentItem=GetParentItem(hItem);
	if(hParentItem!=NULL)
	{
		int iState1=iState;//设初始值，防止没有兄弟节点时出错

		//查找当前节点下面的兄弟节点的状态
		hNextSiblingItem=GetNextSiblingItem(hItem);
		while(hNextSiblingItem!=NULL)
		{
			iState1 = GetItemState( hNextSiblingItem, TVIS_STATEIMAGEMASK ) >> 12;
			if(iState1!=iState && iState1!=0) break;
			else hNextSiblingItem=GetNextSiblingItem(hNextSiblingItem);
		}

		//if(iState1==iState)
		if(iState1==iState || iState1==0)
		{
			//查找当前节点上面的兄弟节点的状态
			hPrevSiblingItem=GetPrevSiblingItem(hItem);
			while(hPrevSiblingItem!=NULL)
			{
				iState1 = GetItemState(hPrevSiblingItem,TVIS_STATEIMAGEMASK)>> 12;
				if(iState1!=iState && iState1!=0) break;
				else hPrevSiblingItem=GetPrevSiblingItem(hPrevSiblingItem);
			}
		}

		if(iState1==iState || iState1==0)
		{
			iState1 = GetItemState( hParentItem, TVIS_STATEIMAGEMASK ) >> 12;
			if(iState1!=0)
			{
				//如果状态一致，则父节点的状态与当前节点的状态一致
				CTreeCtrl::SetItemState( hParentItem, 
					INDEXTOSTATEIMAGEMASK(iState), TVIS_STATEIMAGEMASK );
			}
			//再递归处理父节点的兄弟节点和其父节点
			TravelSiblingAndParent(hParentItem,iState);
		}
		else
		{
			//状态不一致，则当前节点的父节点、父节点的父节点……状态均为第三态
			hParentItem=GetParentItem(hItem);
			while(hParentItem!=NULL)
			{
				iState1 = GetItemState( hParentItem, TVIS_STATEIMAGEMASK ) >> 12;
				if(iState1!=0)
				{
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
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;

	// 设置图标成相应的图标
	if(TVIS_EXPANDED == (GetItemState(hItem,TVIS_EXPANDED) & TVIS_EXPANDED)) // 是否已经展开
	{
		// 设置展开图标
		SetItemImage(hItem,0,0);
	}
	else SetItemImage(hItem,1,1);
}
