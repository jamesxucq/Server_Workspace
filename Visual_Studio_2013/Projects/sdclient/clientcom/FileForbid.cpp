#include "StdAfx.h"
#include "FileForbid.h"

#include "third_party.h"
#include "StringUtility.h"
#include "ShellInterface.h"

CFileFobd::CFileFobd(void) {
	memset(m_szFobdPath, 0, MAX_PATH);
}

CFileFobd::~CFileFobd(void) {
}

CFileFobd objFileFobd;


DWORD CFileFobd::Initialize(LPCTSTR pXmlFilePath) {
	return LoadFileFobd(pXmlFilePath);
}

DWORD CFileFobd::Finalize() {
	FileFobdVec::iterator iter;
//
	for(iter=m_vFobdVec.begin(); m_vFobdVec.end()!=iter; ++iter) {
		if(*iter) free(*iter);
	}
	m_vFobdVec.clear();
//
	return 0x00;
}


DWORD CFileFobd::BuildFobdXml(CString &csXml, FileFobdVec &vFileFobd) {
	FileFobdVec::iterator iter;
	CMarkup xml;
//
	xml.SetDoc(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r"));
	xml.AddElem(_T("Forbid"));
	xml.AddAttrib(_T("Version"), _T("1.2.1"));//Ìí¼ÓÊôÐÔ
	xml.IntoElem(); // Forbid
	for(iter=vFileFobd.begin(); vFileFobd.end()!=iter; ++iter) {
		if(FILE_STATUS_FORBID & (*iter)->dwAttrib) {
			if(FILE_STATUS_DIRECTORY & (*iter)->dwAttrib)
				xml.AddElem(_T("Dire"), (*iter)->szFilePath);
			else if(FILE_STATUS_FILE & (*iter)->dwAttrib)
				xml.AddElem(_T("File"), (*iter)->szFilePath);
			xml.AddAttrib( _T("OwnerType"), (*iter)->szOwnerType);
		}
	}
	xml.OutOfElem(); // Forbid
	csXml = xml.GetDoc();
//
	return 0x00;
}

DWORD CFileFobd::ParseFobdXml(FileFobdVec &vFileFobd, CString &csXml) {
	CMarkup xml;
	CString csLable = _T("");
	CString csAttrib = _T(""); 
	CString csValue = _T(""); 
	struct FileFobd *pFileFobd;
//
	xml.SetDoc(csXml);
	xml.ResetMainPos();  
	xml.FindElem(_T("Forbid"));
	xml.IntoElem(); // Forbid
	while (xml.FindElem()) {
		csLable = xml.GetTagName();
		csAttrib = xml.GetAttrib(_T("OwnerType"));
		csValue = xml.GetData();
		pFileFobd = (struct FileFobd *)malloc(sizeof(struct FileFobd));
		if(!pFileFobd) return ((DWORD)-1);
		//
		if (_T("File") == csLable) {
			_tcscpy_s(pFileFobd->szOwnerType, csAttrib);
			_tcscpy_s(pFileFobd->szFilePath, csValue);
			pFileFobd->dwAttrib = FILE_STATUS_FILE | FILE_STATUS_FORBID;
		} else if (_T("Dire") == csLable) {
			_tcscpy_s(pFileFobd->szOwnerType, csAttrib);
			_tcscpy_s(pFileFobd->szFilePath, csValue);
			pFileFobd->dwAttrib = FILE_STATUS_DIRECTORY | FILE_STATUS_FORBID;			
		}
		vFileFobd.push_back(pFileFobd);
	}
	xml.OutOfElem(); // Forbid
//
	return 0x00;
}


DWORD CFileFobd::SaveFileFobd() {
	CMarkup xml;
	CString csXml;
//
	if (BuildFobdXml(csXml, m_vFobdVec)) return ((DWORD)-1);
	if (!xml.SetDoc(csXml)) return ((DWORD)-1);
	if (!xml.Save(m_szFobdPath)) return ((DWORD)-1);
//
	return 0x00;
}

DWORD CFileFobd::LoadFileFobd(LPCTSTR pXmlFilePath) {
	FileFobdVec::iterator iter;
	// TCHAR szFilePath[MAX_PATH];
	CMarkup xml;
	CString csXml;
//
	if (!xml.Load(pXmlFilePath)) return ((DWORD)-1);
	csXml = xml.GetDoc();
	if (ParseFobdXml(m_vFobdVec, csXml)) return ((DWORD)-1);
	_tcscpy_s(m_szFobdPath, pXmlFilePath);
//
	return 0x00;
}

//
DWORD NFileFobd::GetFileFobdVec(FileFobdVec *pFobdVec) {
	FileFobdVec *pDenyVec;
	FileFobdVec::iterator fite;
//
	if(!pFobdVec) return ((DWORD)-1);
	pDenyVec = &objFileFobd.m_vFobdVec;
	for(fite=pDenyVec->begin(); pDenyVec->end()!=fite; ++fite) {
		if((*fite)->dwAttrib&FILE_STATUS_DIRECTORY && (*fite)->dwAttrib&FILE_STATUS_FORBID && !_tcscmp((*fite)->szOwnerType, _T("client")))
			pFobdVec->push_back(*fite);
	}
//
	return 0x00;
}

struct FileFobd *NFileFobd::AddNewFobd(FileFobdVec *pFobdVec) {
	struct FileFobd *pFileFobd;
	pFileFobd = (struct FileFobd *)malloc(sizeof(struct FileFobd));
	if(!pFileFobd) return NULL;
	pFobdVec->push_back(pFileFobd);
	return pFileFobd;
}

VOID NFileFobd::DeleFobd(struct FileFobd *pFileFobd)
{ if(pFileFobd) free(pFileFobd); }