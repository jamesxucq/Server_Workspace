#include "StdAfx.h"
#include "FilesDenied.h"

#include "third_party.h"
#include "StringUtility.h"
#include "ShellInterface.h"

CFilesDenied::CFilesDenied(void)
{
}

CFilesDenied::~CFilesDenied(void)
{
}

CFilesDenied objFilesDenied;


DWORD CFilesDenied::Initialize()
{

	return 0;
}

DWORD CFilesDenied::Finalize()
{
	vector <struct FileForbid *>::iterator iter;

	for(iter=m_vFilesDeniedVec.begin(); iter!=m_vFilesDeniedVec.end(); iter++) {
		if(*iter) free(*iter);
	}
	m_vFilesDeniedVec.clear();

	return 0;
}


DWORD CFilesDenied::BuildFilesDeniedXml(OUT CString &strXml, IN vector <struct FileForbid *> &vFilesDenied)
{
	vector <struct FileForbid *>::iterator iter;
	CMarkup xml;

	xml.SetDoc(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"));
	xml.AddElem(_T("Forbid"));
	xml.AddAttrib(_T("Version"), _T("1.2.1"));//Ìí¼ÓÊôÐÔ
	xml.IntoElem(); //Forbid
	for(iter=vFilesDenied.begin(); iter!=vFilesDenied.end(); iter++) {
		if(FILE_STATUS_FORBID & (*iter)->dwAttrib) {
			if(FILE_STATUS_DIRECTORY & (*iter)->dwAttrib)
				xml.AddElem(_T("Dir"), (*iter)->filepath);
			else xml.AddElem(_T("File"), (*iter)->filepath);
			xml.AddAttrib( _T("OwnerType"), (*iter)->szOwnerType);
		}
	}
	xml.OutOfElem(); //Forbid
	strXml = xml.GetDoc();

	return 0;
}

DWORD CFilesDenied::ParseFilesDeniedXml(OUT vector <struct FileForbid *> &vFilesDenied, IN CString &strXml)
{
	CMarkup xml;
	CString csLable = _T("");
	CString csAttrib = _T(""); 
	CString csValue = _T(""); 
	struct FileForbid *file_status;

	xml.SetDoc(strXml);
	xml.ResetMainPos();  
	xml.FindElem(_T("Forbid"));
	xml.IntoElem(); //Forbid
	while (xml.FindElem())
	{
		csLable = xml.GetTagName();
		csAttrib = xml.GetAttrib(_T("OwnerType"));
		csValue = xml.GetData();
		file_status = (struct FileForbid *)malloc(sizeof(struct FileForbid));

		if (_T("File") == csLable) {
			_tcscpy_s(file_status->szOwnerType, csAttrib);
			_tcscpy_s(file_status->filepath, csValue);
			file_status->dwAttrib = FILE_STATUS_FORBID;
		}
		else if (_T("Dir") == csLable) {
			_tcscpy_s(file_status->szOwnerType, csAttrib);
			_tcscpy_s(file_status->filepath, csValue);
			file_status->dwAttrib = FILE_STATUS_DIRECTORY | FILE_STATUS_FORBID;			
		}
		vFilesDenied.push_back(file_status);
	}
	xml.OutOfElem(); //Forbid

	return 0;
}


DWORD CFilesDenied::SaveFilesDenied()
{
	CMarkup xml;
	CString strXml;

	if (BuildFilesDeniedXml(strXml, m_vFilesDeniedVec)) return -1;

	if (!xml.SetDoc(strXml)) return -1;
	if (!xml.Save(m_szFilesDeniedPath)) return -1;

	return 0;
}

#define FULL_PATH(PATH, DRIVE)	\
{ \
	_tcscpy_s(szFilePath, MAX_PATH, PATH); \
	_tcscpy_s(PATH, MAX_PATH, DRIVE); \
	_tcscat_s(PATH, MAX_PATH, szFilePath); \
}
DWORD CFilesDenied::LoadFilesDenied(OUT vector <struct FileForbid *> **pFilesVec, IN LPWSTR pXmlFilePath)
{
	vector <struct FileForbid *>::iterator iter;
	//TCHAR szFilePath[MAX_PATH];
	CMarkup xml;
	CString strXml;

	if (!xml.Load(pXmlFilePath)) return -1;
	strXml = xml.GetDoc();

	if (ParseFilesDeniedXml(m_vFilesDeniedVec, strXml)) return -1;
	//for(iter=m_vFilesDeniedVec.begin(); iter!=m_vFilesDeniedVec.end(); iter++)
	//	FULL_PATH((*iter)->filepath, lpDriveRoot)

	_tcscpy_s(m_szFilesDeniedPath, pXmlFilePath);
	*pFilesVec = &m_vFilesDeniedVec;

	return 0;
}

vector <struct FileForbid *> *CFilesDenied::LoadFilesDenied(IN LPWSTR pXmlFilePath)
{
	vector <struct FileForbid *>::iterator iter;
	//TCHAR szFilePath[MAX_PATH];
	CMarkup xml;
	CString strXml;

	if (!xml.Load(pXmlFilePath)) return &m_vFilesDeniedVec;
	strXml = xml.GetDoc();

	if (ParseFilesDeniedXml(m_vFilesDeniedVec, strXml)) return &m_vFilesDeniedVec;
	//for(iter=m_vFilesDeniedVec.begin(); iter!=m_vFilesDeniedVec.end(); iter++)
	//	FULL_PATH((*iter)->filepath, lpDriveRoot)

	_tcscpy_s(m_szFilesDeniedPath, pXmlFilePath);

	return &m_vFilesDeniedVec;
}

struct FileForbid *NFilesDenied::AppendNewNode(vector <struct FileForbid *> &vFilesDeniedVec)
{
	struct FileForbid *NewFileForbid;

	NewFileForbid = (struct FileForbid *)malloc(sizeof(struct FileForbid));
	if(!NewFileForbid) return NULL;
	memset(NewFileForbid, '\0', sizeof(struct FileForbid));

	vFilesDeniedVec.push_back(NewFileForbid);

	return NewFileForbid;
}

void NFilesDenied::DeleteForbid(struct FileForbid *tpFileForbid)
{ free(tpFileForbid); }