#pragma once

#include "common_macro.h"
#include "StringUtility.h"
#include "ParseConfig.h"
#include "Session.h"

namespace NValSessionBzl {
	struct ValidateSession *Initialize(struct ClientConfig *pClientConfig);
	DWORD Finalize();
};

namespace NComSessionBzl {
	struct CommandSession *Initialize(struct ClientConfig *pClientConfig);
	DWORD Finalize();
};