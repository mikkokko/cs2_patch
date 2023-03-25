#pragma once

struct MsgColor
{
	MsgColor(int _r, int _g, int _b, int _a) :
		r(_r),
		g(_g),
		b(_b),
		a(_a)
	{
	}

	uint8 r, g, b, a;
};

void InitLog();

void DebugLogColor(bool bError, MsgColor color, const char *fmt, ...);

template <typename... Args>
inline void DebugLog(const char *fmt, Args... args)
{
	DebugLogColor(false, MsgColor(0, 200, 255, 255), fmt, args...);
}

template <typename... Args>
inline void DebugError(const char *fmt, Args... args)
{
	DebugLogColor(true, MsgColor(255, 0, 0, 255), fmt, args...);
}

// mainly for steam stuff
void HardError(const char *fmt, ...);

void *FindSymbol(void *handle, const char *symbol);

template<typename T>
void FindSymbol(T *dst, void *handle, const char *symbol)
{
	*dst = (T)FindSymbol(handle, symbol);
}

void *GetModule(const char *name);

bool IsAddrFromModule(void *addr, const char *name);

#if defined(_WIN32)
#include <intrin.h>

#define GetReturnAddress() _ReturnAddress()
#else
#define GetReturnAddress() __builtin_return_address(0)
#endif
