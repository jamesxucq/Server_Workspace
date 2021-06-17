#pragma once

class CFastActXml {
public:
	CFastActXml(void);
	~CFastActXml(void);
private:
	FilesVec *m_pFilesVec;
private:
	char *parse_buffe;
	char *parse_posi;
	unsigned int initi_parse;
public:
	DWORD Initialize(FilesVec *pFilesVec, unsigned int max_len);
	VOID Finalize();
public:
	DWORD ParseFilesActXml(const char *buffe, unsigned int length);
};

extern CFastActXml objFastActXml;