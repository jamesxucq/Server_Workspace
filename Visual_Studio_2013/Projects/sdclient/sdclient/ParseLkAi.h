#pragma once

#include "clientcom/clientcom.h"
#include "LockedAction.h"


//



//
namespace NParseLkAi {
	static VOID ParseFileLkAi(LkAiVec *pFileLkAi, LkAiVec *pModiLkAi, LkAiVec *pAttaExis, const TCHAR *szDriveLetter);
	static VOID CleanLkAiNode(LkAiVec *pLkAiVec, LkAiVec *pExisAtta);
	// Ω‚ŒˆLkAiVec…˙≥…copy move
	VOID ParseAnchoLkAi(LkAiVec *pLkAiVec, const TCHAR *szDriveLetter);
	// 0x00:ok !0:exception
	DWORD AnchoLkAiAddi(LkAiVec *pLkAiVec, const TCHAR *szDriveLetter, const TCHAR *szLocation);
	DWORD InitiLkAiAddi(LkAiVec *pLkAiVec, const TCHAR *szDriveLetter, const TCHAR *szLocation);
	VOID FinalLkAiAddi();
};
