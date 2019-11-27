#include "httproutersocket.h"
#include "httpconfig.h"
#include "socketthread.h"
#include "httpserver.h"
CHttpRouterSocket::CHttpRouterSocket()
	:CClientSocket(CHttpConfig::Instance().GetClientRecvSize(),
		CHttpConfig::Instance().GetClientSendSize())
	, m_actTime(time(nullptr))
{
}
CHttpRouterSocket::~CHttpRouterSocket()
{
}
CSocket* CHttpRouterSocket::CreateClient()
{
	return new CHttpRouterSocket();
}
bool CHttpRouterSocket::OnAccept()
{
	g_pSocketThread->AddTimerSec(TimerType::enmTimerType_SocketAct, MAXSOCKETCLOSETIME, MAXSOCKETCLOSETIME, GetSessionIndex());
	return true;
}
void CHttpRouterSocket::DispatchMsg(const char* pMsg, uint32_t size)
{
	//消息处理
	CHttpServer::Instance().RegiterRouterSocket(this);//收到注册消息的时候
}