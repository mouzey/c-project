#include "gameserver.h"
#include "tcpclientsocket.h"
#include "timermanager.h"
#include "common_api.h"
#include "wsssocketthread.h"
#include "listensocket.h"
#include "log.h"
#include "gameconfig.h"
#include "gameroutersocket.h"
#include "mysqlmanager.h"
#include "billthread.cpp"
#include "threadevent.h"
CServerBase* g_pServerBase = &CGameServer::Instance();
CGameServer::CGameServer() :m_timerImpl(this)
{

}
CGameServer::~CGameServer()
{

}
void CGameServer::SigHandle(int sig)
{
	CGameServer::Instance().Stop();
}
void CGameServer::PushSocketEvent(EventType type, CSocket* pSocket)
{
	//这个函数在网络线程中调用
	while (!m_listEvent.empty() && S_OK == m_queueEvent.Push(m_listEvent.front()))
		m_listEvent.pop_front();
	EventData event;
	event.type = type;
	event.data = (void*)pSocket;
	if (m_queueEvent.IsFull())
		m_listEvent.emplace_back(event);
	else
		m_queueEvent.Push(event);
	if (!m_listEvent.empty() && 0 == m_timerImpl.GetTimerIdSize())
		m_timerImpl.AddIntervalTimer(TimerType::enmTimerType_AddEvent, 1000);
}
void CGameServer::OnTimerEvent(const TimerDataInfo& data, const CDateTime& dt)
{
	switch (data.type)
	{
	case TimerType::enmTimerType_AddEvent://这个在网络线程调用的
		while (!m_listEvent.empty() && S_OK == m_queueEvent.Push(m_listEvent.front()))
			m_listEvent.pop_front();
		if (!m_listEvent.empty())
			m_timerImpl.AddIntervalTimer(TimerType::enmTimerType_AddEvent, 1000);
		break;
	case TimerType::enmTimerType_SendMsg://这个在主线程调用的
		if (CGameConfig::Instance().IsCrossServer())
		{
			auto iter = m_mapSocket.find(data.data.u64);
			if (iter != m_mapSocket.end())
				iter->second->SendListMsg();
			break;
			
		}
		CSingleton<CGameRouterSocket>::GetInstance().SendListMsg();
		break;
	default: break;
	}
}
bool CGameServer::Init()
{
	if (CGameConfig::Instance().IsCrossServer())return true;
	if (S_OK != CMysqlManager::Instance().Init(CGameConfig::Instance().GetDbIp(), CGameConfig::Instance().GetDbUser(),
		CGameConfig::Instance().GetDbPassWord(),CGameConfig::Instance().GetDbTable(), CGameConfig::Instance().GetDbPort(), 0))
	{
		WRITE_ERROR_LOG("conenct db has error");
		return false;
	}
	if (S_OK != CBillThread::Instance().Init(CGameConfig::Instance().GetDbLogIp(), CGameConfig::Instance().GetDbLogUser(),
		CGameConfig::Instance().GetDbLogPassWord(),CGameConfig::Instance().GetDbLogTable(), CGameConfig::Instance().GetDbLogPort(), 0))
	{
		WRITE_ERROR_LOG("conenct db log has error");
		return false;
	}
	return true;
}
void CGameServer::Start()
{
	if (CGameConfig::Instance().UseSSL())
		g_pSocketThread = &CSingleton<WSSSocketThread>::Instance();
	else
		g_pSocketThread = &CSingleton<CSocketThread>::Instance();
	new CListenSocket(CGameRouterSocket::CreateClient, CGameConfig::Instance().GetGamePort());
	g_pSocketThread->Start();
	CMysqlManager::Instance().Start();
	CBillThread::Instance().Start();
	WRITE_INFO_LOG("start game server");
	printf("start http server\n");
	Run();
	g_pSocketThread->Stop();
	CMysqlManager::Instance().Stop();
	CBillThread::Instance().Stop();
	g_pSocketThread->WaitExit();
	CMysqlManager::Instance().WaitExit();
	CBillThread::Instance().WaitExit();
	WRITE_INFO_LOG("stop game server");
	printf("stop game server\n");
}
void CGameServer::Run()
{
	EventData event;
	bool doAllTimer = false;
	CClientSocket* pSocket = nullptr;
	CFuncEventBase* pEvent = nullptr;
	uint32_t count = 0;
	while (m_run)
	{
		doAllTimer = CTimerManager::GetInstance().DoTimerEvent();
		for (count = 0; count < 50 && CMysqlManager::Instance().PopCallBack(pEvent); ++count)
		{
			pEvent->CallFunc();
			delete pEvent;
		}
		for (count = 0; count < 50 && CSingleton<CThreadEvent>::Instance().Pop(pEvent) == S_OK; ++count)
		{
			pEvent->CallFunc();
			delete pEvent;
		}
		for (count = 0; count < 50 && m_queueEvent.Pop(event) == S_OK; ++count)
		{
			pSocket = (CClientSocket*)event.data;
			if (enmEventType_Data == event.type)
			{
				if (pSocket->SpiltMsg())
					m_listSession.emplace_back(pSocket->GetKey());
			}
			else if (enmEventType_Remove == event.type)
			{
				auto iter = m_mapSocket.find(pSocket->GetKey());
				if (iter != m_mapSocket.end() && pSocket->GetSessionIndex() == iter->second->GetSessionIndex())
					m_mapSocket.erase(iter);
				WRITE_INFO_LOG("delete socket sessionid=%" PRIu64 " key=%" PRIu64 " port=%u ip=%s", pSocket->GetSessionIndex(),
					pSocket->GetKey(), pSocket->GetPort(), pSocket->GetHost());
				delete pSocket;
			}
			else if (enmEventType_HttpErr == event.type)
				pSocket->Error();
			else//enmEventType_Accept == event.type
				RegiterRouterSocket(pSocket);
		}
		for (count = 0; count < 50 && !m_listSession.empty(); ++count)
		{
			auto iter = m_mapSocket.find(m_listSession.front());
			if (iter != m_mapSocket.end() && iter->second->SpiltMsg())
				m_listSession.emplace_back(iter->first);
			m_listSession.pop_front();
		}
		if (doAllTimer && m_queueEvent.IsEmpty() && CMysqlManager::Instance().CallBackEmpty()
			&& m_listSession.empty() && CSingleton<CThreadEvent>::Instance().IsEmpty())
			MySleep(1);
	}
}

void CGameServer::RegiterRouterSocket(CClientSocket* pSocket)
{
	auto iter = m_mapSocket.find(pSocket->GetKey());
	if (iter != m_mapSocket.end())
	{
		WRITE_INFO_LOG("kick socket sessionid=%" PRIu64 " key=%" PRIu64 " port=%u ip=%s", iter->second->GetSessionIndex(),
			iter->second->GetKey(), iter->second->GetPort(), iter->second->GetHost());
		g_pSocketThread->AddKickSession(iter->second->GetSessionIndex());
	}
	m_mapSocket[pSocket->GetKey()] = pSocket;
}
 void CGameServer::SendMsg(const char* msg, uint32_t size)
{
	 if (CGameConfig::Instance().IsCrossServer())
	 {
		 WRITE_ERROR_LOG("cross can not call this function");
		 KillSelf();
	 }
	 else if(!CSingleton<CGameRouterSocket>::Instance().SendMsg(msg, size))
		 g_pSocketThread->AddKickSession(CSingleton<CGameRouterSocket>::Instance().GetSessionIndex());
}

 void CGameServer::SendMsgByZone(const char* msg, uint32_t size, uint32_t serverId)
{
	 auto iter = m_mapSocket.find(serverId);
	 if (iter != m_mapSocket.end() && !iter->second->SendMsg(msg, size))
		 g_pSocketThread->AddKickSession(iter->second->GetSessionIndex());
}
 void CGameServer::SendMsgByRole(const char* msg, uint32_t size, uint64_t roleId)
{
	 if (CGameConfig::Instance().IsCrossServer())
		 SendMsgByZone(msg, size, GetZoneId(roleId));
	 else if(!CSingleton<CGameRouterSocket>::Instance().SendMsg(msg, size))
		 g_pSocketThread->AddKickSession(CSingleton<CGameRouterSocket>::Instance().GetSessionIndex());
}
 uint32_t CGameServer::GetZoneId(uint64_t roleId)
 {
	 auto iter = m_roleZone.find(roleId);
	 return iter != m_roleZone.end() ? iter->second : 0;
 }
 uint64_t CGameServer::GetRoleId(uint64_t sessionId)
 {
	 auto iter = m_roleSession.find(sessionId);
	 return iter != m_roleSession.end() ? iter->second : 0;
 }