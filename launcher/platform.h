#pragma once

struct MsgColor
{
	unsigned char r, g, b, a;
};

void InitLog();

void DebugLogColor(bool bError, MsgColor color, const char *fmt, ...);

template <typename... Args>
inline void DebugLog(const char *fmt, Args... args)
{
	DebugLogColor(false, { 0, 200, 255, 255 }, fmt, args...);
}

template <typename... Args>
inline void DebugError(const char *fmt, Args... args)
{
	DebugLogColor(true, { 255, 0, 0, 255 }, fmt, args...);
}

// mainly for steam stuff
void HardError(const char *fmt, ...);

bool IsAddrFromModule(void *addr, const char *name);

#if defined(_WIN32)
#include <intrin.h>

#define GetReturnAddress() _ReturnAddress()
#else
#define GetReturnAddress() __builtin_return_address(0)
#endif
