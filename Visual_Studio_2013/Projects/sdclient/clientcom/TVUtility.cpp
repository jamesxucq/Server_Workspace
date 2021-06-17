
#include "stdafx.h"
#include "StringUtility.h"

#include "TVUtility.h"

HTREEITEM NTVUtility::AddChildrenNode(CAdvaTView *tvDiresTView, HTREEITEM hParentItem, const TCHAR *szSubDirectory) {
    static DWORD dwAddNo = 0;
    // HTREEITEM hRoot,hCur;	// 树控制项目句柄
    TV_INSERTSTRUCT TVItem;	// 插入数据项数据结构
    //
    if(hParentItem) TVItem.hParent = hParentItem;	// 增加根项
    else TVItem.hParent = TVI_ROOT;	// 增加根项
    TVItem.hInsertAfter = TVI_LAST;	// 在最后项之后
    TVItem.item.mask = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE; // 设屏蔽
    TVItem.item.state = INDEXTOSTATEIMAGEMASK(2);
    TVItem.item.stateMask = TVIS_STATEIMAGEMASK;
    TVItem.item.pszText = (TCHAR *)szSubDirectory;
    TVItem.item.iImage = 0;	// 正常图标
    TVItem.item.iSelectedImage = 1;	// 选中时图标
    TVItem.item.lParam = dwAddNo++;	// 序号
    //
    return tvDiresTView->InsertItem(&TVItem);	// 返回根项句
}

/*
HTREEITEM InsertChildrenNode(CAdvaTView *tvDiresTView, HTREEITEM hParentItem, const TCHAR *szSubDirectory) {
    CString csItemText;
    //
    HTREEITEM hChildItem = tvDiresTView->GetChildItem( hParentItem );
    if( hChildItem ) {
        csItemText = tvDiresTView->GetItemText(hChildItem);
        if(!_tcscmp(szSubDirectory, csItemText)) 
			return hChildItem;
	} else {
		return AddChildrenNode(tvDiresTView, hParentItem, szSubDirectory);
	}
    //
    HTREEITEM hSlibItem = tvDiresTView->GetNextSiblingItem(hChildItem);
    while( hSlibItem ) {
        csItemText = tvDiresTView->GetItemText(hSlibItem);
        if(!_tcscmp(szSubDirectory, csItemText)) 
			return hSlibItem;
//
        hSlibItem = tvDiresTView->GetNextSiblingItem(hSlibItem);
    }
    //
    return AddChildrenNode(tvDiresTView, hParentItem, szSubDirectory);
}
*/

/*
bool InsertAdvancedDirectory(CAdvaTView *tvDiresTView, const TCHAR *szDirectory) {
    TCHAR *NextToken;
    TCHAR *pValue = NULL;
    TCHAR szDireName[MAX_PATH];
    // HTREEITEM hParentItem = tvDiresTView->GetRootItem();
    HTREEITEM hParentItem = NULL;
    //
// _LOG_DEBUG(_T("------ szDirectory:%s"), szDirectory);
    _tcscpy_s(szDireName, szDirectory);
    pValue = wcstok_s(szDireName, _T("\\"), &NextToken);
    hParentItem = InsertChildrenNode(tvDiresTView, hParentItem, pValue);
    //
    while(pValue = wcstok_s(NULL, _T("\\"), &NextToken)) {
        // pValue = wcstok_s(szDirectory, _T("\\"), &NextToken);
        hParentItem = InsertChildrenNode(tvDiresTView, hParentItem, pValue);
    }
    //
    return true;
}
*/

// 设置一个节点为Checked状态或者Unchecked状态，由参数IsSetToChecked来确定
/*
VOID NTVUtility::SetNodeChecked(CAdvaTView *tvDiresTView, HTREEITEM hItem, bool IsSetToChecked) {
    TVITEM TVItem;
    //
    TVItem.mask = TVIF_HANDLE | TVIF_STATE;
    TVItem.hItem = hItem;
    TVItem.stateMask = TVIS_STATEIMAGEMASK;
    TVItem.state = INDEXTOSTATEIMAGEMASK(IsSetToChecked?3:1);
    //
    TreeView_SetItem(tvDiresTView->m_hWnd, &TVItem);
}
*/

// selected 0x00:checked 0x01:part uncheck 0x02:all uncheck
// state: 0->无选择钮 1->没有选择 2->部分选择 3->全部选择
VOID NTVUtility::SetNodeChecked(CAdvaTView *tvDiresTView, HTREEITEM hItem, DWORD dwSelectState) {
	static DWORD dwStateConv[3] = {0x03, 0x02, 0x01};
    TVITEM TVItem;
    //
    TVItem.mask = TVIF_HANDLE | TVIF_STATE;
    TVItem.hItem = hItem;
    TVItem.stateMask = TVIS_STATEIMAGEMASK;
    TVItem.state = INDEXTOSTATEIMAGEMASK(dwStateConv[dwSelectState]);
    //
    TreeView_SetItem(tvDiresTView->m_hWnd, &TVItem);
}

/*
// 把它的子孙都置成一样
VOID SetChildChecked(CAdvaTView *tvDiresTView, HTREEITEM hItem, bool IsSetToChecked) {
    // step1:设置节点为Unchecked状态
    SetNodeChecked(tvDiresTView, hItem, IsSetToChecked);
    // step1:检查子结点
    HTREEITEM hChildItem = tvDiresTView->GetChildItem( hItem );
    while( hChildItem ) {
        SetChildChecked(tvDiresTView, hChildItem, IsSetToChecked) ;                //递归
        hChildItem = tvDiresTView->GetChildItem( hChildItem );
    }
    // step2: 检查兄弟结点
    HTREEITEM hSlibItem = tvDiresTView->GetNextSiblingItem(hItem);
    while( hSlibItem ) {
        SetChildChecked(tvDiresTView, hSlibItem, IsSetToChecked) ;                //递归
        hSlibItem = tvDiresTView->GetNextSiblingItem(hSlibItem);
    }
}
*/

/*
HTREEITEM GetDirectoryTreeNode(CAdvaTView *tvDiresTView, HTREEITEM hParentItem, TCHAR *szSubDirectory) {
    CString csItemText;
    if(!szSubDirectory) return NULL;
    //
    HTREEITEM hChildItem = tvDiresTView->GetChildItem( hParentItem );
    if( hChildItem ) {
        csItemText = tvDiresTView->GetItemText(hChildItem);
        if(!_tcscmp(szSubDirectory, csItemText)) return hChildItem;
	} else {
		return NULL;
	}
    //
    HTREEITEM hSlibItem = tvDiresTView->GetNextSiblingItem(hChildItem);
    while( hSlibItem ) {
        csItemText = tvDiresTView->GetItemText(hSlibItem);
        if(!_tcscmp(szSubDirectory, csItemText)) return hSlibItem;
//
        hSlibItem = tvDiresTView->GetNextSiblingItem(hSlibItem);
    }
    //
    return NULL;
}
*/

/*
bool SetDirectoryUnchecked(CAdvaTView *tvDiresTView, TCHAR *szDirectory) {
    TCHAR *NextToken;
    TCHAR *pValue = NULL;
    TCHAR szDireName[MAX_PATH];
    //
    HTREEITEM hParentItem = NULL;
    _tcscpy_s(szDireName, szDirectory);
    pValue = wcstok_s(szDireName, _T("\\"), &NextToken);
    hParentItem = GetDirectoryTreeNode(tvDiresTView, hParentItem, pValue);
//
    while(pValue = wcstok_s(NULL, _T("\\"), &NextToken)) {
        hParentItem = GetDirectoryTreeNode(tvDiresTView, hParentItem, pValue);
    }
    // if(hParentItem) SetChildChecked(tvDiresTView, hParentItem, false);
    if(hParentItem)
        tvDiresTView->SetItemState(hParentItem,INDEXTOSTATEIMAGEMASK(1),TVIS_STATEIMAGEMASK);
    //
    return true;
}
*/

/*
VOID SetItemsChecked(CAdvaTView *tvDiresTView, HTREEITEM hItem) {
    // step1:开始节点设置为选中.
    if(hItem) SetChildChecked(tvDiresTView, hItem, true);
    // step1:检查子结点
    if(tvDiresTView->ItemHasChildren(hItem)) {
        HTREEITEM hChildItem = tvDiresTView->GetChildItem(hItem);
        while(hChildItem) {
            SetItemsChecked(tvDiresTView, hChildItem); //递归遍历孩子节点
            hChildItem = tvDiresTView->GetNextSiblingItem(hChildItem);
        }
    }
}
*/

/*
VOID SetAllItemsChecked(CAdvaTView *tvDiresTView) {
    HTREEITEM hRootItem = tvDiresTView->GetRootItem();
    SetItemsChecked(tvDiresTView, hRootItem);
    // step2: 检查兄弟结点
    HTREEITEM hSlibItem = tvDiresTView->GetNextSiblingItem(hRootItem);
    while( hSlibItem ) {
        SetItemsChecked(tvDiresTView, hSlibItem);
        hSlibItem = tvDiresTView->GetNextSiblingItem(hSlibItem);
    }
}
*/

// 判断一个节点是否处于Checked状态
bool GetNodeChecked(CAdvaTView *tvDiresTView, HTREEITEM hItem) {
    TVITEM TVItem;
    //
    TVItem.mask = TVIF_HANDLE | TVIF_STATE;
    TVItem.hItem = hItem;
    TVItem.stateMask = TVIS_STATEIMAGEMASK;
    TreeView_GetItem(tvDiresTView->m_hWnd, &TVItem);
    //
    return ((BOOL)(TVItem.state>>12)-1)? true: false;
}

VOID GetDiresUnchecked(DiresVec *pListVec, CAdvaTView *tvDiresTView, HTREEITEM hItem, TCHAR *szDirectory) {
    TCHAR szDireName[MAX_PATH];
    // step1:检验开始节点
    CString csItemText = tvDiresTView->GetItemText(hItem);
    _stprintf_s(szDireName, _T("%s%s"), szDirectory, csItemText);
    if(!GetNodeChecked(tvDiresTView, hItem)) {
        TCHAR *szNewDire = NDiresVec::AddNewDiresNode(pListVec);
        _tcscpy_s(szNewDire, MAX_PATH, szDireName);
		CLEAR_LACHR(szNewDire)
    } else { // step1:检查子结点
        if(tvDiresTView->ItemHasChildren(hItem)) {
            HTREEITEM hChildItem = tvDiresTView->GetChildItem(hItem);
            while(hChildItem) {
                GetDiresUnchecked(pListVec, tvDiresTView, hChildItem, szDireName); //递归遍历孩子节点
                hChildItem = tvDiresTView->GetNextSiblingItem(hChildItem);
            }
        }
    }
}

VOID NTVUtility::GetAllDiresUnchecked(DiresVec *pListVec, CAdvaTView *tvDiresTView) {
    HTREEITEM hRootItem = tvDiresTView->GetRootItem();
    GetDiresUnchecked(pListVec, tvDiresTView, hRootItem, _T(""));
    // step2: 检查兄弟结点
    HTREEITEM hSlibItem = tvDiresTView->GetNextSiblingItem(hRootItem);
    while( hSlibItem ) {
        GetDiresUnchecked(pListVec, tvDiresTView, hSlibItem, _T(""));
        hSlibItem = tvDiresTView->GetNextSiblingItem(hSlibItem);
    }
}

/*
// 函数说明：返回值：与strId对应的节点.
// 参数： hItem 查找的起始节点， strId 待查找的数据的标识符，开始用SetItemData设置
// ITEMDATA为自定义的数据结构
HTREEITEM TreeVisit(HTREEITEM hItem,const CString& strId) {
	// step1:检验开始节点.
	//	ITEMDATA *pData=(ITEMDATA*) GetItemData( hItem );

	//    if( pData ) {
	//		if( 0 == pData->strID.Compare( strID ) )
	//             return hItem;
	//	}

	// step1:检查子结点
	HTREEITEM hChildItem = GetChildItem( hItem );
	HTREEITEM hFindItem = NULL;
	while( hChildItem ) {
		hFindItem = TreeVisit(hChildItem, strId) ;                // 递归
		if( hFindItem ) return hFindItem;

		hChildItem = GetChildItem( hChildItem );
	}

	// step2: 检查兄弟结点
	HTREEITEM hSlibItem = GetNextSiblingItem(hItem, TVGN_NEXT);
	while( hSlibItem ) {
		hFindItem = TreeVisit(hSlibItem, strId) ;                // 递归
		if( hFindItem ) return hFindItem;

		hChildItem = GetNextSiblingItem(hSlibItem,TVGN_NEXT);
	}
	return NULL;   // 不能少
}
*/

TCHAR *NTVUtility::GetDirectoryPath(TCHAR *path, CAdvaTView *tvDiresTView, HTREEITEM hItem, HTREEITEM hRootItem) {
	CString csDireName;
	CString csPath = _T("");
//
	while(hItem != hRootItem) {
		csDireName = tvDiresTView->GetItemText(hItem); // 检索列表中项目文字
		csPath = csDireName + csPath;        
		hItem = tvDiresTView->GetParentItem(hItem); // 返回父项目句柄
	}
	path[0] = _T('\\');
	strcon::cstr_ustr(0x01+path, csPath);
//
	return path;
}