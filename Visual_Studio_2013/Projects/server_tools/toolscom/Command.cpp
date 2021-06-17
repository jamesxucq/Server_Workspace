#include "StdAfx.h"
//#include "Session.h"
#include "ReplySocks.h"
#include "StringUtility.h"
#include "Command.h"

#include "md5.h"
#include "Logger.h"

CCommand::CCommand(void):
m_pNetworkConf(NULL),
m_pSession(NULL)
{
}

CCommand::~CCommand(void)
{
}

CCommand OCommand;


bool CCommand::Create(struct NetworkConf *pNetworkConf, struct Session *pSession)
{
	if(!pNetworkConf || !pSession) return false;
//
	m_pNetworkConf = pNetworkConf;
	m_pSession = pSession;
//
	return true;
}

int CCommand::SetReplySocks(char *szDestnServ, int dwDestnPort)
{
	m_ReplySockets.Create(szDestnServ, dwDestnPort, m_pNetworkConf);
	return TRUE;

}

int CCommand::ReplySendReceive(struct SessionBuffer *psbuffer)
{
	psbuffer->srlength = m_ReplySockets.SendReceive(psbuffer->srecv_buffer, psbuffer->srecv_buffer);
	if(0 < psbuffer->srlength) {
		psbuffer->hlen = NSDTProtocol::GetHeaderValue(&psbuffer->head_buffer, psbuffer->srecv_buffer);
		psbuffer->blen = NSDTProtocol::GetBodyValue(&psbuffer->body_buffer, psbuffer->srecv_buffer, psbuffer->srlength);
	}
//
	return psbuffer->srlength;
}

#define MID_TEXT_LEN 128
#define MD5_DIGEST_LEN 16
#define MD5_TEXT_LENGTH 33
void get_md5sum_text(char *buf, int len, char *sum)
{
	MD5_CTX md;
//
	MD5Init(&md);
	MD5Update(&md, (unsigned char *)buf, len);
	MD5Final(&md);
//
	int inde;
	for (inde=0; inde<MD5_DIGEST_LEN; inde++)
		sprintf_s (sum+inde*2, MD5_TEXT_LENGTH, "%02x", md.digest[inde]);
	sum[32] = '\0';
}

char *AuthChecksum(char *szChecksum, wchar_t *szUserName, wchar_t *szPassword)
{
	wchar_t szBuffer[MID_TEXT_LEN];
	char plain_buffer[MID_TEXT_LEN];
	char sum[MID_TEXT_LEN];
//
	if(!szChecksum || !szUserName || !szPassword || *szPassword==_T('\0') || *szUserName==_T('\0')) return NULL;
//
	_stprintf_s(szBuffer, _T("%s:%s"), szUserName, szPassword);
	striconv::unicode_ansi(plain_buffer, szBuffer);
	memset(sum, '\0', MD5_TEXT_LENGTH);
	get_md5sum_text(plain_buffer, strlen(plain_buffer), sum);
	strcpy_s(szChecksum, strlen(sum)+1, sum);
//
	return szChecksum;
}


//////////////////////////////////////////////////////////////
#define INITIAL_SESSION_BUFFER(BUFFER)  (BUFFER).head_buffer = (BUFFER).srecv_buffer; \
	(BUFFER).body_buffer = NULL; //modify bu james 20100722

DWORD CCommand::OptionAdd(DWORD dwAddType, char *szAddress, struct CacheParam *pCacheParam)
{
	DWORD dwAddStatus;
	char szChecksum[MID_TEXT_LEN];
//
	if(!pCacheParam) return -1;
	AuthChecksum(szChecksum, pCacheParam->szAdmin, pCacheParam->szPassword);
	//
	INITIAL_SESSION_BUFFER(m_pSession->buffer)
	NSDTProtocol::BuildAddSend(&m_pSession->buffer, dwAddType, szAddress, szChecksum);
	SetReplySocks(pCacheParam->Address.sin_addr, pCacheParam->Address.sin_port);
	if (0 > ReplySendReceive(&m_pSession->buffer)) return -1;
	if(!NSDTProtocol::ParseAddRecv(&dwAddStatus, m_pSession->buffer.head_buffer))
		return -1;
//
	return dwAddStatus;
}

DWORD CCommand::OptionClear(DWORD dwClearType, char *szAddress, struct CacheParam *pCacheParam)
{
	DWORD dwClearStatus;
	char szChecksum[MID_TEXT_LEN];
//
	if(!pCacheParam) return -1;
	AuthChecksum(szChecksum, pCacheParam->szAdmin, pCacheParam->szPassword);	
		//
	INITIAL_SESSION_BUFFER(m_pSession->buffer)
	NSDTProtocol::BuildClearSend(&m_pSession->buffer, dwClearType, szAddress, szChecksum);
	SetReplySocks(pCacheParam->Address.sin_addr, pCacheParam->Address.sin_port);
	if (0 > ReplySendReceive(&m_pSession->buffer)) return -1;
	if(!NSDTProtocol::ParseClearRecv(&dwClearStatus, m_pSession->buffer.head_buffer))
		return -1;
//
	return dwClearStatus;
}

DWORD CCommand::OptionList(char *szListXml, DWORD dwListType, struct CacheParam *pCacheParam)
{
	DWORD dwListStatus;
	char szChecksum[MID_TEXT_LEN];
	DWORD dwContentType, dwContentLength;
//
	if(!pCacheParam) return -1;
	AuthChecksum(szChecksum, pCacheParam->szAdmin, pCacheParam->szPassword);
	//
	INITIAL_SESSION_BUFFER(m_pSession->buffer)
	AuthChecksum(szChecksum, pCacheParam->szAdmin, pCacheParam->szPassword);
	NSDTProtocol::BuildListSend(&m_pSession->buffer, dwListType, szChecksum);
	SetReplySocks(pCacheParam->Address.sin_addr, pCacheParam->Address.sin_port);
	if (0 > ReplySendReceive(&m_pSession->buffer)) return -1;
	if(!NSDTProtocol::ParseListRecv(&dwContentType, &dwContentLength, &dwListStatus, m_pSession->buffer.head_buffer)) 
		return -1;
	strcpy_s(szListXml, strlen(m_pSession->buffer.body_buffer)+1, m_pSession->buffer.body_buffer);
//
	return dwListStatus;
}

