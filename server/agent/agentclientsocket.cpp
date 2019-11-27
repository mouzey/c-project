#include "agentclientsocket.h"
#include "agentconfig.h"
#include "agentroutersocket.h"
#include "socketthread.h"
CAgentClientSocket::CAgentClientSocket()
	:CClientSocket(CAgentConfig::Instance().GetClientRecvSize(), 
		CAgentConfig::Instance().GetClientSendSize())
	,m_actTime(time(nullptr))
{
}
CAgentClientSocket::~CAgentClientSocket()
{
}
CSocket* CAgentClientSocket::CreateClient()
{
	return new CAgentClientSocket();
}
void CAgentClientSocket::DispatchMsg(const char* pMsg, uint32_t size)
{
	//处理客户端发送过来的包
	NetBuffer arrBuf[2];
	arrBuf[0].buf = (char*)pMsg;
	arrBuf[0].len = size;
	arrBuf[1].buf = (char*)&m_nSeesionIndex;
	arrBuf[1].len = sizeof(m_nSeesionIndex);
	*(uint32_t*)arrBuf[0].buf = size + arrBuf[1].len;
	if (!CAgentRouterSocket::GetInstance().SendMsg(arrBuf))
		g_pSocketThread->AddKickSession(CAgentRouterSocket::GetInstance().GetSessionIndex());
}