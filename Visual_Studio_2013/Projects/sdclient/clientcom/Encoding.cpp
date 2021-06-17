#include "StdAfx.h"
#include "utility\base64.h"

#include "StringUtility.h"
#include "Checksum.h"
#include "Encoding.h"


// base64 encode
VOID NEncoding::Base64FromBits(unsigned char *out, const unsigned char *in, int inlen) {
	const char base64digits[] ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	for (; inlen >= 3; inlen -= 3) {
		*out++ = base64digits[in[0] >> 2];
		*out++ = base64digits[((in[0] << 4) & 0x30)|(in[1] >> 4)];
		*out++ = base64digits[((in[1] << 2) & 0x3c)|(in[2] >> 6)];
		*out++ = base64digits[in[2] & 0x3f];
		in += 3;
	}

	if (inlen > 0) {
		unsigned char fragment;
		*out++ = base64digits[in[0] >> 2];
		fragment = (in[0] << 4) & 0x30;
		if (inlen > 1)
			fragment |= in[1] >> 4;
		*out++ = base64digits[fragment];
		*out++ = (inlen < 2) ? '=': base64digits[(in[1] << 2) & 0x3c];
		*out++ = '=';
	}
	*out = NULL;
}

TCHAR *NEncoding::DecodeBase64Text(TCHAR *szPlaintext, TCHAR *szCiphertext, TCHAR *szSaltValue) {
	TCHAR szBuffer[MID_TEXT_LEN];
	char plain_buffer[MID_TEXT_LEN];
	char cipher_buffer[MID_TEXT_LEN];

	if(!szCiphertext || !szPlaintext || !szSaltValue || _T('\0')==*szCiphertext) return NULL;
	strcon::ustr_ansi(cipher_buffer, szCiphertext);
	memset(plain_buffer, '\0', MID_TEXT_LEN);
	base64_decode((unsigned char *)plain_buffer, cipher_buffer, MID_TEXT_LEN);
	strcon::ansi_ustr(szBuffer, plain_buffer);
	int salt_length = (int)_tcslen(szSaltValue);
	_tcsncpy_s(szPlaintext, MID_TEXT_LEN, szBuffer+salt_length, _tcslen(szBuffer)-(salt_length<<1));
	szPlaintext[_tcslen(szBuffer)-(salt_length<<1)] = _T('\0');

	return szPlaintext;
}

TCHAR *NEncoding::EncodeBase64Text(TCHAR *szCiphertext, TCHAR *szPlaintext, TCHAR *szSaltValue) {
	TCHAR szBuffer[MID_TEXT_LEN];
	char plain_buffer[MID_TEXT_LEN];
	char cipher_buffer[MID_TEXT_LEN];

	if(!szCiphertext || !szPlaintext || !szSaltValue || _T('\0')==*szPlaintext) return NULL;
	_stprintf_s(szBuffer, _T("%s%s%s"), szSaltValue, szPlaintext, szSaltValue);
	strcon::ustr_ansi(plain_buffer, szBuffer);
	mkzeo(cipher_buffer);
	if(!base64_encode(cipher_buffer, MID_TEXT_LEN, (const unsigned char *)plain_buffer, (int)strlen(plain_buffer)))
		return NULL;
	strcon::ansi_ustr(szCiphertext, cipher_buffer);

	return szCiphertext;
}

//
TCHAR *NEncoding::PWDChks(TCHAR *szChksText, TCHAR *szPlaintext, TCHAR *szSaltValue) {
	TCHAR szBuffer[MID_TEXT_LEN];
	char plain_buffer[MID_TEXT_LEN];
	char chksum_str[MD5_TEXT_LENGTH];

	if(!szChksText || !szPlaintext || !szSaltValue || _T('\0')==*szPlaintext) return NULL;
	_stprintf_s(szBuffer, _T("%s%s%s"), szSaltValue, szPlaintext, szSaltValue);
	strcon::ustr_ansi(plain_buffer, szBuffer);
	mkzeo(chksum_str);
	chksm::get_md5sum_text(chksum_str, plain_buffer, (unsigned int)strlen(plain_buffer));
	strcon::ansi_ustr(szChksText, chksum_str);

	return szChksText;
}

TCHAR *NEncoding::AuthChks(TCHAR *szChksText, TCHAR *szUserName, TCHAR *szPassword) {
	TCHAR szBuffer[MID_TEXT_LEN];
	char plain_buffer[MID_TEXT_LEN];
	char chksum_str[MD5_TEXT_LENGTH];

	if(!szChksText || !szUserName || !szPassword || _T('\0')==*szPassword || _T('\0')==*szUserName) return NULL;
	_stprintf_s(szBuffer, _T("%s:%s"), szUserName, szPassword);
	strcon::ustr_ansi(plain_buffer, szBuffer);
	mkzeo(chksum_str);
	chksm::get_md5sum_text(chksum_str, plain_buffer, (unsigned int)strlen(plain_buffer));
	strcon::ansi_ustr(szChksText, chksum_str);

	return szChksText;
}

char *NEncoding::AuthChks(char *szChksText, TCHAR *szUserName, TCHAR *szPassword) {
	TCHAR szBuffer[MID_TEXT_LEN];
	char plain_buffer[MID_TEXT_LEN];
	char chksum_str[MD5_TEXT_LENGTH];

	if(!szChksText || !szUserName || !szPassword || _T('\0')==*szPassword || _T('\0')==*szUserName) return NULL;
	_stprintf_s(szBuffer, _T("%s:%s"), szUserName, szPassword);
	strcon::ustr_ansi(plain_buffer, szBuffer);
	mkzeo(chksum_str);
	chksm::get_md5sum_text(chksum_str, plain_buffer, (unsigned int)strlen(plain_buffer));
	strcpy_s(szChksText, MD5_TEXT_LENGTH, chksum_str);

	return szChksText;
}

char *NEncoding::AuthChks(char *szChksText, char *szUserName, char *szPassword) {
	char plain_buffer[MID_TEXT_LEN];
	char chksum_str[MD5_TEXT_LENGTH];

 	if(!szChksText || !szUserName || !szPassword || '\0'==*szPassword || '\0'==*szUserName) return NULL;
	sprintf_s(plain_buffer, "%s:%s", szUserName, szPassword);
	mkzeo(chksum_str);
	chksm::get_md5sum_text(chksum_str, plain_buffer, (unsigned int)strlen(plain_buffer));
	strcpy_s(szChksText, MD5_TEXT_LENGTH, chksum_str);

	return szChksText;
}


TCHAR *NEncoding::AuthChks(TCHAR *szChksText, DWORD uiUID, TCHAR *szAccessKey) {
	TCHAR szBuffer[MID_TEXT_LEN];
	char plain_buffer[MID_TEXT_LEN];
	char chksum_str[MD5_TEXT_LENGTH];

	if(!szChksText || !szAccessKey || _T('\0')==*szAccessKey) return NULL;
	_stprintf_s(szBuffer, _T("%lu:%s"), uiUID, szAccessKey);
	strcon::ustr_ansi(plain_buffer, szBuffer);
	mkzeo(chksum_str);
	chksm::get_md5sum_text(chksum_str, plain_buffer, (unsigned int)strlen(plain_buffer));
	strcon::ansi_ustr(szChksText, chksum_str);

	return szChksText;
}

char *NEncoding::AuthChks(char *szChksText, DWORD uiUID, char *szAccessKey) {
	char plain_buffer[MID_TEXT_LEN];
	char chksum_str[MD5_TEXT_LENGTH];

	if(!szChksText || !szAccessKey || '\0'==*szAccessKey) return NULL;
	sprintf_s(plain_buffer, "%lu:%s", uiUID, szAccessKey);
	mkzeo(chksum_str);
	chksm::get_md5sum_text(chksum_str, plain_buffer, (unsigned int)strlen(plain_buffer));
	strcpy_s(szChksText, MD5_TEXT_LENGTH, chksum_str);

	return szChksText;
}

/*
TCHAR *NEncoding::RandomSalt(TCHAR *szSaltValue) {
if(!szSaltValue) return NULL;

srand((unsigned int) time(NULL));
_stprintf_s(szSaltValue, SALT_TEXT_LEN,_T("%lu%lu"), rand(), time(NULL));

return szSaltValue;
}
*/

TCHAR *NEncoding::RandomSalt(TCHAR *szSaltValue) {
	TCHAR rand_str[SALT_TEXT_LEN];
	TCHAR code_array[63] = _T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");
	DWORD last_seed;

	if(!szSaltValue) return NULL;
	last_seed = GetTickCount();
	int count;
	for(count=0; count<16; count++) {
		srand((unsigned int) last_seed);
		last_seed = rand();
		rand_str[count] = code_array[last_seed%62]; 
	}
	rand_str[count] = '\0';
	_tcscpy_s(szSaltValue, SALT_TEXT_LEN, rand_str);

	return szSaltValue;
}