#include "agentroutersocket.h"
#include"agentconfig.h"
#include "socketthread.h"
#include"agentserver.h"
CAgentRouterSocket::CAgentRouterSocket()
	:CClientSocket(CAgentConfig::Instance().GetRouterRecvSize(),
		CAgentConfig::Instance().GetRouterSendSize())
	, m_actTime(time(nullptr))
{
}
CAgentRouterSocket::~CAgentRouterSocket()
{
}
CSocket* CAgentRouterSocket::CreateClient()
{
	auto pSocket = &CAgentRouterSocket::GetInstance();
	pSocket->Reinit();
	return pSocket;
}
bool CAgentRouterSocket::OnAccept()
{
	g_pSocketThread->AddTimerSec(TimerType::enmTimerType_SocketAct, MAXSOCKETCLOSETIME, MAXSOCKETCLOSETIME, GetSessionIndex());
	return true;
}
int32_t CAgentRouterSocket::EpollErr()
{
	return CSocket::EpollErr();
}
void CAgentRouterSocket::DispatchMsg(const char* pMsg, uint32_t size)
{
	//处理服务器发送过来的包
	auto msgId = *(uint16_t*)(pMsg + 4);
	if (msgId == 0)//直接服务器处理的包，不是发送给客户端的包
	{
		g_pSocketThread->AddKickSession(0);
		return;
	}
	size -= sizeof(SessionID);
	auto sessionId = *(SessionID*)(pMsg + size);
	*(uint32_t*)pMsg = size;
	CAgentServer::Instance().SendMsgToSocket(sessionId, pMsg, size);
}