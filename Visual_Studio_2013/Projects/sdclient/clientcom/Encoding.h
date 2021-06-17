#pragma once

namespace NEncoding {
	//
	void Base64FromBits(unsigned char *out, const unsigned char *in, int inlen);	
	//
	TCHAR *DecodeBase64Text(TCHAR *szPlaintext, TCHAR *szCiphertext, TCHAR *szSaltValue);
	TCHAR *EncodeBase64Text(TCHAR *szCiphertext, TCHAR *szPlaintext, TCHAR *szSaltValue);
	//
	TCHAR *PWDChks(TCHAR *szChksText, TCHAR *szPlaintext, TCHAR *szSaltValue);
	//
	TCHAR *AuthChks(TCHAR *szChksText, TCHAR *szUserName, TCHAR *szPassword);
	char *AuthChks(char *szChksText, TCHAR *szUserName, TCHAR *szPassword);
	char *AuthChks(char *szChksText, char *szUserName, char *szPassword);
	TCHAR *AuthChks(TCHAR *szChksText, DWORD uiUID, TCHAR *szAccessKey);
	char *AuthChks(char *szChksText, DWORD uiUID, char *szAccessKey);
	//
	TCHAR *RandomSalt(TCHAR *szSaltValue);
};
