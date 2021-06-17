#pragma once

#include "AdvaTView.h"

namespace NTVUtility {
	HTREEITEM AddChildrenNode(CAdvaTView *tvDiresTView, HTREEITEM hParentItem, const TCHAR *szSubDirectory);
	VOID SetNodeChecked(CAdvaTView *tvDiresTView, HTREEITEM hItem, DWORD dwSelectState);
	VOID GetAllDiresUnchecked(DiresVec *pListVec, CAdvaTView *tvDiresTView);
//
	TCHAR *GetDirectoryPath(TCHAR *path, CAdvaTView *tvDiresTView, HTREEITEM hItem, HTREEITEM hRootItem);
};
