/* 
 * File:   encoding.h
 * Author: David
 *
 * Created on 2011
 */

#ifndef ENCODING_H
#define	ENCODING_H

#ifdef	__cplusplus
extern "C" {
#endif

    char *reply_auth_chks(char *chks_str, char *user_name, char *password);
    char *trans_auth_chks(char *chks_str, unsigned int uid, char *access_key);

    char *encode_base64_text(char *cipher_txt, char *plain_txt, char *salt_value);
    char *decode_base64_text(char *plain_txt, char *cipher_txt, char *salt_value);
    char *random_salt(char *salt_value);

#ifdef	__cplusplus
}
#endif

#endif	/* ENCODING_H */

