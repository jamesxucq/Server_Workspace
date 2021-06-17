#pragma once

#include <vector>
using std::vector;

struct FileFobd {
	TCHAR szFilePath[MAX_PATH];
	DWORD dwAttrib;
	TCHAR szOwnerType[8];
};
typedef vector <struct FileFobd *> FileFobdVec;

//
class CFileFobd {
public:
	CFileFobd(void);
	~CFileFobd(void);
public:
	DWORD Initialize(LPCTSTR pXmlFilePath);
	DWORD Finalize();
public:
	TCHAR m_szFobdPath[MAX_PATH];
	vector <struct FileFobd *> m_vFobdVec;
private:
	DWORD BuildFobdXml(CString &csXml, FileFobdVec &vFileFobd);
	DWORD ParseFobdXml(FileFobdVec &vFileFobd, CString &csXml);
public:
	DWORD SaveFileFobd();
	DWORD LoadFileFobd(LPCTSTR pXmlFilePath);
};
extern CFileFobd objFileFobd;

namespace NFileFobd {
	inline DWORD Initialize(LPCTSTR pXmlFilePath) { return objFileFobd.Initialize(pXmlFilePath); }
	inline DWORD Finalize() { return objFileFobd.Finalize();}
	//
	inline DWORD SaveFileFobd() { return objFileFobd.SaveFileFobd(); }
	DWORD GetFileFobdVec(FileFobdVec *pFobdVec);
	inline FileFobdVec *GetFileFobdPoint() { return &objFileFobd.m_vFobdVec; }
	//
	struct FileFobd *AddNewFobd(FileFobdVec *pFobdVec);
	VOID DeleFobd(struct FileFobd *pFileFobd);
};