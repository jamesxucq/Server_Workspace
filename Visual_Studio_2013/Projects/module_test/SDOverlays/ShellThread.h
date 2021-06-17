#pragma once

#include "./utility/ipc.h"

namespace ShellThread {
	DWORD EnsureThreadOpen();
	void CloseThread();
};
