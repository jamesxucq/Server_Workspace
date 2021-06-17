#pragma once

class CSlowAttXml
{
public:
    CSlowAttXml(void);
    ~CSlowAttXml(void);
private:
	HANDLE m_hFilesAtt;
private:
	char *parse_buffe;
	char *parse_posi;
	unsigned int initi_parse;
public:
    DWORD Initialize(HANDLE hFilesAtt, unsigned int max_len);
    VOID Finalize();
public:
    DWORD ParseFilesAttXml(const char *buffe, unsigned int length);
};

extern CSlowAttXml objSlowAttXml;