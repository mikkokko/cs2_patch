#include "precompiled.h"
#include <windows.h>

#if defined(_M_X64)
#define STEAM_API L"steam_api64.dll"
#else
#define STEAM_API L"steam_api.dll"
#endif

//------------------------------------------------------------------------------
// GC Proxy
//------------------------------------------------------------------------------

void *Hk_GetISteamGenericInterface(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion, void *pOriginal, void *pReturnAddress)
{
	if (!strcmp(pchVersion, STEAMGAMECOORDINATOR_INTERFACE_VERSION))
	{
		// mikkotodo ds support?
#if defined(_WIN32)
		if (IsAddrFromModule(pReturnAddress, "server.dll"))
#else
		if (IsAddrFromModule(pReturnAddress, "server_client.so"))
#endif
		{
			DebugLog("Server interface requested...\n");
			GCProxyServer.m_pOriginal = (ISteamGameCoordinator *)pOriginal;
			return &GCProxyServer;
		}
#if defined(_WIN32)
		else if (IsAddrFromModule(pReturnAddress, "client.dll"))
#else
		else if (IsAddrFromModule(pReturnAddress, "client_client.so"))
#endif
		{
			DebugLog("Client interface requested...\n");
			GCProxyClient.m_pOriginal = (ISteamGameCoordinator *)pOriginal;
			return &GCProxyClient;
		}
		else
		{
			HardError("Unsupported Game Coordinator implementation\n");
			return nullptr;
		}
	}

	return pOriginal;
}


//------------------------------------------------------------------------------
// cs2 beta dlc spoof
//------------------------------------------------------------------------------

class CSteamAppsProxy : public ISteamApps
{
public:
	ISteamApps *m_pOriginal;

	virtual bool BIsSubscribed() override { return m_pOriginal->BIsSubscribed(); }
	virtual bool BIsLowViolence() override { return m_pOriginal->BIsLowViolence(); }
	virtual bool BIsCybercafe() override { return m_pOriginal->BIsCybercafe(); }
	virtual bool BIsVACBanned() override { return m_pOriginal->BIsVACBanned(); }
	virtual const char *GetCurrentGameLanguage() override { return m_pOriginal->GetCurrentGameLanguage(); }
	virtual const char *GetAvailableGameLanguages() override { return m_pOriginal->GetAvailableGameLanguages(); }

	virtual bool BIsSubscribedApp(AppId_t appID) override
	{
		if (appID == 2279720)
		{
			// why yes we are invited to the cs2 limited test build
			return true;
		}

		return m_pOriginal->BIsSubscribedApp(appID);
	}

	virtual bool BIsDlcInstalled(AppId_t appID) override { return m_pOriginal->BIsDlcInstalled(appID); }
	virtual uint32 GetEarliestPurchaseUnixTime(AppId_t nAppID) override { return m_pOriginal->GetEarliestPurchaseUnixTime(nAppID); }
	virtual bool BIsSubscribedFromFreeWeekend() override { return m_pOriginal->BIsSubscribedFromFreeWeekend(); }
	virtual int GetDLCCount() override { return m_pOriginal->GetDLCCount(); }
	virtual bool BGetDLCDataByIndex(int iDLC, AppId_t *pAppID, bool *pbAvailable, char *pchName, int cchNameBufferSize) override { return m_pOriginal->BGetDLCDataByIndex(iDLC, pAppID, pbAvailable, pchName, cchNameBufferSize); }
	virtual void InstallDLC(AppId_t nAppID) override { return m_pOriginal->InstallDLC(nAppID); }
	virtual void UninstallDLC(AppId_t nAppID) override { return m_pOriginal->UninstallDLC(nAppID); }
	virtual void RequestAppProofOfPurchaseKey(AppId_t nAppID) override { return m_pOriginal->RequestAppProofOfPurchaseKey(nAppID); }
	virtual bool GetCurrentBetaName(char *pchName, int cchNameBufferSize) override { return m_pOriginal->GetCurrentBetaName(pchName, cchNameBufferSize); }
	virtual bool MarkContentCorrupt(bool bMissingFilesOnly) override { return m_pOriginal->MarkContentCorrupt(bMissingFilesOnly); }
	virtual uint32 GetInstalledDepots(AppId_t appID, DepotId_t *pvecDepots, uint32 cMaxDepots) override { return m_pOriginal->GetInstalledDepots(appID, pvecDepots, cMaxDepots); }
	virtual uint32 GetAppInstallDir(AppId_t appID, char *pchFolder, uint32 cchFolderBufferSize) override { return m_pOriginal->GetAppInstallDir(appID, pchFolder, cchFolderBufferSize); }
	virtual bool BIsAppInstalled(AppId_t appID) override { return m_pOriginal->BIsAppInstalled(appID); }
	virtual CSteamID GetAppOwner() override { return m_pOriginal->GetAppOwner(); }
	virtual const char *GetLaunchQueryParam(const char *pchKey) override { return m_pOriginal->GetLaunchQueryParam(pchKey); }
	virtual bool GetDlcDownloadProgress(AppId_t nAppID, uint64 *punBytesDownloaded, uint64 *punBytesTotal) override { return m_pOriginal->GetDlcDownloadProgress(nAppID, punBytesDownloaded, punBytesTotal); }
	virtual int GetAppBuildId() override { return m_pOriginal->GetAppBuildId(); }
	virtual void RequestAllProofOfPurchaseKeys() override { return m_pOriginal->RequestAllProofOfPurchaseKeys(); }
	virtual SteamAPICall_t GetFileDetails(const char *pszFileName) override { return m_pOriginal->GetFileDetails(pszFileName); }
	virtual int GetLaunchCommandLine(char *pszCommandLine, int cubCommandLine) override { return m_pOriginal->GetLaunchCommandLine(pszCommandLine, cubCommandLine); }
	virtual bool BIsSubscribedFromFamilySharing() override { return m_pOriginal->BIsSubscribedFromFamilySharing(); }
	virtual bool BIsTimedTrial(uint32 *punSecondsAllowed, uint32 *punSecondsPlayed) override { return m_pOriginal->BIsTimedTrial(punSecondsAllowed, punSecondsPlayed); }
	virtual bool SetDlcContext(AppId_t nAppID) override { return m_pOriginal->SetDlcContext(nAppID); }
};

CSteamAppsProxy SteamAppsProxy;

void *(*Og_SteamInternal_FindOrCreateUserInterface)(HSteamUser hSteamUser, const char *pchVersion);

void *Hk_SteamInternal_FindOrCreateUserInterface(HSteamUser hSteamUser, const char *pchVersion)
{
	// only spoof the latest version
	if (!strcmp(pchVersion, STEAMAPPS_INTERFACE_VERSION))
	{
		SteamAppsProxy.m_pOriginal = (ISteamApps *)Og_SteamInternal_FindOrCreateUserInterface(hSteamUser, pchVersion);
		return &SteamAppsProxy;
	}

	return Og_SteamInternal_FindOrCreateUserInterface(hSteamUser, pchVersion);
}

//------------------------------------------------------------------------------
// ISteamClient proxy
//------------------------------------------------------------------------------

// gc proxy by nikolay aulov georgievich
template<class T>
class CSteamClientProxyBase : public T
{
public:
	T *InitProxy(void *pOriginal)
	{
		pOrig = (T *)pOriginal;
		return this;
	}

	T *pOrig;
};

class CSteamClientProxy final :public CSteamClientProxyBase<ISteamClient>
{
public:
	virtual HSteamPipe CreateSteamPipe() override { return pOrig->CreateSteamPipe(); }
	virtual bool BReleaseSteamPipe(HSteamPipe hSteamPipe) override { return pOrig->BReleaseSteamPipe(hSteamPipe); }
	virtual HSteamUser ConnectToGlobalUser(HSteamPipe hSteamPipe) override { return pOrig->ConnectToGlobalUser(hSteamPipe); }
	virtual HSteamUser CreateLocalUser(HSteamPipe *phSteamPipe, EAccountType eAccountType) override { return pOrig->CreateLocalUser(phSteamPipe, eAccountType); }
	virtual void ReleaseUser(HSteamPipe hSteamPipe, HSteamUser hUser) override { return pOrig->ReleaseUser(hSteamPipe, hUser); }
	virtual ISteamUser *GetISteamUser(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamUser(hSteamUser, hSteamPipe, pchVersion); }
	virtual ISteamGameServer *GetISteamGameServer(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamGameServer(hSteamUser, hSteamPipe, pchVersion); }
	virtual void SetLocalIPBinding(const SteamIPAddress_t &unIP, uint16 usPort) override { return pOrig->SetLocalIPBinding(unIP, usPort); }
	virtual ISteamFriends *GetISteamFriends(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamFriends(hSteamUser, hSteamPipe, pchVersion); }
	virtual ISteamUtils *GetISteamUtils(HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamUtils(hSteamPipe, pchVersion); }
	virtual ISteamMatchmaking *GetISteamMatchmaking(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamMatchmaking(hSteamUser, hSteamPipe, pchVersion); }
	virtual ISteamMatchmakingServers *GetISteamMatchmakingServers(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamMatchmakingServers(hSteamUser, hSteamPipe, pchVersion); }
	virtual void *GetISteamGenericInterface(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion) override { return Hk_GetISteamGenericInterface(hSteamUser, hSteamPipe, pchVersion, pOrig->GetISteamGenericInterface(hSteamUser, hSteamPipe, pchVersion), GetReturnAddress()); }
	virtual ISteamUserStats *GetISteamUserStats(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamUserStats(hSteamUser, hSteamPipe, pchVersion); }
	virtual ISteamGameServerStats *GetISteamGameServerStats(HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamGameServerStats(hSteamuser, hSteamPipe, pchVersion); }
	virtual ISteamApps *GetISteamApps(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamApps(hSteamUser, hSteamPipe, pchVersion); }
	virtual ISteamNetworking *GetISteamNetworking(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamNetworking(hSteamUser, hSteamPipe, pchVersion); }
	virtual ISteamRemoteStorage *GetISteamRemoteStorage(HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamRemoteStorage(hSteamuser, hSteamPipe, pchVersion); }
	virtual ISteamScreenshots *GetISteamScreenshots(HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamScreenshots(hSteamuser, hSteamPipe, pchVersion); }
	virtual ISteamGameSearch *GetISteamGameSearch(HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamGameSearch(hSteamuser, hSteamPipe, pchVersion); }
	virtual void RunFrame() override { return pOrig->RunFrame(); }
	virtual uint32 GetIPCCallCount() override { return pOrig->GetIPCCallCount(); }
	virtual void SetWarningMessageHook(SteamAPIWarningMessageHook_t pFunction) override { return pOrig->SetWarningMessageHook(pFunction); }
	virtual bool BShutdownIfAllPipesClosed() override { return pOrig->BShutdownIfAllPipesClosed(); }
	virtual ISteamHTTP *GetISteamHTTP(HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamHTTP(hSteamuser, hSteamPipe, pchVersion); }
	virtual void *DEPRECATED_GetISteamUnifiedMessages(HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->DEPRECATED_GetISteamUnifiedMessages(hSteamuser, hSteamPipe, pchVersion); }
	virtual ISteamController *GetISteamController(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamController(hSteamUser, hSteamPipe, pchVersion); }
	virtual ISteamUGC *GetISteamUGC(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamUGC(hSteamUser, hSteamPipe, pchVersion); }
	virtual ISteamAppList *GetISteamAppList(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamAppList(hSteamUser, hSteamPipe, pchVersion); }
	virtual ISteamMusic *GetISteamMusic(HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamMusic(hSteamuser, hSteamPipe, pchVersion); }
	virtual ISteamMusicRemote *GetISteamMusicRemote(HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamMusicRemote(hSteamuser, hSteamPipe, pchVersion); }
	virtual ISteamHTMLSurface *GetISteamHTMLSurface(HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamHTMLSurface(hSteamuser, hSteamPipe, pchVersion); }
	virtual void DEPRECATED_Set_SteamAPI_CPostAPIResultInProcess(void (*arg)()) override { return pOrig->DEPRECATED_Set_SteamAPI_CPostAPIResultInProcess(arg); }
	virtual void DEPRECATED_Remove_SteamAPI_CPostAPIResultInProcess(void (*arg)()) override { return pOrig->DEPRECATED_Remove_SteamAPI_CPostAPIResultInProcess(arg); }
	virtual void Set_SteamAPI_CCheckCallbackRegisteredInProcess(SteamAPI_CheckCallbackRegistered_t func) override { return pOrig->Set_SteamAPI_CCheckCallbackRegisteredInProcess(func); }
	virtual ISteamInventory *GetISteamInventory(HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamInventory(hSteamuser, hSteamPipe, pchVersion); }
	virtual ISteamVideo *GetISteamVideo(HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamVideo(hSteamuser, hSteamPipe, pchVersion); }
	virtual ISteamParentalSettings *GetISteamParentalSettings(HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamParentalSettings(hSteamuser, hSteamPipe, pchVersion); }
	virtual ISteamInput *GetISteamInput(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamInput(hSteamUser, hSteamPipe, pchVersion); }
	virtual ISteamParties *GetISteamParties(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamParties(hSteamUser, hSteamPipe, pchVersion); }
	virtual ISteamRemotePlay *GetISteamRemotePlay(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion) override { return pOrig->GetISteamRemotePlay(hSteamUser, hSteamPipe, pchVersion); }
	virtual void DestroyAllInterfaces() override { return pOrig->DestroyAllInterfaces(); }
};

static CSteamClientProxy SteamClientProxy;

static void *(*Og_SteamInternal_CreateInterface)(const char *ver);

// for newer builds
void *Hk_SteamInternal_CreateInterface(const char *ver)
{
	void *pOriginal = Og_SteamInternal_CreateInterface(ver);

	if (!memcmp(ver, "SteamClient", 11))
	{
		if (!strcmp(ver, STEAMCLIENT_INTERFACE_VERSION))
			return SteamClientProxy.InitProxy(pOriginal);

		HardError("Could not find SteamClient\n");
	}

	return pOriginal;
}

//-------------------------------------

#define MAX_ADDRESS_OFFSET ((DWORD)~0)

void *AllocateNear(void *pOrigin, size_t nSize)
{
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	UINT_PTR Address = (UINT_PTR)pOrigin;
	UINT_PTR MaxAddress = (UINT_PTR)pOrigin + MAX_ADDRESS_OFFSET;

	Address -= Address % SystemInfo.dwAllocationGranularity;
	Address += SystemInfo.dwAllocationGranularity;

	while (Address <= MaxAddress)
	{
		MEMORY_BASIC_INFORMATION MemoryInfo;
		if (!VirtualQuery((void *)Address, &MemoryInfo, sizeof(MemoryInfo)))
			break;

		if (MemoryInfo.State == MEM_FREE)
		{
			void *pBlock = VirtualAlloc((void *)Address, nSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			if (pBlock)
				return pBlock;
		}

		Address = (UINT_PTR)MemoryInfo.BaseAddress + MemoryInfo.RegionSize;

		Address += SystemInfo.dwAllocationGranularity - 1;
		Address -= Address % SystemInfo.dwAllocationGranularity;
	}

	return NULL;
}

void *TrampolineNear(void *pAddress, void *pDestination)
{
	UINT8 Payload[] =
	{
		0x50, // push   rax
		0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // movabs rax, pDestination
		0x48, 0x87, 0x04, 0x24, // xchg  QWORD PTR [rsp], rax
		0xc3 // ret
	};

	void* pTrampoline = AllocateNear(pAddress, sizeof(Payload));
	if (!pTrampoline)
		return NULL;

	memcpy(&Payload[3], &pDestination, sizeof(pDestination));
	memcpy(pTrampoline, Payload, sizeof(Payload));

	return pTrampoline;
}

DWORD *FindFromEAT(uintptr_t hModule, const char *szFunction)
{
	IMAGE_DOS_HEADER *DosHeader = (IMAGE_DOS_HEADER *)(hModule);
	IMAGE_NT_HEADERS *NtHeader = (IMAGE_NT_HEADERS *)(hModule + DosHeader->e_lfanew);
	IMAGE_DATA_DIRECTORY *DataDirectory = &NtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	IMAGE_EXPORT_DIRECTORY *ExportDirectory = (IMAGE_EXPORT_DIRECTORY *)(hModule + DataDirectory->VirtualAddress);

	DWORD *AddressOfFunctions = (DWORD *)(hModule + ExportDirectory->AddressOfFunctions);
	DWORD *AddressOfNames = (DWORD *)(hModule + ExportDirectory->AddressOfNames);
	WORD *AddressOfNameOrdinals = (WORD *)(hModule + ExportDirectory->AddressOfNameOrdinals);

	for (DWORD i = 0; i < ExportDirectory->NumberOfFunctions; i++)
	{
		char *szName = (char *)(hModule + AddressOfNames[i]);

		if (!strcmp(szFunction, szName))
			return AddressOfFunctions + AddressOfNameOrdinals[i];
	}

	return NULL;
}

void InstallHookEAT(HMODULE hModule, const char *szFunction, void *pHook, void **ppOriginal)
{
	DWORD *Offset = FindFromEAT((uintptr_t)hModule, szFunction);
	if (!Offset)
	{
		HardError("Could not hook %s", szFunction);
		return;
	}

	uintptr_t HookOffset = (uintptr_t)pHook - (uintptr_t)hModule;

	if (HookOffset > MAX_ADDRESS_OFFSET)
	{
		void *pTrampoline = TrampolineNear(hModule, pHook);
		if (!pTrampoline)
		{
			HardError("Could not hook %s", szFunction);
			return;
		}

		HookOffset = (uintptr_t)pTrampoline - (uintptr_t)hModule;
	}

	*ppOriginal = (void *)(*Offset + (uintptr_t)hModule);

	DWORD flOldProtect;
	VirtualProtect(Offset, sizeof(DWORD), PAGE_READWRITE, &flOldProtect);
	*Offset = HookOffset;
	VirtualProtect(Offset, sizeof(DWORD), flOldProtect, &flOldProtect);
}

static HMODULE LoadSteam(const wchar_t *szBaseDir)
{
	// try the easy way out
	HMODULE hSteam = LoadLibraryExW(STEAM_API, NULL, 0);
	if (hSteam)
		return hSteam;

	// sorry that didn't work out, god knows we tried
	return NULL;
}

//-------------------------------------

void InstallSteamProxy(const wchar_t *szBaseDir)
{
	HMODULE hSteam = LoadSteam(szBaseDir);
	if (!hSteam)
		HardError("Could not find Steam\n");

#define HOOK_EAT(name) InstallHookEAT(hSteam, #name, Hk_##name, (void **)&Og_##name)
	HOOK_EAT(SteamInternal_CreateInterface);
	HOOK_EAT(SteamInternal_FindOrCreateUserInterface);
#undef HOOK_EAT

	// should be removed
	InitLog();
}
