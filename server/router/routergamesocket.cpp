#include "routergamesocket.h"
#include "serverbase.h"
#include "routerconfig.h"
CRouterGameSocket::CRouterGameSocket()
	:CClientSocket(CRouterConfig::Instance().GetGameRecvSize(), CRouterConfig::Instance().GetGameSendSize())
{
	Init(CRouterConfig::Instance().GetGameIp().data(), CRouterConfig::Instance().GetGamePort(), true);
	CClientSocket::SetSessionIndex(enmClientType_Server);
	g_pServerBase->AddIntervalTimer(TimerType::enmTimerType_SocketHeart, 5, 5, GetSessionIndex());//添加心跳定时器
}
CRouterGameSocket::~CRouterGameSocket()
{

}
void CRouterGameSocket::ConnectSucc(bool)
{
	
}
void CRouterGameSocket::DispatchMsg(const char* pMsg, uint32_t size)
{
	//处理本服发送过来的消息
}
void CRouterGameSocket::SendHeart()
{
	if (!CanSendMsg())return;
}