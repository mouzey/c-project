#include "httpclientsocket.h"
#include "socketthread.h"
#include "httpconfig.h"
#include "httpserver.h"
#include "common_api.h"
#include "snprintf.h"
CHttpClientSocket::CHttpClientSocket()
	:CClientSocket(CHttpConfig::Instance().GetClientRecvSize(),
		CHttpConfig::Instance().GetClientSendSize())
{
}
CHttpClientSocket::~CHttpClientSocket()
{
}
CSocket* CHttpClientSocket::CreateClient()
{
	return new CHttpClientSocket();
}
bool CHttpClientSocket::OnAccept()
{
	g_pSocketThread->AddTimerSec(TimerType::enmTimerType_SocketClose, 60, 0, GetSessionIndex());//一分钟后就关闭
	return true;
}

bool CHttpClientSocket::SpiltMsg()
{
	//处理服务器发送过来的包
	if (m_pRecvBuf->GetSize() < PACKHEADSIZE)return false;
	NetBuffer arrBuf[2];
	m_pRecvBuf->GetReadEnable(arrBuf);
	uint32_t nContentPos = 0;
	uint32_t nContentLen = 0;
	auto nRet = GetLength(arrBuf[0].buf,arrBuf[0].len,nContentPos,nContentLen);
	if (-1 == nRet)
	{
		g_pSocketThread->AddKickSession(GetSessionIndex());
		return false;
	}
	if (0 == nRet)return false;
	//if (domsg(arrBuf[0].buf + nContentPos, nContentLen))
		CHttpServer::Instance().RegiterHttpSocket(this);
	m_pRecvBuf->Remove(nRet);//直接全部删除
	return false;
}

bool CHttpClientSocket::SendMsg(const char* pMsg, uint32_t size)
{
	
	if (size + 256 > m_pSendBuf->GetFreeSize())
	{
		std::string str;
		str.reserve(size +size_t( 256));
		str.append("HTTP/1.1 200 OK\r\nContent-Length: ");
		str.append(ToStr(size));
		str.append("\r\nContent-Type: application/json; charset=UTF-8\r\nConnection: close\r\n\r\n");//Connection: Keep-Alive
		str.append(pMsg, size);
		return CClientSocket::SendMsg(str.data(), (uint32_t)str.size());
	}
	NetBuffer arrBuf[2];
	m_pSendBuf->GetWriteEnable(arrBuf);
	int32_t nRet = SNPRINTF(arrBuf[0].buf, arrBuf[0].len,
		"HTTP/1.1 200 OK\r\nContent-Length: %u\r\nContent-Type: application/json; charset=UTF-8\r\nConnection: close\r\n\r\n",size);
	memcpy(arrBuf[0].buf + nRet, pMsg, size);
	nRet += size;
	m_pSendBuf->Add(size);
	g_pSocketThread->EpollMod(this);
	return true;
}