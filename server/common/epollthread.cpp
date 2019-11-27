#include "pch.h" 
#include "epollthread.h"
#include "common_api.h"
#include "socket.h"
#include "log.h"
#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#define close _close
#endif
#include "tcpclientsocket.h"
CEpollThread::CEpollThread()
{
	m_FdEpoll = 0;
	memset(m_pEvent, 0, sizeof m_pEvent);
}
CEpollThread::~CEpollThread()
{
	close(m_FdEpoll);
}
void CEpollThread::Run()
{
	if (!Init())
		KillSelf();
	while (IsRun())
	{
		SocketThreadBase::Run();
		int32_t nCount = epoll_wait(m_FdEpoll,m_pEvent,MaxEpollSocket,0);
		if (nCount < 0)
		{
			WRITE_ERROR_LOG("epoll wait has error");
			break;
		}
		else if(nCount > 0)
			DoEvent(nCount);
		else if(CanSleep())
			MySleep(1);
	}
}
int32_t CEpollThread::DoEvent(const int32_t nCount)
{
	for (int32_t i = 0; i < nCount; ++i)
	{
		CSocket *pSocket = reinterpret_cast<CSocket*>(m_pEvent[i].data.ptr);
		if(unlikely(NULL == pSocket)) continue;
		if ((m_pEvent[i].events & EPOLLIN) && pSocket->EpollIn() < 0)
		{
			ErrorSocket(pSocket);
			continue;
		}
		if((m_pEvent[i].events & EPOLLOUT) && pSocket->EpollOut() < 0)
		{
			ErrorSocket(pSocket);
			continue;
		}
	}
	return 0;
}
bool CEpollThread::ErrorSocket(CSocket *pSocket)
{
	WRITE_ERROR_LOG("socket has error fd=%d error=%d ip=%s port=%u session=%" PRIu64,pSocket->GetSocket(),
		pSocket->GetSocketError(),pSocket->GetHost(),pSocket->GetPort(),pSocket->GetSessionIndex());
	EpollDelete(pSocket->GetSocket());
	auto sessionId = pSocket->GetSessionIndex();
	if(pSocket->EpollErr() < 0 )
		m_ClientSocket.erase(sessionId);
	return false;
}
bool CEpollThread::Init()
{
	m_FdEpoll = epoll_create(1);
	if (m_FdEpoll < 0)
	{
		WRITE_ERROR_LOG("create epoll has error");
		return false;
	}
	return true;
}
int32_t CEpollThread::EpollMod(CSocket *pSocket)
{
	epoll_event ev = {0};
	ev.data.ptr =(void *) pSocket;
	ev.events = EPOLLIN | EPOLLET |(pSocket->IsListenSocket() ? 0 : EPOLLOUT);
	if(epoll_ctl(m_FdEpoll,EPOLL_CTL_MOD,pSocket->GetSocket(),&ev) < 0)
	{
		WRITE_ERROR_LOG("mode epoll has error fd=%d error=%d ip=%s port=%u session=%" PRIu64, pSocket->GetSocket(),
			pSocket->GetSocketError(), pSocket->GetHost(), pSocket->GetPort(), pSocket->GetSessionIndex());
		return E_UNKNOW;
	}
	return S_OK;
}
int32_t CEpollThread::EpollDelete(const SOCKET fd)
{
	if(epoll_ctl(m_FdEpoll,EPOLL_CTL_DEL,fd,nullptr) < 0)
	{
		WRITE_ERROR_LOG("delete epoll has error fd=%u",fd);
		return E_UNKNOW;
	}
	return S_OK;
}
int32_t CEpollThread::EpollAdd(CSocket *pSocket)
{
	epoll_event ev = {0};
	ev.data.ptr =(void *) pSocket;
	ev.events = EPOLLIN | EPOLLET |(pSocket->IsListenSocket() ? 0 : EPOLLOUT);
	if(epoll_ctl(m_FdEpoll,EPOLL_CTL_ADD,pSocket->GetSocket(),&ev) < 0)
	{
		WRITE_ERROR_LOG("add epoll has error fd=%d error=%d ip=%s port=%u", pSocket->GetSocket(),
			pSocket->GetSocketError(), pSocket->GetHost(), pSocket->GetPort());
		return E_UNKNOW;
	}
	return SocketThreadBase::EpollAdd(pSocket);
}