#include "pch.h"
#include "socketthread.h"
#include "tcpclientsocket.h"
#include "log.h"
#include "singleton.h"
#include "serverbase.h"
#include "listensocket.h"
#include"timermanager.h"
#include "datetime.h"
SocketThreadBase* g_pSocketThread = nullptr;
SocketThreadBase::SocketThreadBase():m_nSeesionIndex(0), m_impl(this)
{
}
SocketThreadBase::~SocketThreadBase()
{
	for (auto it = m_ClientSocket.begin(); it != m_ClientSocket.end(); ++it)
		delete it->second;
}
void SocketThreadBase::OnTimerEvent(const TimerDataInfo& data, const CDateTime& dt)
{
	switch (data.type)
	{
	case TimerType::enmTimerType_SocketAct://被动连接才会有
	{
		auto iter = m_ClientSocket.find(data.data.u64);
		if (iter == m_ClientSocket.end())
		{
			DeleteTimer(data.id);
			break;
		}
		if (iter->second->Active(dt.GetSeconds()))break;
		WRITE_ERROR_LOG("socket not active fd=%d error=%d ip=%s port=%u session=%" PRIu64, iter->second->GetSocket(),
			iter->second->GetSocketError(), iter->second->GetHost(), iter->second->GetPort(), iter->second->GetSessionIndex());
		EpollDelete(iter->second->GetSocket());
		iter->second->EpollErr();
		m_ClientSocket.erase(iter);
		DeleteTimer(data.id);
		break;
	}
	case TimerType::enmTimerType_SocketClose:
	{
		auto iter = m_ClientSocket.find(data.data.u64);
		if (iter == m_ClientSocket.end())break;
		WRITE_ERROR_LOG("socket close fd=%d error=%d ip=%s port=%u session=%" PRIu64, iter->second->GetSocket(),
			iter->second->GetSocketError(), iter->second->GetHost(), iter->second->GetPort(), iter->second->GetSessionIndex());
		EpollDelete(iter->second->GetSocket());
		iter->second->EpollErr();
		m_ClientSocket.erase(iter);
		break;
	}
	case TimerType::enmTimerType_AddSocket://主线程
		while (!m_connectList.empty() && m_waitConnect.Push(m_connectList.front()) == S_OK)
			m_connectList.pop_front();
		if(!m_connectList.empty())
			m_impl.AddIntervalTimer(TimerType::enmTimerType_AddSocket, 50);
		break;
	case TimerType::enmTimerType_KickSocket://主线程
		while (!m_kickList.empty() && m_kickSession.Push(m_kickList.front()) == S_OK)
			m_kickList.pop_front();
		if (!m_kickList.empty())
			m_impl.AddIntervalTimer(TimerType::enmTimerType_KickSocket, 50);
		break;
	default:
		break;
	}
}
void SocketThreadBase::AddConnectSocket()
{
	uint32_t nCount = 0;
	CClientSocket* pSocket;
	while (++nCount < 50 && m_waitConnect.Pop(pSocket) == S_OK)
	{
		pSocket->SetSessionIndex(GetSessionID());
		if (pSocket->OpenAsClient() == S_OK)
			EpollAdd(pSocket);
		else
		{
			WRITE_ERROR_LOG("open connect socket has error host=%s port=%u", pSocket->GetHost(), pSocket->GetPort());
			delete pSocket;
		}
	}
}
void SocketThreadBase::KickSession()
{
	SessionID id;
	while (m_kickSession.Pop(id) == S_OK)
	{
		auto iter = m_ClientSocket.find(id);
		if (iter != m_ClientSocket.end())
		{
			EpollDelete(iter->second->GetSocket());
			WRITE_ERROR_LOG("kick socket host=%s port=%u session=%" PRIu64, iter->second->GetHost(), iter->second->GetPort(), id);
			iter->second->EpollErr();
			m_ClientSocket.erase(iter);
		}
	}
}
 bool SocketThreadBase::ErrorSocket(CSocket *pSocket)
{
	 return true;
}

 void SocketThreadBase::AddConnectSocket(CClientSocket* pSocket) //主线程
 {
	 if (!m_connectList.empty())
		 m_connectList.emplace_back(pSocket);
	 else if(m_waitConnect.Push(pSocket) != S_OK)
	 {
		 m_connectList.emplace_back(pSocket);
		 m_impl.AddIntervalTimer(TimerType::enmTimerType_AddSocket, 50);
	 }
 }
 void SocketThreadBase::AddKickSession(SessionID id) //主线程
 {
	 if (!m_kickList.empty())
		 m_kickList.emplace_back(id);
	 else if (m_kickSession.Push(id) != S_OK)
	 {
		 m_kickList.emplace_back(id);
		 m_impl.AddIntervalTimer(TimerType::enmTimerType_KickSocket, 50);
	 }
 }
 int32_t SocketThreadBase::EpollAdd(CSocket* pSocket)
 {
	 m_ClientSocket.emplace(pSocket->GetSessionIndex(), pSocket);
	 return S_OK;
 }
 bool SocketThreadBase::Init()
 {
	 return true;
 }
 void SocketThreadBase::Run()
 {
	 m_doAllTimer =  CTimerManager::GetInstance().DoTimerEvent();
	 KickSession();
	 AddConnectSocket();
 }