/* 
 * base64.h
 * Author: Administrator
 * Created on 2013年1月23日, 下午3:18
 */

#ifndef BASE64_H
#define	BASE64_H

#ifdef	__cplusplus
extern "C" {
#endif

/**
* Decode a base64-encoded string.
*
* @param out      buffer for decoded data
* @param in       null-terminated input string
* @param out_size size in bytes of the out buffer, must be at
*                 least 3/4 of the length of in
* @return         number of bytes written, or a negative value in case of
*                 invalid input
*/
int base64_decode(unsigned char *out, const char *in, int out_size);

/**
* Encode data to base64 and null-terminate.
*
* @param out      buffer for encoded data
* @param out_size size in bytes of the output buffer, must be at
*                 least BASE64_SIZE(in_size)
* @param in_size  size in bytes of the 'in' buffer
* @return         'out' or NULL in case of error
*/
char *base64_encode(char *out, int out_size, const unsigned char *in, int in_size);

/**
* Calculate the output size needed to base64-encode x bytes.
*/
#define BASE64_SIZE(x)  (((x)+2) / 3 * 4 + 1)

#ifdef	__cplusplus
}
#endif

#endif	/* BASE64_H */

