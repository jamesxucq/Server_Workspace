#ifndef _USER_SPACE_H_
#define _USER_SPACE_H_

namespace Parser {
	TCHAR *ApplicaDataPath(TCHAR *szFilePath, const TCHAR *szFileName);
	int ParseLocation(char *szLocation, TCHAR *szFileName);
};

// 

DWORD __stdcall DeleUserSpace();

#endif /* _USER_SPACE_H_ */
