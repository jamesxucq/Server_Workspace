#include "StdAfx.h"
#include "utility\base64.h"

#include "Checksum.h"
#include "clientcom.h"
#include "Encoding.h"


//base64 encode
void NEncoding::Base64FromBits(unsigned char *out, const unsigned char *in, int inlen) {
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

	if(!szCiphertext || !szPlaintext || !szSaltValue || *szCiphertext == _T('\0')) return NULL;

	nstriconv::unicode_ansi(cipher_buffer, szCiphertext);
	memset(plain_buffer, _T('\0'), MID_TEXT_LEN);
	base64_decode((unsigned char *)plain_buffer, cipher_buffer, MID_TEXT_LEN);
	nstriconv::ansi_unicode(szBuffer, plain_buffer);

	int salt_length = _tcslen(szSaltValue);
	_tcsncpy(szPlaintext, szBuffer+salt_length, _tcslen(szBuffer)-(salt_length*2));
	szPlaintext[_tcslen(szBuffer)-(salt_length*2)] = _T('\0');

	return szPlaintext;
}

TCHAR *NEncoding::EncodeBase64Text(TCHAR *szCiphertext, TCHAR *szPlaintext, TCHAR *szSaltValue) {
	TCHAR szBuffer[MID_TEXT_LEN];
	char plain_buffer[MID_TEXT_LEN];
	char cipher_buffer[MID_TEXT_LEN];

	if(!szCiphertext || !szPlaintext || !szSaltValue || *szPlaintext == _T('\0')) return NULL;

	_stprintf_s(szBuffer, _T("%s%s%s"), szSaltValue, szPlaintext, szSaltValue);
	nstriconv::unicode_ansi(plain_buffer, szBuffer);
	memset(cipher_buffer, '\0', MID_TEXT_LEN);
	if(!base64_encode(cipher_buffer, MID_TEXT_LEN, (const unsigned char *)plain_buffer, strlen(plain_buffer)))
		return NULL;
	nstriconv::ansi_unicode(szCiphertext, cipher_buffer);

	return szCiphertext;
}

////////////////////////////////////////////////////////////////////////////////

TCHAR *NEncoding::PWDCsum(TCHAR *szCsum, TCHAR *szPlaintext, TCHAR *szSaltValue) {
	TCHAR szBuffer[MID_TEXT_LEN];
	char plain_buffer[MID_TEXT_LEN];
	char sum[MID_TEXT_LEN];

	if(!szCsum || !szPlaintext || !szSaltValue || *szPlaintext == _T('\0')) return NULL;

	_stprintf_s(szBuffer, _T("%s%s%s"), szSaltValue, szPlaintext, szSaltValue);
	nstriconv::unicode_ansi(plain_buffer, szBuffer);
	get_md5sum_text(plain_buffer, strlen(plain_buffer), sum);
	nstriconv::ansi_unicode(szCsum, sum);

	return szCsum;
}

TCHAR *NEncoding::AuthCsum(TCHAR *szCsum, TCHAR *szUserName, TCHAR *szPassword) {
	TCHAR szBuffer[MID_TEXT_LEN];
	char plain_buffer[MID_TEXT_LEN];
	char sum[MID_TEXT_LEN];

	if(!szCsum || !szUserName || !szPassword || *szPassword==_T('\0') || *szUserName==_T('\0')) return NULL;

	_stprintf_s(szBuffer, _T("%s:%s"), szUserName, szPassword);
	nstriconv::unicode_ansi(plain_buffer, szBuffer);
	memset(sum, '\0', MD5_TEXT_LENGTH);
	get_md5sum_text(plain_buffer, strlen(plain_buffer), sum);
	nstriconv::ansi_unicode(szCsum, sum);

	return szCsum;
}

char *NEncoding::AuthCsum(char *szCsum, TCHAR *szUserName, TCHAR *szPassword) {
	TCHAR szBuffer[MID_TEXT_LEN];
	char plain_buffer[MID_TEXT_LEN];
	char sum[MID_TEXT_LEN];

	if(!szCsum || !szUserName || !szPassword || *szPassword==_T('\0') || *szUserName==_T('\0')) return NULL;

	_stprintf_s(szBuffer, _T("%s:%s"), szUserName, szPassword);
	nstriconv::unicode_ansi(plain_buffer, szBuffer);
	memset(sum, '\0', MD5_TEXT_LENGTH);
	get_md5sum_text(plain_buffer, strlen(plain_buffer), sum);
	strcpy(szCsum, sum);

	return szCsum;
}

char *NEncoding::AuthCsum(char *szCsum, char *szUserName, char *szPassword) {
	char plain_buffer[MID_TEXT_LEN];
	char sum[MID_TEXT_LEN];

	if(!szCsum || !szUserName || !szPassword || *szPassword=='\0' || *szUserName=='\0') return NULL;

	sprintf_s(plain_buffer, "%s:%s", szUserName, szPassword);
	memset(sum, '\0', MD5_TEXT_LENGTH);
	get_md5sum_text(plain_buffer, strlen(plain_buffer), sum);
	strcpy(szCsum, sum);

	return szCsum;
}


TCHAR *NEncoding::AuthCsum(TCHAR *szCsum, DWORD uiUID, TCHAR *szAccessKey) {
	TCHAR szBuffer[MID_TEXT_LEN];
	char plain_buffer[MID_TEXT_LEN];
	char sum[MID_TEXT_LEN];

	if(!szCsum || !szAccessKey || *szAccessKey == _T('\0')) return NULL;

	_stprintf_s(szBuffer, _T("%lu:%s"), uiUID, szAccessKey);
	nstriconv::unicode_ansi(plain_buffer, szBuffer);
	memset(sum, '\0', MD5_TEXT_LENGTH);
	get_md5sum_text(plain_buffer, strlen(plain_buffer), sum);
	nstriconv::ansi_unicode(szCsum, sum);

	return szCsum;
}

char *NEncoding::AuthCsum(char *szCsum, DWORD uiUID, char *szAccessKey) {
	char plain_buffer[MID_TEXT_LEN];
	char sum[MID_TEXT_LEN];

	if(!szCsum || !szAccessKey || *szAccessKey == '\0') return NULL;

	sprintf_s(plain_buffer, "%lu:%s", uiUID, szAccessKey);
	memset(sum, '\0', MD5_TEXT_LENGTH);
	get_md5sum_text(plain_buffer, strlen(plain_buffer), sum);
	strcpy_s(szCsum, MID_TEXT_LEN, sum);

	return szCsum;
}

/*
TCHAR *NEncoding::RandomSalt(TCHAR *szSaltValue) {
if(!szSaltValue) return NULL;

srand((unsigned int) time(NULL));
_stprintf_s(szSaltValue, MIN_TEXT_LEN,_T("%lu%lu"), rand(), time(NULL));

return szSaltValue;
}
*/

TCHAR *NEncoding::RandomSalt(TCHAR *szSaltValue) {
	TCHAR rand_str[MIN_TEXT_LEN];
	TCHAR code_array[64] = _T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");
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
	_tcscpy_s(szSaltValue, _tcslen(rand_str)+1, rand_str);

	return szSaltValue;
}