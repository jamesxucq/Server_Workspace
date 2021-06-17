//ApplicationOpt.h
#ifndef _APPLICATION_OPT_H_
#define _APPLICATION_OPT_H_

	bool ExecuteApplication(char *sApplicationPath);
	bool EndApplication(char *sProcessName);
	bool RefreshExplorer(LPCSTR sProcessName);
	bool ExecuteApplicationEx(char *sApplicationPath, char *sProcessName, bool fFailIfExists);

#endif /* _APPLICATION_OPT_H_ */