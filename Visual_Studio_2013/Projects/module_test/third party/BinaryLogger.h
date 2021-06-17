#pragma once


#ifndef BINARY_LOGGER_H_
#define BINARY_LOGGER_H_

namespace BinaryLogger {
	VOID LogBinaryCode(char *binary_code, int length);
	VOID LogBinaryCodeW(TCHAR *binary_code, int length);
}

#endif /* BINARY_LOGGER_H_ */