#include "StdAfx.h"
#include "FileConfli.h"

#include "third_party.h"

CFileColi::CFileColi(void) {
}

CFileColi::~CFileColi(void) {
}

CFileColi objFileColi;


DWORD CFileColi::Initialize(LPCTSTR pXmlFilePath) {
// _LOG_DEBUG(_T("######## load file coli!"));
	return LoadFileColi(pXmlFilePath);
}

DWORD CFileColi::Finalize() {
// _LOG_DEBUG(_T("######## file coli finalize!"));
	FileColiVec::iterator iter;
	for(iter=m_vConfliVec.begin(); m_vConfliVec.end()!=iter; ++iter) {
		if(*iter) free(*iter);
	}
	m_vConfliVec.clear();
	return 0x00;
}


DWORD CFileColi::BuildColiXml(CString &csXml, FileColiVec &vConfliVec) {
	FileColiVec::iterator iter;
	CMarkup xml;
//
	xml.SetDoc(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r"));
	xml.AddElem(_T("Conflict"));
	xml.AddAttrib(_T("Version"), _T("1.2.1"));//Ìí¼ÓÊôÐÔ
	xml.IntoElem(); // Confli
	for(iter=vConfliVec.begin(); vConfliVec.end()!=iter; ++iter) {
		xml.AddElem(_T("File"), (*iter)->szFileName);
// _LOG_DEBUG(_T("######## Build szFileName:%s"), (*iter)->szFileName);
	}
	xml.OutOfElem(); // Confli
	csXml = xml.GetDoc();
//
	return 0x00;
}

DWORD CFileColi::ParseColiXml(FileColiVec &vConfliVec, CString &csXml) {
	CMarkup xml;
	CString csLable = _T("");
	CString csValue = _T(""); 
	struct FileColi *pFileColi;
//
	xml.SetDoc(csXml);
	xml.ResetMainPos();  
	xml.FindElem(_T("Conflict"));
	xml.IntoElem(); // Confli
	while (xml.FindElem()) {
		csLable = xml.GetTagName();
		csValue = xml.GetData();
		pFileColi = (struct FileColi *)malloc(sizeof(struct FileColi));
		if(!pFileColi) return ((DWORD)-1);
		memset(pFileColi, '\0', sizeof(struct FileColi));
		//
		if (_T("File") == csLable) {
			_tcscpy_s(pFileColi->szFileName, csValue);
		}
// _LOG_DEBUG(_T("######## Parse szFileName:%s, dwExcepValue:%08X"), pFileColi->szFileName, pFileColi->dwExcepValue);
		vConfliVec.push_back(pFileColi);
	}
	xml.OutOfElem(); // Confli
//
	return 0x00;
}

DWORD CFileColi::SaveFileColi() {
	CMarkup xml;
	CString csXml;
//
	if (BuildColiXml(csXml, m_vConfliVec)) return ((DWORD)-1);
//
	if (!xml.SetDoc(csXml)) return ((DWORD)-1);
	if (!xml.Save(m_szColiPath)) return ((DWORD)-1);
//
	return 0x00;
}

DWORD CFileColi::LoadFileColi(LPCTSTR pXmlFilePath) {
	FileColiVec::iterator iter;
	// TCHAR szFilePath[MAX_PATH];
	CMarkup xml;
	CString csXml;
//
	if (!xml.Load(pXmlFilePath)) return ((DWORD)-1);
	csXml = xml.GetDoc();
	if (ParseColiXml(m_vConfliVec, csXml)) return ((DWORD)-1);
	_tcscpy_s(m_szColiPath, pXmlFilePath);
//
	return 0x00;
}

//
DWORD NFileColi::Initialize(LPCTSTR pXmlFilePath) { 
	objFileColi.Initialize(pXmlFilePath); 
	return 0x00;
}

//
DWORD NFileColi::FileColiSerial() { 
	FileColiVec *pColiVec = &objFileColi.m_vConfliVec;
	FileColiVec::iterator iter;
	for(iter=pColiVec->begin(); pColiVec->end()!=iter; ) {
// _LOG_DEBUG(_T("######## qwFileSize:%llu, dwExcepValue:%08X, szFileName:%s"), (*iter)->qwFileSize, (*iter)->dwExcepValue, (*iter)->szFileName);
		if(COFT_HAND_DONE((*iter)->dwExcepValue)) {
// _LOG_DEBUG(_T("######## Dele szFileName:%s"), (*iter)->szFileName);
			NFileColi::DeleColi(*iter);
			iter = pColiVec->erase(iter);
		} else ++iter;
	}
	return objFileColi.SaveFileColi(); 
}

struct FileColi *NFileColi::AddNewColi(FileColiVec *pColiVec, TCHAR *szFileName) {
	struct FileColi *pFileColi;
	pFileColi = (struct FileColi *)malloc(sizeof(struct FileColi));
	if(!pFileColi) return NULL;
	memset(pFileColi, '\0', sizeof(struct FileColi));
	_tcscpy_s(pFileColi->szFileName, szFileName);
// _LOG_DEBUG(_T("######## Add szFileName:%s, dwExcepValue:%08X"), szFileName, pFileColi->dwExcepValue);
	pColiVec->push_back(pFileColi);
	return pFileColi;
}

VOID NFileColi::DeleColi(struct FileColi *pFileColi)
{ if(pFileColi) free(pFileColi); }

VOID NFileColi::DeleColiHmap(FileColiHmap *pColiHmap) {
    pColiHmap->clear();
}

void NFileColi::BuildColiHmap(FileColiHmap *pColiHmap, FileColiVec *pColiVec) {
	FileColiVec::iterator iter;
    for(iter = pColiVec->begin(); pColiVec->end() != iter; ++iter) {
		pColiHmap->insert(FileColiHmap::value_type((*iter)->szFileName, (*iter)));
	}
}