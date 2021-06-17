#include "StdAfx.h"
#include "LiveVec.h"
// #include "StringUtility.h"


void NLiveVec::DeleLiveVec(LiveVec *pLiveVec)
{
	vector <struct LiveData *>::iterator iter;

	for(iter = pLiveVec->begin(); pLiveVec->end() != iter; ++iter) {
		free(*iter);
	}
	pLiveVec->clear();
}

struct LiveData *NLiveVec::AddNewLiveData(LiveVec *pLiveVec)
{
	struct LiveData *ldata;

	ldata = (struct LiveData *)malloc(sizeof(struct LiveData));
	if(!ldata) return NULL;
	memset(ldata, '\0', sizeof(struct LiveData));
	//
	pLiveVec->push_back(ldata);

	return ldata;
}

DWORD NLiveVec::MaxLivePid(LiveVec *pLiveVec) {
	vector <struct LiveData *>::iterator iter;
	DWORD max_pid = 0;

	for(iter = pLiveVec->begin(); pLiveVec->end() != iter; ++iter) {
		if(max_pid < (*iter)->pid) max_pid = (*iter)->pid;
	}

	return max_pid;
}

struct LiveData *NLiveVec::GetLivePid(LiveVec *pLiveVec, DWORD pid) {
	vector <struct LiveData *>::iterator iter;
	struct LiveData *ldata = NULL;

	for(iter = pLiveVec->begin(); pLiveVec->end() != iter; ++iter) {
		if(pid == (*iter)->pid) {
			ldata = (*iter);
			break;
		}
	}

	return ldata;
}

DWORD GetLiveCmd(char *szCommand) {
	if (!szCommand || '\0'==*szCommand) 
		return LIVE_INVALID_CMD;
	//
	else if (!strcmp("replace", szCommand)) 
		return LIVE_REPLACE_CMD;
	//
	else if (!strcmp("copy", szCommand)) 
		return LIVE_COPY_CMD;
	else if (!strcmp("delete", szCommand)) 
		return LIVE_DELETE_CMD;
	else if (!strcmp("execute", szCommand)) 
		return LIVE_EXECUTE_CMD;
	else if (!strcmp("unilib", szCommand)) 
		return LIVE_UNILIB_CMD;
	else if (!strcmp("inslib", szCommand)) 
		return LIVE_INSLIB_CMD;
	//
	else if (!strcmp("adddir", szCommand))
		return LIVE_ADDDIR_CMD;
	else if (!strcmp("copydir", szCommand))
		return LIVE_COPYDIR_CMD;
	else if (!strcmp("deldir", szCommand)) 
		return LIVE_DELDIR_CMD;
	else if (!strcmp("eradir", szCommand)) 
		return LIVE_ERADIR_CMD;
	//
	else if (!strcmp("addkey", szCommand)) 
		return LIVE_CREKEY_CMD;
	else if (!strcmp("setreg", szCommand)) 
		return LIVE_SETREG_CMD;
	else if (!strcmp("delkey", szCommand)) 
		return LIVE_DELKEY_CMD;
	else if (!strcmp("delreg", szCommand)) 
		return LIVE_DELREG_CMD;
	else if (!strcmp("enadriv", szCommand)) 
		return LIVE_ENDRIV_CMD;
	else if (!strcmp("disdriv", szCommand)) 
		return LIVE_DSDRIV_CMD;
	else if (!strcmp("kill", szCommand)) 
		return LIVE_KILL_CMD;
	else if (!strcmp("edelkey", szCommand)) 
		return LIVE_EDELKEY_CMD;
	//
	return LIVE_INVALID_CMD;
}