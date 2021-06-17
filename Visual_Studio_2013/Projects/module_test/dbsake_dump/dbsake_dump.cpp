// dbsake_dump.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "windows.h"

#include "..\dbz\kchashdb.h"

using namespace std;
using namespace kyotocabinet;

#pragma comment(lib, "..\\third party\\dbz\\dbz.lib")

#define FILES_NSAKE_DEFAULT				_T(".\\files_nsake_bin.kct")

char* unicode_ansi(char* ansi, const TCHAR* unicode) {
	// unicode to ansi
	int alength = WideCharToMultiByte(CP_ACP, NULL, unicode, (int)wcslen(unicode), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, NULL, unicode, (int)wcslen(unicode), ansi, alength, NULL, NULL);
	ansi[alength] = '\0';

	return ansi;
}

int _tmain(int argc, _TCHAR* argv[])
{
	char fileName[MAX_PATH];
	unicode_ansi(fileName, FILES_NSAKE_DEFAULT);

	if(2 != argc) {
		printf("input error! argc:%d\n", argc);
		return -1;
	}
	TCHAR szFileName[MAX_PATH];
	_tcscpy_s(szFileName, argv[1]);
	// create the database object
	TreeDB db;

	// open the database
	if (!db.open(fileName, TreeDB::OWRITER | TreeDB::OCREATE)) {
		cerr << "open error: " << db.error().name() << endl;
	}

	DWORD value;
	DWORD key_len = _tcslen(szFileName);
	if(0 < db.get((const char *)szFileName, key_len<<1, (char *)&value, sizeof(DWORD))) {
		wprintf(_T("%s : %d\n"), szFileName, value);
	} else wprintf(_T("not found\n"));

	// close the database
	if (!db.close()) {
		cerr << "close error: " << db.error().name() << endl;
	}

	return 0;
}

