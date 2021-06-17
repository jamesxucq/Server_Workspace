#ifndef _CHECKSUM_H
#define _CHECKSUM_H

#ifdef __cplusplus
extern "C" {
#endif

	unsigned int adler32_csum(char *buf, int len);
	unsigned int adler32_rolling_csum(unsigned int csum, int len, char c1, char c2);

#ifdef __cplusplus
}
#endif

#endif
