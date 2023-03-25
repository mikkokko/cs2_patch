#include <windows.h>
#include <cwchar>

void InstallSteamProxy(const wchar_t *szBaseDir); // steam_proxy.cpp

#if !defined(DEDICATED)
#define EXPORT extern "C" __declspec(dllexport)

EXPORT DWORD NvOptimusEnablement = 1;
EXPORT int AmdPowerXpressRequestHighPerformance = 1;

// these are always irrelevant apparently?

EXPORT bool BSecureAllowed(unsigned char *a1, int a2, int a3)
{
	return true;
}

EXPORT int CountFilesCompletedTrustCheck()
{
	return 0;
}

EXPORT int CountFilesNeedTrustCheck()
{
	return 0;
}

EXPORT int GetTotalFilesLoaded()
{
	return 0;
}

EXPORT int RuntimeCheck(int a1, int a2)
{
	return 0;
}

// these are for cs2

EXPORT int BinaryProperties_GetValue(int a1, void *a2)
{
	return 0;
}

EXPORT int CountItemsToReport()
{
	return 0;
}

#endif

typedef void(*Source2Main_t)(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd, const char *szBaseDir, const char *szGame);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	wchar_t szBaseDir[MAX_PATH];
	wchar_t szEngine2Path[MAX_PATH];
	char szBaseDirUTF8[MAX_PATH];

	if (!GetModuleFileNameW(NULL, szBaseDir, ARRAYSIZE(szBaseDir)))
	{
		MessageBoxW(NULL, L"GetModuleFileName failed", L"Launcher Error", MB_OK);
		return 1;
	}

	// rip off exe from the path
	wchar_t *pSlash = wcsrchr(szBaseDir, '\\');
	if (!pSlash)
		pSlash = szBaseDir; // what the fuck

	*pSlash = '\0';

	_snwprintf_s(szEngine2Path, ARRAYSIZE(szEngine2Path), L"%ls\\engine2.dll", szBaseDir);
	HMODULE hEngine2 = LoadLibraryExW(szEngine2Path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (!hEngine2)
	{
		MessageBoxW(NULL, L"Could not load engine2", L"Launcher Error", MB_OK);
		return 1;
	}

	Source2Main_t pSource2Main = (Source2Main_t)GetProcAddress(hEngine2, "Source2Main");
	if (!pSource2Main)
	{
		MessageBoxW(NULL, L"Could not find Source2Main from engine2", L"Launcher Error", MB_OK);
		return 1;
	}

	if (!WideCharToMultiByte(CP_UTF8, 0, szBaseDir, -1, szBaseDirUTF8, MAX_PATH, NULL, NULL))
	{
		MessageBoxW(NULL, L"Could not convert base path", L"Launcher Error", MB_OK);
		return 1;
	}

	InstallSteamProxy(szBaseDir);

	pSource2Main(hInstance, hPrevInstance, lpCmdLine, nShowCmd, szBaseDirUTF8, "csgo");
	return true;
}