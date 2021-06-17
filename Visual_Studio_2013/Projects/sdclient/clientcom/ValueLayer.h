#pragma once

#include "Session.h"
#include "SDTProtocol.h"

//
namespace NValueLayer {
	//
	VOID RegistRequest(struct RegistRequest *pReqValue, struct ListData *ldata);
	VOID RegistResponse(struct ValInteValue *ival, struct RegistResponse *pResValue);

	VOID SettingsRequest(struct RegistRequest *pReqValue, struct ListData *ldata);
	VOID SettingsResponse(struct ValInteValue *ival, struct RegistResponse *pResValue);

	VOID LinkRequest(struct RegistRequest *pReqValue, struct ListData *ldata);
	VOID LinkResponse(struct RegistResponse *pResValue);

	VOID UnlinkRequest(struct RegistRequest *pReqValue, struct ListData *ldata);
	VOID UnlinkResponse(struct RegistResponse *pResValue);

	//
	VOID QueryRequest(struct AuthRequest *pReqValue, struct ListData *ldata);
	VOID QueryResponse(struct ComInteValue *ival, struct AuthResponse *pResValue);

	//
	VOID InitialRequest(struct ServRequest *pReqValue, struct ListData *ldata);
	VOID InitialResponse(struct InteValue *ival, struct ServResponse *pResValue);

	VOID KeepAliveRequest(struct ServRequest *pReqValue, struct ListData *ldata);
	VOID KeepAliveResponse(struct InteValue *ival, struct ServResponse *pResValue);

	VOID HeadListRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival);
	VOID HeadListResponse(struct InteValue *ival, struct ServResponse *pResValue);

	VOID HeadFileRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival);
	VOID HeadFileResponse(struct InteValue *ival, struct ServResponse *pResValue);

	VOID GetAnchorRequest(struct ServRequest *pReqValue, struct ListData *ldata);
	VOID GetAnchorResponse(struct InteValue *ival, struct ServResponse *pResValue);

	VOID GetListRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival);
	VOID GetListResponse(struct InteValue *ival, struct ServResponse *pResValue);

	VOID GetFileRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival);
	VOID GetFileResponse(struct InteValue *ival, struct ServResponse *pResValue);

	VOID GetChksRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival);
	VOID GetChksResponse(struct InteValue *ival, struct ServResponse *pResValue);

	VOID PostRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival);
	VOID PostResponse(struct InteValue *ival, struct ServResponse *pResValue);

	VOID PutFileRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival);
	VOID PutFileResponse(struct InteValue *ival, struct ServResponse *pResValue);

	VOID PutDireRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival);
	VOID PutDireResponse(struct InteValue *ival, struct ServResponse *pResValue);

	VOID PutChksRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival);
	VOID PutChksResponse(struct InteValue *ival, struct ServResponse *pResValue);

	VOID MoveRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival);
	VOID MoveResponse(struct InteValue *ival, struct ServResponse *pResValue);

	VOID CopyRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival);
	VOID CopyResponse(struct InteValue *ival, struct ServResponse *pResValue);

	VOID DeleteRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival);
	VOID DeleteResponse(struct InteValue *ival, struct ServResponse *pResValue);

	VOID FinalRequest(struct ServRequest *pReqValue, DWORD dwFinalType, struct ListData *ldata);
	VOID FinalSuccessResponse(struct InteValue *ival, struct ServResponse *pResValue);
	VOID FinalEndResponse(struct InteValue *ival, struct ServResponse *pResValue);
	VOID FinalFinishResponse(struct InteValue *ival, struct ServResponse *pResValue);

	//
};
