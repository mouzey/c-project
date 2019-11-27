#include "routeragentsocket.h"
#include "serverbase.h"
#include "routerconfig.h"
CRouterAgentSocket::CRouterAgentSocket()
	:CClientSocket(CRouterConfig::Instance().GetAgentRecvSize(),CRouterConfig::Instance().GetAgentSendSize())
{
	Init(CRouterConfig::Instance().GetAgentIp().data(), CRouterConfig::Instance().GetAgentPort(),true);
	CClientSocket::SetSessionIndex(enmClientType_Proxy);
	g_pServerBase->AddIntervalTimer(TimerType::enmTimerType_SocketHeart, 5, 5, GetSessionIndex());//添加心跳定时器
}
CRouterAgentSocket::~CRouterAgentSocket()
{

}
 void CRouterAgentSocket::ConnectSucc(bool)
{
	
}
 void CRouterAgentSocket::DispatchMsg(const char* pMsg, uint32_t size)
 {
	 //处理agent发送过来的消息
 }
 void CRouterAgentSocket::SendHeart()
 {
	 if (!CanSendMsg())return;

 }