#include "gameroutersocket.h"
#include "gameconfig.h"
#include "socketthread.h"
#include "singleton.h"
#include "msgmanager.h"
CGameRouterSocket::CGameRouterSocket()
	:CClientSocket(CGameConfig::Instance().GetGameRecvSize(),
		CGameConfig::Instance().GetGameSendSize())
	, m_actTime(time(nullptr))
{
}
CGameRouterSocket::~CGameRouterSocket()
{
}
CSocket* CGameRouterSocket::CreateClient()
{
	if (CGameConfig::Instance().IsCrossServer())
		return new CGameRouterSocket;
	auto pSocket = &CSingleton<CGameRouterSocket>::GetInstance();
	pSocket->Reinit();
	return pSocket;
}
bool CGameRouterSocket::OnAccept()
{
	if (CGameConfig::Instance().IsCrossServer())
		CClientSocket::OnAccept();
	else
		g_pSocketThread->AddTimerSec(TimerType::enmTimerType_SocketAct, MAXSOCKETCLOSETIME, MAXSOCKETCLOSETIME, GetSessionIndex());
	return true;
}
int32_t CGameRouterSocket::EpollErr()
{
	if (CGameConfig::Instance().IsCrossServer())
		return CClientSocket::EpollErr();
	return CSocket::EpollErr();
}
void CGameRouterSocket::DispatchMsg(const char* pMsg, uint32_t size)
{
	//处理消息包 对于服务器之前的消息直接用sessionid作为 key 前端发送或者gm发送的消息直接用消息中的key
	CMsgManager::Instance().OnMsgEvent(pMsg, size,GetKey());
 }