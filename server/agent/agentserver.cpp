#include "agentserver.h"
#include "tcpclientsocket.h"
#include "timermanager.h"
#include "common_api.h"
#include"agentroutersocket.h"
#include "wsssocketthread.h"
#include "agentclientsocket.h"
#include "listensocket.h"
#include "agentconfig.h"
#include"log.h"
CServerBase* g_pServerBase =  &CAgentServer::Instance();
CAgentServer::CAgentServer():m_timerImpl(this)
{

}
 CAgentServer::~CAgentServer()
{

}
void CAgentServer::SigHandle(int sig)
{
	CAgentServer::Instance().Stop();
}
void CAgentServer::PushSocketEvent(EventType type, CSocket* pSocket)
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
 void CAgentServer::OnTimerEvent(const TimerDataInfo& data, const CDateTime& dt)
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
		 else
			 CAgentRouterSocket::GetInstance().SendListMsg();
		 break;
	 }
	 default: break;
	 }
}
 void CAgentServer::Start()
 {
	 if(CAgentConfig::Instance().UseSSL())
		 g_pSocketThread = &CSingleton<WSSSocketThread>::Instance();
	 else
		 g_pSocketThread = &CSingleton<CSocketThread>::Instance();
	 new CListenSocket(CAgentClientSocket::CreateClient, CAgentConfig::Instance().GetClientPort());
	 new CListenSocket(CAgentClientSocket::CreateClient, CAgentConfig::Instance().GetRouterPort());
	 g_pSocketThread->Start();
	 WRITE_INFO_LOG("start agent server");
	 printf("start agent server\n");
	 Run();
	 g_pSocketThread->Stop();
	 g_pSocketThread->WaitExit();
	 CAgentRouterSocket::SwapInstance(nullptr);//在网络线程可能析构 避免生成core文件特殊处理，但是这样有可能在退出的时候提示内存泄露
	 WRITE_INFO_LOG("stop agent server");
	 printf("stop agent server\n");
 }
 void CAgentServer::Run()
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
			 case enmEventType_Accept:
				 m_mapSocket.emplace(pSocket->GetSessionIndex(), pSocket); break;
			 case enmEventType_Remove:
				 m_mapSocket.erase(pSocket->GetSessionIndex());
				 SendOfflineMsgToServer(pSocket->GetSessionIndex());
				 delete pSocket;
				 break;
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
 void CAgentServer::SendOfflineMsgToServer(SessionID id)
 {
	 //网络断开连接通知服务器下线 CSingleton<CAgentRouterSocket>::GetInstance().SendMsg();
 }
 void CAgentServer::SendMsgToSocket(SessionID id, const char* msg, uint32_t size)
 {
	 auto iter = m_mapSocket.find(id);
	 if (iter != m_mapSocket.end() && !iter->second->SendMsg(msg, size))
		 g_pSocketThread->AddKickSession(id);
 }