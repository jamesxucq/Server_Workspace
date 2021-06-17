#pragma once

#include "./utility/TinyPipe.h"

#include <vector>
using  std::vector;

struct Valid {
	DWORD dwValidType;		/* add mod del list recu */
	TCHAR szFileName1[MAX_PATH];
	TCHAR szFileName2[MAX_PATH];
};

namespace NValidVec {
	struct Valid *FillValid(const TCHAR *szFileName, DWORD dwValidType);
	struct Valid *FillValid(const TCHAR *szFileName1, const TCHAR *szFileName2, DWORD dwValidType);
	void VoidValid(struct Valid *pValid);
	struct Valid *NewValid(struct Valid *pValid);
	void DelValid(struct Valid *pValid);
};

class CValidLayer {
public:
	CValidLayer(void);
	~CValidLayer(void);
public:
	void Initialize(void);
	void Finalize(void);
private:
	CWinThread *m_pWinThread;
private:
	LONG m_bProcessStarted;
	CTinyPipe m_objValidPipe;
public:
	struct Valid *previousValid;
	CRITICAL_SECTION m_csValidLink;
public:
	void InsValidate(struct Valid *pValid);
	void DeleteCached();
private:
	static UINT ValidActionThread(LPVOID lpParam);
};

#define VALID_NONE					  0x00000000 // none
#define VALID_ADDITION                0x00000001 // addition
#define VALID_MODIFY                  0x00000002 // modify
#define VALID_DELETE                  0x00000004 // delete
#define VALID_MOVE					  0x00000008 // move
#define VALID_DIRECTORY				  0x00000010 // directory