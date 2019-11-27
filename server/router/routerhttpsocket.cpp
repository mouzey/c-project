#include "routerhttpsocket.h"
#include "serverbase.h"
#include "routerconfig.h"
CRouterHttpSocket::CRouterHttpSocket()
	:CClientSocket(CRouterConfig::Instance().GetHttpRecvSize(), CRouterConfig::Instance().GetHttpSendSize())
{
	Init(CRouterConfig::Instance().GetHttpIp().data(), CRouterConfig::Instance().GetHttpPort(), true);
	CClientSocket::SetSessionIndex(enmClientType_Http);
	g_pServerBase->AddIntervalTimer(TimerType::enmTimerType_SocketHeart, 5, 5, GetSessionIndex());
}
CRouterHttpSocket::~CRouterHttpSocket()
{

}
void CRouterHttpSocket::ConnectSucc(bool)
{
	//发送注册信息
}
void CRouterHttpSocket::DispatchMsg(const char* pMsg, uint32_t size)
{
	//处理http服务器发送过来的消息
}
 void CRouterHttpSocket::SendHeart()
{
	 if (!CanSendMsg())return;
}