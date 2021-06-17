#pragma once

#include "tools_macro.h"


struct SessionBuffer
{
	//sdtp buff info
	char		*head_buffer;
	size_t		hlen;
	char		*body_buffer;
	size_t		blen;
	char		srecv_buffer[RECV_BUFF_SIZE];
	size_t		srlength;
	//send buff info
};

struct Session
{
	int	Method;
	//struct GroupData group;
	struct SessionBuffer buffer;
	//struct SessionData data;
	//struct SessionCtrl ctrl;
};

extern struct Session OSession;