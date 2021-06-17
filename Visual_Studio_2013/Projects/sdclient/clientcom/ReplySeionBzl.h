#pragma once

#include "common_macro.h"
#include "StringUtility.h"
#include "ParseConfig.h"
#include "Session.h"

namespace NValSeionBzl {
	struct ValidSeion *Initialize(struct ClientConfig *pClientConfig);
	DWORD Finalize();
};

namespace NCmdSeionBzl {
	struct ComandSeion *Initialize(struct ClientConfig *pClientConfig);
	DWORD Finalize();
};