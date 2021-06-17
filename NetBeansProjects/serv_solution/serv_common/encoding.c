#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common_macro.h"
#include "checksum.h"
#include "utility/base64.h"
#include "encoding.h"

char *reply_auth_chks(char *chks_str, char *user_name, char *password) {
    char buffer[MID_TEXT_LEN];
    char chks[MD5_TEXT_LENGTH];
    //
    if (!password || !user_name || !chks_str) return NULL;
    sprintf(buffer, "%s:%s", user_name, password);
    get_md5sum_text(buffer, strlen(buffer), chks);
    //
    strcpy(chks_str, chks);
    return chks_str;
}

char *trans_auth_chks(char *chks_str, unsigned int uid, char *access_key) {
    char buffer[MID_TEXT_LEN];
    char chks[MD5_TEXT_LENGTH];
    //
    if (!access_key || !chks_str) return NULL;
    sprintf(buffer, "%u:%s", uid, access_key);
    get_md5sum_text(buffer, strlen(buffer), chks);
    //
    strcpy(chks_str, chks);
    return chks_str;
}

char *decode_base64_text(char *plain_txt, char *cipher_txt, char *salt_value) {
    char buffer[MID_TEXT_LEN];
    //
    if (!cipher_txt || !plain_txt || !salt_value || *cipher_txt == '\0') return NULL;
    MKZERO(buffer);
    int desiz = base64_decode((unsigned char *) buffer, cipher_txt, MID_TEXT_LEN);
    int salt_length = strlen(salt_value);
    strncpy(plain_txt, buffer + salt_length, desiz - (salt_length << 1));
    plain_txt[desiz - (salt_length << 1)] = '\0';
    //
    return plain_txt;
}

char *encode_base64_text(char *cipher_txt, char *plain_txt, char *salt_value) {
    char buffer[MID_TEXT_LEN];
    //
    if (!cipher_txt || !plain_txt || !salt_value || *plain_txt == '\0') return NULL;
    sprintf(buffer, "%s%s%s", salt_value, plain_txt, salt_value);
    if (!base64_encode(cipher_txt, MID_TEXT_LEN, (const unsigned char *) buffer, strlen(buffer)))
        return NULL;
    //
    return cipher_txt;
}

char *random_salt(char *salt_value) {
    char rand_str[MIN_TEXT_LEN];
    char code_array[62] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    unsigned int last_seed;
    //
    if (!salt_value) return NULL;
    last_seed = time(NULL);
    int tatol;
    for (tatol = 0; tatol < 16; tatol++) {
        srand((unsigned int) last_seed);
        last_seed = rand();
        rand_str[tatol] = code_array[last_seed % 62];
    }
    rand_str[tatol] = '\0';
    strcpy(salt_value, rand_str);
    //
    return salt_value;
}