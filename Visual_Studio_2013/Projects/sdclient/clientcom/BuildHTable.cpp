#include "stdafx.h"

#include "common_macro.h"
#include "third_party.h"

#include "Checksum.h"
#include "FileChks.h"
#include "BuildHTable.h"

TCHAR *get_md5sum_textT(unsigned char *md5sum) {
    static TCHAR chksum_str[33];
    for (int inde=0; inde<16; inde++)
        _stprintf_s(chksum_str+inde*2, 33, _T("%02x"), md5sum[inde]);
    chksum_str[32] = _T('\0');
    return chksum_str;
}

int NBdHTab::BuildHTable(hash_table *htab, HANDLE sums_stre, DWORD file_size, int file_chunk) {
	DWORD dwReadSize;
	struct complex_chks *read_complex = NULL;
//
	if (!htab || INVALID_HANDLE_VALUE == sums_stre) 
		return -1;
//
	DWORD dwResult = SetFilePointer(sums_stre, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) 
		return -1;
//
// _LOG_DEBUG(_T("------ GCHKS ------ file_chunk:%d"), file_chunk);
// DWORD inde = 0x00; // for test
	int compl_tatol = file_size / sizeof(struct complex_chks);
	for (int inde = 0; inde < compl_tatol; inde++) {
		read_complex = (struct complex_chks *)malloc(sizeof(struct complex_chks));
		if(!read_complex) { 
			_LOG_FATAL(_T("read_complex malloc error !")); 
			return -1;
		}
		if(!ReadFile(sums_stre, read_complex, sizeof(struct complex_chks), &dwReadSize, NULL) || !dwReadSize) {
			// if(ERROR_HANDLE_EOF != GetLastError()) 
			free(read_complex);
			return -1;
		}
// BinaryLogger::LogBinaryCode("********", 8); 
// BinaryLogger::LogBinaryCode((char *)read_complex->md5_chks, 16);
// _LOG_DEBUG(_T("inde:%d offset:%u adler32:%u md5:%s"), inde, read_complex->offset, read_complex->adler32_chk, get_md5sum_textT(read_complex->md5_chks));
		hash_insert_adler32(read_complex->md5_chks, read_complex, read_complex->adler32_chk, htab);
	}
//
	return 0;
}

int NBdHTab::HashSearch(hash_table *htab, BYTE *byte_ptr, sea_env *senv) { // bFine :>=0 found <0  not found
	unsigned char c2;
	unsigned char md5_chks[MD5_DIGEST_LEN];
	struct complex_chks *found_sums;
//
// _LOG_DEBUG(_T("---------------- mode:%u"), mode);
	if (senv->mode) {
		c2 = *(byte_ptr + BLOCK_SIZE - 1);
		senv->adler32_chk = get_adler32_rolling(senv->adler32_chk, BLOCK_SIZE, senv->c1, c2);
		senv->c1 = *byte_ptr;
	} else {
		unsigned int adler32_chk = get_adler32_chksum(0, NULL, 0);
		senv->adler32_chk = get_adler32_chksum(adler32_chk, byte_ptr, BLOCK_SIZE);
// _LOG_DEBUG(_T("initial adler32_chk:%u"), adler32_chk); // disable by james 20130408
		senv->c1 = *byte_ptr;
	}
// _LOG_DEBUG(_T("---------------- adler32_chk:%u"), senv->adler32_chk);
	struct hash_entr *root = hash_value_adler32(senv->adler32_chk, htab);
	if(!root) return -1;
// _LOG_DEBUG(_T("---------------- adler32_chk:%u"), senv->adler32_chk);
//
	chksm::get_chksum_md5(byte_ptr, BLOCK_SIZE, md5_chks);
// _LOG_DEBUG(_T("---------------- md5_chks:%s"), get_md5sum_textT(md5_chks));
	found_sums = (struct complex_chks *)hash_value_md5(md5_chks, root, htab);
	if(!found_sums) return -1;
//
// _LOG_DEBUG(_T("found chks, offset:%d  md5_chks:%s"), found_sums->offset, get_md5sum_textT(md5_chks)); // disable by james 20130408
	return found_sums->offset;
}

int NBdHTab::HashSearch(hash_table *htab, BYTE *byte_ptr, sea_env *senv, int selen) { // bFine :>=0 found <0  not found
	unsigned char c2;
	unsigned char md5_chks[MD5_DIGEST_LEN];
	struct complex_chks *found_sums;
//
// _LOG_DEBUG(_T("---------------- mode:%u"), mode);
	if(senv->mode) {
		c2 = *(byte_ptr + selen - 1);
		senv->adler32_chk = get_adler32_rolling(senv->adler32_chk, selen, senv->c1, c2);
		senv->c1 = *byte_ptr;
	} else {
		unsigned int adler32_chk = get_adler32_chksum(0, NULL, 0);
		senv->adler32_chk = get_adler32_chksum(adler32_chk, byte_ptr, selen);
// _LOG_DEBUG(_T("initial adler32_chk:%u"), adler32_chk); // disable by james 20130408
		senv->c1 = *byte_ptr;
	}
// _LOG_DEBUG(_T("initial adler32_chk:%u"), adler32_chk); // disable by james 20130408
// _LOG_DEBUG(_T("---------------- adler32_chk:%u"), senv->adler32_chk);
	struct hash_entr *root = hash_value_adler32(senv->adler32_chk, htab);
	if(!root) return -1;
// _LOG_DEBUG(_T("---------------- adler32_chk:%u"), senv->adler32_chk);
//
	chksm::get_chksum_md5(byte_ptr, selen, md5_chks);
// _LOG_DEBUG(_T("---------------- md5_chks:%s"), get_md5sum_textT(md5_chks));
	found_sums = (struct complex_chks *)hash_value_md5(md5_chks, root, htab);
	if(!found_sums) return -1;
//
// _LOG_DEBUG(_T("found chks, offset:%d  md5_chks:%s"), found_sums->offset, get_md5sum_textT(md5_chks)); // disable by james 20130408
	return found_sums->offset;
}
unsigned int NULL_Hash(void *dat) {
	return 0;
}

int equal_md5(void *k1, void *k2) {
	return (0 == memcmp((char *) k1, (char *) k2, MD5_DIGEST_LEN));
}

unsigned int lh_strhash(void *dat) {
	int inde, l;
	unsigned long ret = 0;
	unsigned short *s;
	char *str = (char *) dat;
	if (NULL == str)
		return (0);
	l = (strlen(str) + 1) / 2;
	s = (unsigned short *) str;

	for (inde = 0; inde < l; inde++)
		ret ^= s[inde] << (inde & 0x0f);

	return (ret);
}

int equal_str(void *k1, void *k2) {
	return (0 == strcmp((char *) k1, (char *) k2));
}


int NBdHTab::BuildHTable(hash_table *htab, struct complex_chks *pcchks, int block_count) {
	struct complex_chks *complex_tok;
//
	if (!htab || !pcchks) return -1;
	complex_tok = pcchks;
//
	for (int inde = 0; inde < block_count; inde++) {
// BinaryLogger::LogBinaryCode("********", 8);
// BinaryLogger::LogBinaryCode(complex_tok->md5_chks, 16);
		hash_insert_adler32(complex_tok->md5_chks, complex_tok, complex_tok->adler32_chk, htab);
		complex_tok++;
	}
//
	return 0;
}


static void dele_bnod(void *key, void *data) {
	free(data);
}

void NBdHTab::DeleBHTab(hash_table *htab) {
	hash_destroy(htab, dele_bnod);
}