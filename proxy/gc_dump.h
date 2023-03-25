#pragma once

class CGCProxyClient final : public ISteamGameCoordinator
{
public:
	virtual EGCResults SendMessage(uint32 unMsgType, const void *pubData, uint32 cubData) override;
	virtual bool IsMessageAvailable(uint32 *pcubMsgSize) override;
	virtual EGCResults RetrieveMessage(uint32 *punMsgType, void *pubDest, uint32 cubDest, uint32 *pcubMsgSize) override;

	ISteamGameCoordinator *m_pOriginal;
};

class CGCProxyServer final : public ISteamGameCoordinator
{
public:
	virtual EGCResults SendMessage(uint32 unMsgType, const void *pubData, uint32 cubData) override;
	virtual bool IsMessageAvailable(uint32 *pcubMsgSize) override;
	virtual EGCResults RetrieveMessage(uint32 *punMsgType, void *pubDest, uint32 cubDest, uint32 *pcubMsgSize) override;

	ISteamGameCoordinator *m_pOriginal;
};

extern CGCProxyClient GCProxyClient;
extern CGCProxyServer GCProxyServer;
