#pragma once

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <unordered_map>
using std::unordered_map;
#include <vector>
using  std::vector;

//
#define COFT_ATTR_SUCCE	0x0001
#define COFT_ATTR_EXCEP	0x0000
#define SECF_ATTR_SUCCE(EXCEP_VALUE)	(EXCEP_VALUE = (0xFF00&EXCEP_VALUE)|COFT_ATTR_SUCCE)
#define SECF_ATTR_EXCEP(EXCEP_VALUE)	(EXCEP_VALUE = (0xFF00&EXCEP_VALUE)|COFT_ATTR_EXCEP)
#define COFT_ATTR_DONE(EXCEP_VALUE)		(0x00FF & EXCEP_VALUE)

//
#define COFT_HAND_SUCCE	0x0100
#define COFT_HAND_EXCEP	0x0000
#define SECF_HAND_SUCCE(EXCEP_VALUE)	(EXCEP_VALUE = (0x00FF&EXCEP_VALUE)|COFT_HAND_SUCCE)
#define SECF_HAND_EXCEP(EXCEP_VALUE)	(EXCEP_VALUE = (0x00FF&EXCEP_VALUE)|COFT_HAND_EXCEP)
#define COFT_HAND_DONE(EXCEP_VALUE)		(0xFF00 & EXCEP_VALUE)

//
struct FileColi {
	TCHAR szFileName[MAX_PATH];
	unsigned __int64 qwFileSize;
	FILETIME ftLastWrite; /* When the item was last modified */
	DWORD dwExcepValue;
};
typedef vector <struct FileColi *> FileColiVec;
typedef unordered_map <wstring, struct FileColi*> FileColiHmap;

class CFileColi {
public:
	CFileColi(void);
	~CFileColi(void);
public:
	DWORD Initialize(LPCTSTR pXmlFilePath);
	DWORD Finalize();
public:
	TCHAR m_szColiPath[MAX_PATH];
	FileColiVec m_vConfliVec;
private:
	DWORD BuildColiXml(CString &csXml, FileColiVec &vConfliVec);
	DWORD ParseColiXml(FileColiVec &vConfliVec, CString &csXml);
public:
	DWORD SaveFileColi();
	DWORD LoadFileColi(LPCTSTR pXmlFilePath);
};
extern CFileColi objFileColi;

namespace NFileColi {
	DWORD Initialize(LPCTSTR pXmlFilePath);
	inline DWORD Finalize() { return objFileColi.Finalize();}
	//
	DWORD FileColiSerial();
	inline FileColiVec *GetFileColiPoint() { return &objFileColi.m_vConfliVec; }
	//
	struct FileColi *AddNewColi(FileColiVec *pColiVec, TCHAR *szFileName);
	VOID DeleColi(struct FileColi *pFileColi);
	void BuildColiHmap(FileColiHmap *pColiHmap, FileColiVec *pColiVec);
	VOID DeleColiHmap(FileColiHmap *pColiHmap);
};