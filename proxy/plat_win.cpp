#include "precompiled.h"
#include <windows.h>

typedef void (*ConColorMsg_t)(const MsgColor &color, const char *fmt, ...);
ConColorMsg_t ConColorMsg;

void InitLog()
{
	HMODULE hTier0 = LoadLibraryExW(L"tier0.dll", NULL, 0);
	if (!hTier0)
		return;
	
	// try source2 one
	ConColorMsg = (ConColorMsg_t)GetProcAddress(hTier0, "?ConColorMsg@@YAXAEBVColor@@PEBDZZ");
}

void DebugLogColor(bool bError, MsgColor color, const char *fmt, ...)
{
	va_list ap;
	char buf[1024];

	if (!ConColorMsg)
		return;

	va_start(ap, fmt);
	int len = vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	ConColorMsg(color, "[GC Proxy] %s", buf);
}

void HardError(const char *fmt, ...)
{
	va_list ap;
	char buf[1024];

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	MessageBoxA(NULL, buf, "Error", MB_OK);
	ExitProcess(1);
}

void *FindSymbol(void *handle, const char *symbol)
{
	return (void *)GetProcAddress((HMODULE)handle, symbol);
}

void *GetModule(const char *name)
{
	return (void *)GetModuleHandleA(name);
}

bool IsAddrFromModule(void *addr, const char *name)
{
	HMODULE hModule;
	GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)addr, &hModule);
	return (hModule == GetModuleHandleA(name));
}
