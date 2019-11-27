#include "pch.h"
#include "listensocket.h"
#include "log.h"
#include "socketthread.h"
#include "common_api.h"
CListenSocket::CListenSocket(Factory factory,uint32_t nPort,const char* host/*=nullptr*/)
	:CSocket(host,nPort),m_factory(factory)
{
	if( S_OK != OpenAsServer())
		KillSelf();
}
CListenSocket::~CListenSocket(void)
{
}
int32_t CListenSocket::OpenAsServer()
{
	Close();
	if (S_OK != CreateSocket())
	{
		WRITE_ERROR_LOG("create socket has error");
		return E_UNKNOW;
	}
	if (!SetNonBlocking())
	{
		WRITE_ERROR_LOG("set nonblock has error");
		return E_UNKNOW;
	}
	if (!SetReuseAddr())
	{
		WRITE_ERROR_LOG("set refuse has error");
		return E_UNKNOW;
	}
	if (!SetNagle())
	{
		WRITE_ERROR_LOG("set nagle has error");
		return E_UNKNOW;
	}
	if (S_OK != Bind())
	{
		WRITE_ERROR_LOG("bind socket has error");
		return E_UNKNOW;
	}
	if (S_OK != Listen())
	{
		WRITE_ERROR_LOG("lisen soket has eror");
		return E_UNKNOW;
	}
	if(S_OK != g_pSocketThread->EpollAdd(this))
	{
		WRITE_ERROR_LOG("add epoll error");
		return E_UNKNOW;
	}
	WRITE_DEBUG_LOG("create listen socket sucess port=%u host=%s",GetPort(),GetHost());
	return S_OK;
}
int32_t CListenSocket::EpollIn()
{
	for (uint32_t i = 0; i < MaxOnceAccept; ++i)
	{
		sockaddr_in addr = {0};
		SOCKET fd = INVALID_SOCKET;
		if (INVALID_SOCKET ==(fd = Accept(&addr)))
			return HasError() ? E_UNKNOW : S_OK;
		auto pClient = CreateNewScoket();
		pClient->SetAddAndSocket(addr,fd);
		pClient->SetNonBlocking(true);
		pClient->SetSessionIndex(g_pSocketThread->GetSessionID());
		if(S_OK != g_pSocketThread->EpollAdd(pClient) || !pClient->OnAccept())
			delete pClient;
		else
		{
			WRITE_INFO_LOG("accept new socket host=%s port=%u index=%" PRIu64,
				pClient->GetHost(), pClient->GetPort(), pClient->GetSessionIndex());
		}
	}
	g_pSocketThread->EpollMod(this);
	return S_OK;
}
int32_t CListenSocket::EpollErr()
{
	if(S_OK != OpenAsServer())
		KillSelf();
	return S_OK;
}