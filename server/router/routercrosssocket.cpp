#include "routercrosssocket.h"
#include "serverbase.h"
#include "routerconfig.h"
CRouterCrossSocket::CRouterCrossSocket()
	:CClientSocket(CRouterConfig::Instance().GetCrossRecvSize(), CRouterConfig::Instance().GetCrossSendSize())
{
	Init(CRouterConfig::Instance().GetCrossIp().data(), CRouterConfig::Instance().GetCrossPort(), true);
	CClientSocket::SetSessionIndex(enmClientType_Cross);
	g_pServerBase->AddIntervalTimer(TimerType::enmTimerType_SocketHeart, 5, 5, GetSessionIndex());//添加心跳定时器
}
CRouterCrossSocket::~CRouterCrossSocket()
{

}
void CRouterCrossSocket::ConnectSucc(bool)
{
	//发送注册信息
}
void CRouterCrossSocket::DispatchMsg(const char* pMsg, uint32_t size)
{
	//处理跨服发送过来的消息
}
void CRouterCrossSocket::SendHeart()
{
	if (!CanSendMsg())return;
}