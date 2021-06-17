#pragma once

#include "common_macro.h"

#include "Checksum.h"
#include "StringUtility.h"
#include "ParseConfig.h"
#include "FilesVec.h"


struct ListInform {
	char		szVersion[MIN_TEXT_LEN];
	DWORD		uiUID;
	char		szUserName[USERNAME_LENGTH];
	char		szPassword[PASSWORD_LENGTH];
	char		szLocalIdenti[IDENTI_LENGTH];
	TCHAR		szLocation[MAX_PATH];
	char		access_key[KEY_LENGTH];
	serv_address AuthAddress;
	serv_address	ServAddress;
	char		_sAuthsum_[MD5_TEXT_LENGTH];
	//////////////////////////////////////////////
	DWORD		dwLastAnchor;
	char		szSessionID[SESSION_LENGTH];
};
extern struct ListInform tListInform;

/////////////////////////////////////////////////////////////////
struct InternalValue
{
	FILETIME	last_write;
	off_t		content_length;
	DWORD		content_type;
	char		session_id[SESSION_LENGTH];
	///////////////////////////////////////
	//get//get sum//post//put//put sum//del
	DWORD		last_anchor;
	DWORD		list_type;
	TCHAR		file_name[MAX_PATH];
	DWORD		csum_type;
	DWORD		range_type;
	off_t		file_length;
	off_t		offset;
	DWORD		final_status;
	TCHAR		data_buffer[MAX_PATH];
	///////////////////////////////////////
};


#define FILE_CACHE		0x00000001
#define MEMORY_CACHE	0x00000003

struct SessionCtrl
{
	//    int sdtp_type;
	int control_stype; //
	//////////////////////////
	HANDLE	file_stream;
	off_t	rwrite_count;
	//////////////////////////
};

struct SessionBuffer
{
	//sdtp buffer info
	char		*head_buffer;
	size_t		hlen;
	char		*body_buffer;
	size_t		blen;
	char		srecv_buffer[RECV_BUFF_SIZE];
	size_t		srlength;
	//send buffer info
};

/////////////////////////////////////////////////////////////////
struct TRANSSession
{
	//DWORD	sdtp_method;
	struct ListInform *list_inform;
	struct SessionBuffer buffer;
	struct InternalValue inte_value;
	struct SessionCtrl control;
};

/////////////////////////////////////////////////////////////////
struct ValInteValue
{
	off_t		content_length;
	DWORD		content_type;
	///////////////////////////////////////
};

struct ValidateSession
{
	struct ListInform *list_inform;
	struct SessionBuffer buffer;
	struct ValInteValue inte_value;
};
extern struct ValidateSession objValidateSession;

/////////////////////////////////////////////////////////////////
struct ComInteValue
{
	off_t		content_length;
	DWORD		content_type;
	///////////////////////////////////////
};

struct CommandSession
{
	struct ListInform *list_inform;
	struct SessionBuffer buffer;
	struct ComInteValue inte_value;
};
extern struct CommandSession objCommandSession;

/////////////////////////////////////////////////////////////////////////////////////
