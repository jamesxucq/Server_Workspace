#pragma once

#include "Session.h"
#include "SDTProtocol.h"

///////////////////////////////////////////////////////////////////////
namespace NValueLayer
{
	///////////////////////////////////////////////////////////////////////
	void RegistRequest(struct RegistRequest *RequestValue, struct ListInform *list_inform);
	void RegistResponse(struct ValInteValue *psvalue, struct RegistResponse *ResponseValue);

	void SettingsRequest(struct RegistRequest *RequestValue, struct ListInform *list_inform);
	void SettingsResponse(struct ValInteValue *psvalue, struct RegistResponse *ResponseValue);

	void LinkRequest(struct RegistRequest *RequestValue, struct ListInform *list_inform);
	void LinkResponse(struct RegistResponse *ResponseValue);

	void UnlinkRequest(struct RegistRequest *RequestValue, struct ListInform *list_inform);
	void UnlinkResponse(struct RegistResponse *ResponseValue);

	///////////////////////////////////////////////////////////////////////
	void QueryRequest(struct AuthRequest *RequestValue, struct ListInform *list_inform);
	void QueryResponse(struct ComInteValue *psvalue, struct AuthResponse *ResponseValue);

	///////////////////////////////////////////////////////////////////////
	void InitialRequest(struct ServRequest *RequestValue, struct ListInform *list_inform);
	void InitialResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue);

	void GetAnchorRequest(struct ServRequest *RequestValue, struct ListInform *list_inform);
	void GetAnchorResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue);

	void GetListRequest(struct ServRequest *RequestValue, struct ListInform *list_inform, struct InternalValue *psvalue);
	void GetListResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue);

	void GetFileRequest(struct ServRequest *RequestValue, struct ListInform *list_inform, struct InternalValue *psvalue);
	void GetFileResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue);

	void GetCsumRequest(struct ServRequest *RequestValue, struct ListInform *list_inform, struct InternalValue *psvalue);
	void GetCsumResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue);

	void PostRequest(struct ServRequest *RequestValue, struct ListInform *list_inform, struct InternalValue *psvalue);
	void PostResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue);

	void PutFileRequest(struct ServRequest *RequestValue, struct ListInform *list_inform, struct InternalValue *psvalue);
	void PutFileResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue);

	void PutCsumRequest(struct ServRequest *RequestValue, struct ListInform *list_inform, struct InternalValue *psvalue);
	void PutCsumResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue);

	void MoveRequest(struct ServRequest *RequestValue, struct ListInform *list_inform, struct InternalValue *psvalue);
	void MoveResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue);

	void DeleteRequest(struct ServRequest *RequestValue, struct ListInform *list_inform, struct InternalValue *psvalue);
	void DeleteResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue);

	void FinalRequest(struct ServRequest *RequestValue, DWORD dwFinalType, struct ListInform *list_inform);
	void FinalSuccessResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue);
	void FinalEndResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue);
	void FinalFinishResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue);

	///////////////////////////////////////////////////////////////////////
};
