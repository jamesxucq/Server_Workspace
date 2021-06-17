#include "StdAfx.h"
#include "Encoding.h"


//base64 encode
void NEncoding::Base64FromBits(unsigned char *out, const unsigned char *in, int inlen)
{
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
