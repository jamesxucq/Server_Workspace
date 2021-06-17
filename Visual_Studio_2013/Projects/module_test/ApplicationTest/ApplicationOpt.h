//ApplicationOpt.h
#ifndef _APPLICATION_OPT_H_
#define _APPLICATION_OPT_H_

	bool ExecuteApplication(char *sApplicationPath);
	bool EndApplication(char *sProcessName);
	bool RefreshExplorer(LPCSTR sProcessName);
	bool ExecuteApplicationExt(char *sApplicationPath, char *sProcessName, bool fFailIfExists);

	static bool CheckApplicationAutorun(char *sApplicationPath);
	bool AddApplicationAutorun(char *sApplicationPath);
	bool AddApplicationAutorunExt(char *sApplicationPath);
	bool DelApplicationAutorun(char *sApplicationPath);

#endif /* _APPLICATION_OPT_H_ */