#include "common_macro.h"
#include "checksum.h"
#include "hash_table_ext.h"

int build_hash_table(hashtable *htable, struct complex_chks *pcchks, int block_count) {
    struct complex_chks *complex_toke;
    //
    if (!htable || !pcchks) return -1;
    complex_toke = pcchks;
    int inde;
    for (inde = 0; inde < block_count; inde++) {
        hash_insert_adler32(complex_toke->md5_chks, complex_toke, complex_toke->adler32_chk, htable);
        complex_toke++;
    }
    return 0;
}

int hash_search(hashtable *htable, BYTE *byte_ptr, seaenv *senv) { //bFine :>=0 found <0  not found
    unsigned char c2;
    unsigned char md5_chks[MD5_DIGEST_LEN];
    struct complex_chks *found_chks;
    //
    if (senv->mode) {
        c2 = *(byte_ptr + BLOCK_SIZE - 1);
        senv->adler32_chk = get_adler32_rolling(senv->adler32_chk, BLOCK_SIZE, senv->c1, c2);
        senv->c1 = *byte_ptr;
    } else {
        unsigned int adler32_chk = get_adler32_chks(0, NULL, 0);
        senv->adler32_chk = get_adler32_chks(adler32_chk, byte_ptr, BLOCK_SIZE);
        senv->c1 = *byte_ptr;
    }
    struct tab_nod *root = hash_value_adler32(senv->adler32_chk, htable);
    if (!root) return -1;
    //
    get_chks_md5(byte_ptr, BLOCK_SIZE, md5_chks);
    found_chks = (struct complex_chks *) hash_value_md5(md5_chks, root, htable);
    if (!found_chks) return -1;
    //
    return found_chks->offset;
}

int hash_search_leng(hashtable *htable, BYTE *byte_ptr, seaenv *senv, int selen) { //bFine :>=0 found <0  not found
    unsigned char c2;
    unsigned char md5_chks[MD5_DIGEST_LEN];
    struct complex_chks *found_chks;
    //
    if (senv->mode) {
        c2 = *(byte_ptr + selen - 1);
        senv->adler32_chk = get_adler32_rolling(senv->adler32_chk, selen, senv->c1, c2);
        senv->c1 = *byte_ptr;
    } else {
        unsigned int adler32_chk = get_adler32_chks(0, NULL, 0);
        senv->adler32_chk = get_adler32_chks(adler32_chk, byte_ptr, selen);
        senv->c1 = *byte_ptr;
    }
    struct tab_nod *root = hash_value_adler32(senv->adler32_chk, htable);
    if (!root) return -1;
    //
    get_chks_md5(byte_ptr, selen, md5_chks);
    found_chks = (struct complex_chks *) hash_value_md5(md5_chks, root, htable);
    if (!found_chks) return -1;
    //
    return found_chks->offset;
}

unsigned int none_hash(void *src) {
    return 0;
}

int equal_md5(void *k1, void *k2) {
    return (0 == strncmp((char *) k1, (char *) k2, MD5_DIGEST_LEN));
}

unsigned int lh_strhash(void *src) {
    int i, l;
    unsigned long ret = 0;
    unsigned short *s;
    char *str = (char *) src;
    if (NULL == str) return (0);
    l = (strlen(str) + 1) / 2;
    s = (unsigned short *) str;

    for (i = 0; i < l; i++)
        ret ^= s[i] << (i & 0x0f);
    //
    return (ret);
}

int equal_str(void *k1, void *k2) {
    return (0 == strcmp((char *) k1, (char *) k2));
}
