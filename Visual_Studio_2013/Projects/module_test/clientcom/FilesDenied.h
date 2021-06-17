#pragma once

#include <vector>
using std::vector;

struct FileForbid {
	TCHAR filepath[MAX_PATH];
	DWORD dwAttrib;
	TCHAR szOwnerType[8];
};

class CFilesDenied
{
public:
	CFilesDenied(void);
	~CFilesDenied(void);
public:
	DWORD Initialize();
	DWORD Finalize();
private:
	TCHAR		m_szFilesDeniedPath[MAX_PATH];
	vector <struct FileForbid *> m_vFilesDeniedVec;
private:
	DWORD BuildFilesDeniedXml(OUT CString &strXml, IN vector <struct FileForbid *> &vFilesDenied);
	DWORD ParseFilesDeniedXml(OUT vector <struct FileForbid *> &vFilesDenied, IN CString &strXml);
public:
	DWORD SaveFilesDenied();
	vector <struct FileForbid *> *LoadFilesDenied(IN LPWSTR pXmlFilePath);
	DWORD LoadFilesDenied(OUT vector <struct FileForbid *> **pFilesVec, IN LPWSTR pXmlFilePath);
public:
	inline void SetFilesDenied(IN vector <struct FileForbid *> &vFilesDenied) {m_vFilesDeniedVec = vFilesDenied;}
	inline vector <struct FileForbid *> &GetFilesDeniedVec() {return m_vFilesDeniedVec;}
	inline vector <struct FileForbid *> *GetFilesDeniedPoint() {return &m_vFilesDeniedVec;}
};
extern CFilesDenied objFilesDenied;

namespace NFilesDenied
{
	struct FileForbid *AppendNewNode(vector <struct FileForbid *> &vFilesDeniedVec);
	void DeleteForbid(struct FileForbid *tpFileForbid);
};