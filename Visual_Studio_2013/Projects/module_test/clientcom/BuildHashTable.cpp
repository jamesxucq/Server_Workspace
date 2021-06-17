#include "stdafx.h"

#include "common_macro.h"
#include "third_party.h"

#include "Checksum.h"
#include "FileCsums.h"
#include "BuildHashTable.h"

int BuildHashTable(hashtable *htable, HANDLE sums_stream, DWORD file_size) {
	DWORD dwReadSize;
	struct complex_sum *read_complex = NULL;

	if (!htable || INVALID_HANDLE_VALUE == sums_stream) 
		return -1;

	DWORD result = SetFilePointer(sums_stream, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		return -1;

	int complex_count = file_size / sizeof(struct complex_sum);
	for (int index = 0; index < complex_count; index++) {
		read_complex = (struct complex_sum *)malloc(sizeof(struct complex_sum));
		if(!read_complex) LOG_FATAL(_T("malloc error !!!"));
		if(!ReadFile(sums_stream, read_complex, sizeof(struct complex_sum), &dwReadSize, NULL)) {
			if(ERROR_HANDLE_EOF != GetLastError()) return -1;
		}
BinaryLogger::LogBinaryCode("********", 8); 
BinaryLogger::LogBinaryCode((char *)read_complex->md5_sum, 16);
		hash_insert_adler32(read_complex->md5_sum, read_complex, read_complex->adler32_sum, htable);
	}

	return 0;
}

int HashTableSearch(BYTE *DatPtr, hashtable *htable, BYTE mode) {// bFine :>=0 found <0  not found
	static unsigned int adler32_sum;
	static char c1;
	char c2;
	unsigned char md5_sum[DIGEST_LENGTH];
	struct complex_sum *found_sums;

	if (!mode) {
		adler32_sum = get_adler32_csum((char *) DatPtr, BLOCK_SIZE);
		LOG_DEBUG(_T("adler32_sum:%u"), adler32_sum);
		c1 = *((char *) DatPtr);
	} else {
		c2 = *((char *) DatPtr + BLOCK_SIZE);
		adler32_sum = get_adler32_rolling(adler32_sum, BLOCK_SIZE, c1, c2);
		c1 = *((char *) DatPtr);
	}
	struct hashentry *root = hash_value_adler32(adler32_sum, htable);
	if(!root) return -1;

	get_csum_md5((char *) DatPtr, BLOCK_SIZE, md5_sum);
	found_sums = (struct complex_sum *)hash_value_md5(md5_sum, root, htable);
	if(!found_sums) return -1;

	return found_sums->offset;
}

unsigned int NULL_Hash(void *src) {

	return 0;
}

int equal_md5(void *k1, void *k2) {
	return (0 == strncmp((char *) k1, (char *) k2, DIGEST_LENGTH));
}

unsigned int lh_strhash(void *src) {
	int i, l;
	unsigned long ret = 0;
	unsigned short *s;
	char *str = (char *) src;
	if (str == NULL)
		return (0);
	l = (strlen(str) + 1) / 2;
	s = (unsigned short *) str;

	for (i = 0; i < l; i++)
		ret ^= s[i] << (i & 0x0f);

	return (ret);
}

int equal_str(void *k1, void *k2) {
	return (0 == strcmp((char *) k1, (char *) k2));
}


int BuildHashTable(hashtable *htable, struct complex_sum *psums_complex, int block_count) {
	struct complex_sum *complex_token;

	if (!htable || !psums_complex) return -1;
	complex_token = psums_complex;

	for (int index = 0; index < block_count; index++) {
// BinaryLogger::LogBinaryCode("********", 8);
// BinaryLogger::LogBinaryCode(complex_token->md5_sum, 16);
		hash_insert_adler32(complex_token->md5_sum, complex_token, complex_token->adler32_sum, htable);
		complex_token++;
	}

	return 0;
}