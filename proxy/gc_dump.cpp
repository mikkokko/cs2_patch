#include "precompiled.h"

#include "gcsystemmsgs.pb.h"
#include "gcsdk_gcmessages.pb.h"

CGCProxyClient GCProxyClient;
CGCProxyServer GCProxyServer;

constexpr uint32 k_nProtoMask = 0x80000000;

#pragma pack(push, 1)
struct ProtoMsgHeader
{
	uint32 type;
	uint32 header_size;
};
#pragma pack(pop)

class CMsgWrite /* : public CNonCopyable */
{
public:
	CMsgWrite(uint32 unMsgType, google::protobuf::Message &Message)
	{
		// protobuf message so add the mask (already in!!!)

		// allocate the buffer
		m_cubData = Message.ByteSizeLong() + sizeof(ProtoMsgHeader);
		m_pubData = new uint8_t[m_cubData];

		// build the header
		ProtoMsgHeader *pHeader = (ProtoMsgHeader *)m_pubData;
		pHeader->type = unMsgType;
		pHeader->header_size = 0;

		// serialize the message after the header
		Message.SerializeToArray(m_pubData + sizeof(ProtoMsgHeader), Message.ByteSizeLong());
	}

	~CMsgWrite()
	{
		delete[] m_pubData;
	}

	uint8_t *m_pubData;
	uint32 m_cubData;
};

#define DumpMessage(...) (void)0

EGCResults CGCProxyClient::SendMessage(uint32 unMsgType, const void *pubData, uint32 cubData)
{
	// make the gc think that this is a source 1 client
	if (unMsgType == (k_EMsgGCClientHello | k_nProtoMask))
	{
		CMsgClientHello NewHello;
		NewHello.set_version(1548);
		NewHello.set_client_session_need(0);
		NewHello.set_client_launcher(0);
		NewHello.set_steam_launcher(0);
	
		CMsgWrite ReplaceHello(unMsgType, NewHello);
	
		DumpMessage(false, false, unMsgType, ReplaceHello.m_pubData, ReplaceHello.m_cubData);
		return m_pOriginal->SendMessage(unMsgType, ReplaceHello.m_pubData, ReplaceHello.m_cubData);
	}

	DumpMessage(false, false, unMsgType, pubData, cubData);
	return m_pOriginal->SendMessage(unMsgType, pubData, cubData);
}

bool CGCProxyClient::IsMessageAvailable(uint32 *pcubMsgSize)
{
	return m_pOriginal->IsMessageAvailable(pcubMsgSize);
}

EGCResults CGCProxyClient::RetrieveMessage(uint32 *punMsgType, void *pubDest, uint32 cubDest, uint32 *pcubMsgSize)
{
	EGCResults eResult = m_pOriginal->RetrieveMessage(punMsgType, pubDest, cubDest, pcubMsgSize);

	if (eResult == k_EGCResultOK)
		DumpMessage(false, true, *punMsgType, pubDest, *pcubMsgSize);

	return eResult;
}

EGCResults CGCProxyServer::SendMessage(uint32 unMsgType, const void *pubData, uint32 cubData)
{
	DumpMessage(true, false, unMsgType, pubData, cubData);
	return m_pOriginal->SendMessage(unMsgType, pubData, cubData);
}

bool CGCProxyServer::IsMessageAvailable(uint32 *pcubMsgSize)
{
	return m_pOriginal->IsMessageAvailable(pcubMsgSize);
}

EGCResults CGCProxyServer::RetrieveMessage(uint32 *punMsgType, void *pubDest, uint32 cubDest, uint32 *pcubMsgSize)
{
	EGCResults eResult = m_pOriginal->RetrieveMessage(punMsgType, pubDest, cubDest, pcubMsgSize);

	if (eResult == k_EGCResultOK)
		DumpMessage(true, true, *punMsgType, pubDest, *pcubMsgSize);

	return eResult;
}
