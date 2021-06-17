#pragma once

#include "common_macro.h"

#include "Checksum.h"
#include "StringUtility.h"
#include "ParseConfig.h"


struct ListData {
	char		szVersion[MIN_TEXT_LEN];
	DWORD		uiUID;
	char		szReqValid[VALID_LENGTH];
	char		szUserName[USERNAME_LENGTH];
	char		szPassword[PASSWORD_LENGTH];
	char		szLocalIdenti[IDENTI_LENGTH];
	TCHAR		szLocation[MAX_PATH];
	char		access_key[KEY_LENGTH];
	serv_address	tAuthAddress;
	serv_address	tServAddress;
	char		_szAuthsum_[MD5_TEXT_LENGTH];
	//
//	DWORD		dwLastAnchor;
	char		szSessionID[SESSION_LENGTH];
};
extern struct ListData tListData;

//
struct InteValue {
	FILETIME	last_write;
	DWORD		content_length;
	DWORD		content_type;
	char		session_id[SESSION_LENGTH];
	//
	// get // get sum // post // put // put sum // del
	DWORD		last_anchor;
	DWORD		list_type;
	TCHAR		file_name[MAX_PATH];
	DWORD		chksum_type;
	DWORD		cache_verify;
	DWORD		range_type;
	unsigned __int64		file_size;
	unsigned __int64		offset;
	DWORD		final_status;
	TCHAR		data_buffer[MAX_PATH];
	//
};
//
struct SeionCtrl {
	//
	HANDLE	file_handle;
	size_t	rwrite_tatol;
	//
};

struct SeionBuffer {
	// sdtp buffer info
	char		*head_buffer;
	size_t		hlen;
	char		*body_buffer;
	size_t		blen;
	char		srecv_buffer[RECV_BUFF_SIZE];
	off_t		srlength;
	// send buffer info
};

//
struct TRANSSession {
	struct ListData *ldata;
	struct SeionBuffer buffer;
	struct InteValue ivalue;
	struct SeionCtrl control;
};

//
struct ValInteValue {
	size_t		content_length;
	DWORD		content_type;
	//
};

struct ValidSeion {
	struct ListData *ldata;
	struct SeionBuffer buffer;
	struct ValInteValue ivalue;
};
extern struct ValidSeion objValidSeion;

//
struct ComInteValue {
	size_t		content_length;
	DWORD		content_type;
	//
};

struct ComandSeion {
	struct ListData *ldata;
	struct SeionBuffer buffer;
	struct ComInteValue ivalue;
};
extern struct ComandSeion objCMDSeion;

//
