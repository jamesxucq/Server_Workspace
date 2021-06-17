#pragma once


namespace NEncoding
{
	////////////////////////////////////////////////////////////////////////////////
	void Base64FromBits(unsigned char *out, const unsigned char *in, int inlen);	

	////////////////////////////////////////////////////////////////////////////////
	TCHAR *DecodeBase64Text(TCHAR *szPlaintext, TCHAR *szCiphertext, TCHAR *szSaltValue);
	TCHAR *EncodeBase64Text(TCHAR *szCiphertext, TCHAR *szPlaintext, TCHAR *szSaltValue);

	////////////////////////////////////////////////////////////////////////////////
	TCHAR *PWDCsum(TCHAR *szCsum, TCHAR *szPlaintext, TCHAR *szSaltValue);

	////////////////////////////////////////////////////////////////////////////////
	TCHAR *AuthCsum(TCHAR *szCsum, TCHAR *szUserName, TCHAR *szPassword);
	char *AuthCsum(char *szCsum, TCHAR *szUserName, TCHAR *szPassword);
	char *AuthCsum(char *szCsum, char *szUserName, char *szPassword);
	TCHAR *AuthCsum(TCHAR *szCsum, DWORD uiUID, TCHAR *szAccessKey);
	char *AuthCsum(char *szCsum, DWORD uiUID, char *szAccessKey);

	////////////////////////////////////////////////////////////////////////////////
	TCHAR *RandomSalt(TCHAR *szSaltValue);
};
