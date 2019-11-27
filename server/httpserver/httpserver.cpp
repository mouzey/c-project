#include "httpserver.h"
#include "tcpclientsocket.h"
#include "timermanager.h"
#include "common_api.h"
#include "epollthread.h"
#include "selectthread.h"
#include "listensocket.h"
#include"log.h"
#include"httpconfig.h"
#include"httpclientsocket.h"
#include"httproutersocket.h"
CServerBase* g_pServerBase = &CHttpServer::Instance();
CHttpServer::CHttpServer() :m_timerImpl(this)
{

}
CHttpServer::~CHttpServer()
{

}
void CHttpServer::SigHandle(int sig)
{
	CHttpServer::Instance().Stop();
}
void CHttpServer::PushSocketEvent(EventType type, CSocket* pSocket)
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
void CHttpServer::OnTimerEvent(const TimerDataInfo& data, const CDateTime& dt)
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
	{
		auto iter = m_mapSocket.find(data.data.u64);
		if (iter != m_mapSocket.end())
			iter->second->SendListMsg();
		break;
	}
	default: break;
	}
}
void CHttpServer::Start()
{
	g_pSocketThread = &CSingleton<CSocketThread>::Instance();
	new CListenSocket(CHttpClientSocket::CreateClient, CHttpConfig::Instance().GetClientPort());
	new CListenSocket(CHttpRouterSocket::CreateClient, CHttpConfig::Instance().GetRouterPort());
	g_pSocketThread->Start();
	WRITE_INFO_LOG("start http server");
	printf("start http server\n");
	Run();
	g_pSocketThread->Stop();
	g_pSocketThread->WaitExit();
	WRITE_INFO_LOG("stop http server");
	printf("stop http server\n");
}
void CHttpServer::Run()
{
	EventData event;
	bool doAllTimer = false;
	CClientSocket* pSocket = nullptr;
	uint32_t count = 0;
	while (m_run)
	{
		doAllTimer = CTimerManager::GetInstance().DoTimerEvent();
		for (count = 0; count < 50 && m_queueEvent.Pop(event) == S_OK; ++count)
		{
			pSocket = (CClientSocket*)event.data;
			switch (event.type)
			{
			case enmEventType_Remove:
			{
				m_mapSocket.erase(pSocket->GetSessionIndex());
				auto iter = m_zoneSocket.find((uint32_t)pSocket->GetKey());
				if(iter != m_zoneSocket.end() && iter->second->GetSessionIndex() == pSocket->GetSessionIndex())//之前的socket连接关闭事件可能比新的socket注册晚
					m_zoneSocket.erase(iter);
				WRITE_INFO_LOG("delete socket sessionid=%" PRIu64 " key=%" PRIu64 " port=%u ip=%s", pSocket->GetSessionIndex(),
					pSocket->GetKey(), pSocket->GetPort(), pSocket->GetHost());
				delete pSocket;
				break;
			}
			case enmEventType_Data:
				if (pSocket->SpiltMsg())
					m_listSession.emplace_back(pSocket->GetSessionIndex());
				break;
			default:break;
			}
		}
		for (count = 0; count < 50 && !m_listSession.empty(); ++count)
		{
			auto iter = m_mapSocket.find(m_listSession.front());
			if (iter != m_mapSocket.end() && iter->second->SpiltMsg())
				m_listSession.emplace_back(iter->first);
			m_listSession.pop_front();
		}
		if (doAllTimer && m_queueEvent.IsEmpty() && m_listSession.empty())
			MySleep(1);
	}
}
void CHttpServer::SendMsgToSocket(SessionID id, const char* msg, uint32_t size)
{
	auto iter = m_mapSocket.find(id);
	if (iter != m_mapSocket.end() && !iter->second->SendMsg(msg, size))
		g_pSocketThread->AddKickSession(id);
}
bool CHttpServer::SendMsgToZone(uint32_t id, const char* msg, uint32_t size)
{
	auto iter = m_zoneSocket.find(id);
	if (iter == m_zoneSocket.end())
		return false;
	if (iter->second->SendMsg(msg, size))
		return true;
	g_pSocketThread->AddKickSession(iter->second->GetSessionIndex());
	return false;
}
void CHttpServer::RegiterHttpSocket(CClientSocket* pSocket)
{
	m_mapSocket.emplace(pSocket->GetSessionIndex(), pSocket);
}
void CHttpServer::RegiterRouterSocket(CClientSocket* pSocket)
{
	RegiterHttpSocket(pSocket);
	auto iter = m_zoneSocket.find((uint32_t)pSocket->GetKey());
	if (iter != m_zoneSocket.end())
	{
		WRITE_INFO_LOG("kick socket sessionid=%" PRIu64 " key=%" PRIu64 " port=%u ip=%s",iter->second->GetSessionIndex(),
			iter->second->GetKey(),iter->second->GetPort(),iter->second->GetHost());
		g_pSocketThread->AddKickSession(iter->second->GetSessionIndex());
	}
	m_zoneSocket[(uint32_t)pSocket->GetKey()] = pSocket;
}