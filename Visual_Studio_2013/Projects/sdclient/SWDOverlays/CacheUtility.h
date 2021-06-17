#ifndef _FILEUTILITY_H_
#define _FILEUTILITY_H_

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <unordered_map>
using std::unordered_map;      // namespace where class lives by default
#include <vector>
using  std::vector;

//
#define MKZEO(TEXT) TEXT[0] = _T('\0')

//
struct path_attrib {
	TCHAR path[MAX_PATH];
	DWORD shell_status;
};

struct dire_attrib {
	DWORD syncing;
	DWORD deleted;
	DWORD modified;
	DWORD added;
	DWORD conflict;
	DWORD dispstat;
};

//
namespace NCacheUtility {
	void ClearPathsVec(vector<struct path_attrib *> *paths_vec);
	struct path_attrib *PathAttrib(const TCHAR *path, DWORD statcode);
	//
	void ClearDireCache(unordered_map<wstring, struct dire_attrib *> *dires_cache);
	struct dire_attrib *DireAttrib();
	const TCHAR *wcsend(const TCHAR *str);
};

#endif	/* _CACHEUTILITY_H_ */