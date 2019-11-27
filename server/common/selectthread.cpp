#include "pch.h"
#include "selectthread.h"
#include "socket.h"
#include "log.h"
#include "common_api.h"
#include "tcpclientsocket.h"
CSelectThread::CSelectThread(void)
{
	FD_ZERO(&m_FdSet);
	FD_ZERO(&m_Read);
	FD_ZERO(&m_Write);
	FD_ZERO(&m_Error);
}
CSelectThread::~CSelectThread(void)
{
}
void  CSelectThread::Run()
{
	if (!Init())
		KillSelf();
	timeval tv = {0,0};
	int32_t nRet = 0;
	while(IsRun())
	{
		SocketThreadBase::Run();
		m_Read = m_FdSet;
		m_Write = m_FdSet;
		m_Error = m_FdSet;
		nRet = ::select(0,&m_Read,&m_Write,&m_Error,&tv);//windows中第一个参数无用
		if(SOCKET_ERROR == nRet)
		{
			WRITE_ERROR_LOG("select has error ret=%d",nRet);
			KillSelf();
		}
		if (0 == nRet || DoEvent() || CanSleep())
			MySleep(1);
	}
}
bool CSelectThread::DoEvent()
{
	int32_t nRet = 0;
	bool doEvent = false;
	for (auto it = m_ClientSocket.begin(); it != m_ClientSocket.end();)
	{
		bool err = true;
		do 
		{
			if(FD_ISSET((it->second)->GetSocket(),&m_Error))
				break;
			if(FD_ISSET((it->second)->GetSocket(),&m_Read))
			{
				nRet = (it->second)->EpollIn();
				if(nRet < 0)break;
				doEvent |= nRet > 0;
			}
			if(FD_ISSET((it->second)->GetSocket(),&m_Write))
			{
				nRet = (it->second)->EpollOut();
				if (nRet < 0)break;
				doEvent |= nRet > 0;
			}
			err = false;
		} while (0);
		if( err && ErrorSocket(it->second))
			m_ClientSocket.erase(it++);
		else 
			++it;
	}
	return !doEvent;

}
bool CSelectThread::ErrorSocket(CSocket *pSocket)
{
	WRITE_ERROR_LOG("socket has error fd=%d host=%s port=%u error=%d session=%" PRIu64,pSocket->GetSocket(),
		pSocket->GetHost(),pSocket->GetPort(), pSocket->GetSocketError(),pSocket->GetSessionIndex());
	EpollDelete(pSocket->GetSocket());
	if(pSocket->EpollErr() < 0 )
		return true;
	return false;
}
int32_t CSelectThread::EpollDelete(const SOCKET fd)
{
	FD_CLR(fd,&m_FdSet);
	return S_OK;
}
int32_t CSelectThread::EpollAdd(CSocket *pSocket)
{
	FD_SET(pSocket->GetSocket(),&m_FdSet);
	return SocketThreadBase::EpollAdd(pSocket);
}