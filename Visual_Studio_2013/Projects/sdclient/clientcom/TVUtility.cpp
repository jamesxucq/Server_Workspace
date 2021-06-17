
#include "stdafx.h"
#include "StringUtility.h"

#include "TVUtility.h"

HTREEITEM NTVUtility::AddChildrenNode(CAdvaTView *tvDiresTView, HTREEITEM hParentItem, const TCHAR *szSubDirectory) {
    static DWORD dwAddNo = 0;
    // HTREEITEM hRoot,hCur;	// ��������Ŀ���
    TV_INSERTSTRUCT TVItem;	// �������������ݽṹ
    //
    if(hParentItem) TVItem.hParent = hParentItem;	// ���Ӹ���
    else TVItem.hParent = TVI_ROOT;	// ���Ӹ���
    TVItem.hInsertAfter = TVI_LAST;	// �������֮��
    TVItem.item.mask = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE; // ������
    TVItem.item.state = INDEXTOSTATEIMAGEMASK(2);
    TVItem.item.stateMask = TVIS_STATEIMAGEMASK;
    TVItem.item.pszText = (TCHAR *)szSubDirectory;
    TVItem.item.iImage = 0;	// ����ͼ��
    TVItem.item.iSelectedImage = 1;	// ѡ��ʱͼ��
    TVItem.item.lParam = dwAddNo++;	// ���
    //
    return tvDiresTView->InsertItem(&TVItem);	// ���ظ����
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

// ����һ���ڵ�ΪChecked״̬����Unchecked״̬���ɲ���IsSetToChecked��ȷ��
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
// state: 0->��ѡ��ť 1->û��ѡ�� 2->����ѡ�� 3->ȫ��ѡ��
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
// ���������ﶼ�ó�һ��
VOID SetChildChecked(CAdvaTView *tvDiresTView, HTREEITEM hItem, bool IsSetToChecked) {
    // step1:���ýڵ�ΪUnchecked״̬
    SetNodeChecked(tvDiresTView, hItem, IsSetToChecked);
    // step1:����ӽ��
    HTREEITEM hChildItem = tvDiresTView->GetChildItem( hItem );
    while( hChildItem ) {
        SetChildChecked(tvDiresTView, hChildItem, IsSetToChecked) ;                //�ݹ�
        hChildItem = tvDiresTView->GetChildItem( hChildItem );
    }
    // step2: ����ֵܽ��
    HTREEITEM hSlibItem = tvDiresTView->GetNextSiblingItem(hItem);
    while( hSlibItem ) {
        SetChildChecked(tvDiresTView, hSlibItem, IsSetToChecked) ;                //�ݹ�
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
    // step1:��ʼ�ڵ�����Ϊѡ��.
    if(hItem) SetChildChecked(tvDiresTView, hItem, true);
    // step1:����ӽ��
    if(tvDiresTView->ItemHasChildren(hItem)) {
        HTREEITEM hChildItem = tvDiresTView->GetChildItem(hItem);
        while(hChildItem) {
            SetItemsChecked(tvDiresTView, hChildItem); //�ݹ�������ӽڵ�
            hChildItem = tvDiresTView->GetNextSiblingItem(hChildItem);
        }
    }
}
*/

/*
VOID SetAllItemsChecked(CAdvaTView *tvDiresTView) {
    HTREEITEM hRootItem = tvDiresTView->GetRootItem();
    SetItemsChecked(tvDiresTView, hRootItem);
    // step2: ����ֵܽ��
    HTREEITEM hSlibItem = tvDiresTView->GetNextSiblingItem(hRootItem);
    while( hSlibItem ) {
        SetItemsChecked(tvDiresTView, hSlibItem);
        hSlibItem = tvDiresTView->GetNextSiblingItem(hSlibItem);
    }
}
*/

// �ж�һ���ڵ��Ƿ���Checked״̬
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
    // step1:���鿪ʼ�ڵ�
    CString csItemText = tvDiresTView->GetItemText(hItem);
    _stprintf_s(szDireName, _T("%s%s"), szDirectory, csItemText);
    if(!GetNodeChecked(tvDiresTView, hItem)) {
        TCHAR *szNewDire = NDiresVec::AddNewDiresNode(pListVec);
        _tcscpy_s(szNewDire, MAX_PATH, szDireName);
		CLEAR_LACHR(szNewDire)
    } else { // step1:����ӽ��
        if(tvDiresTView->ItemHasChildren(hItem)) {
            HTREEITEM hChildItem = tvDiresTView->GetChildItem(hItem);
            while(hChildItem) {
                GetDiresUnchecked(pListVec, tvDiresTView, hChildItem, szDireName); //�ݹ�������ӽڵ�
                hChildItem = tvDiresTView->GetNextSiblingItem(hChildItem);
            }
        }
    }
}

VOID NTVUtility::GetAllDiresUnchecked(DiresVec *pListVec, CAdvaTView *tvDiresTView) {
    HTREEITEM hRootItem = tvDiresTView->GetRootItem();
    GetDiresUnchecked(pListVec, tvDiresTView, hRootItem, _T(""));
    // step2: ����ֵܽ��
    HTREEITEM hSlibItem = tvDiresTView->GetNextSiblingItem(hRootItem);
    while( hSlibItem ) {
        GetDiresUnchecked(pListVec, tvDiresTView, hSlibItem, _T(""));
        hSlibItem = tvDiresTView->GetNextSiblingItem(hSlibItem);
    }
}

/*
// ����˵��������ֵ����strId��Ӧ�Ľڵ�.
// ������ hItem ���ҵ���ʼ�ڵ㣬 strId �����ҵ����ݵı�ʶ������ʼ��SetItemData����
// ITEMDATAΪ�Զ�������ݽṹ
HTREEITEM TreeVisit(HTREEITEM hItem,const CString& strId) {
	// step1:���鿪ʼ�ڵ�.
	//	ITEMDATA *pData=(ITEMDATA*) GetItemData( hItem );

	//    if( pData ) {
	//		if( 0 == pData->strID.Compare( strID ) )
	//             return hItem;
	//	}

	// step1:����ӽ��
	HTREEITEM hChildItem = GetChildItem( hItem );
	HTREEITEM hFindItem = NULL;
	while( hChildItem ) {
		hFindItem = TreeVisit(hChildItem, strId) ;                // �ݹ�
		if( hFindItem ) return hFindItem;

		hChildItem = GetChildItem( hChildItem );
	}

	// step2: ����ֵܽ��
	HTREEITEM hSlibItem = GetNextSiblingItem(hItem, TVGN_NEXT);
	while( hSlibItem ) {
		hFindItem = TreeVisit(hSlibItem, strId) ;                // �ݹ�
		if( hFindItem ) return hFindItem;

		hChildItem = GetNextSiblingItem(hSlibItem,TVGN_NEXT);
	}
	return NULL;   // ������
}
*/

TCHAR *NTVUtility::GetDirectoryPath(TCHAR *path, CAdvaTView *tvDiresTView, HTREEITEM hItem, HTREEITEM hRootItem) {
	CString csDireName;
	CString csPath = _T("");
//
	while(hItem != hRootItem) {
		csDireName = tvDiresTView->GetItemText(hItem); // �����б�����Ŀ����
		csPath = csDireName + csPath;        
		hItem = tvDiresTView->GetParentItem(hItem); // ���ظ���Ŀ���
	}
	path[0] = _T('\\');
	strcon::cstr_ustr(0x01+path, csPath);
//
	return path;
}