#include "httpreqsocket.h"
#include "socketthread.h"
#include "snprintf.h"
#include "common_api.h"
#include "serverbase.h"
#ifndef WIN32
#include <netdb.h>
#endif
CHttpReqSocket::CHttpReqSocket():CClientSocket(1024,1024)
{

}

CHttpReqSocket::CHttpReqSocket(uint32_t recvSize, uint32_t sendSize, uint16_t port) : CClientSocket(recvSize, sendSize)
{
	SetPort(port);
}

CHttpReqSocket::~CHttpReqSocket()
{

}
bool CHttpReqSocket::SpiltMsg()
{
	if (m_pRecvBuf->GetSize() < PACKHEADSIZE)return false;
	NetBuffer arrBuf[2];
	m_pRecvBuf->GetReadEnable(arrBuf);
	uint32_t nContentPos = 0;
	uint32_t nContentLen = 0;
	auto nRet = GetLength(arrBuf[0].buf, arrBuf[0].len, nContentPos, nContentLen);
	if (-1 == nRet)
	{
		g_pSocketThread->AddKickSession(GetSessionIndex());
		return false;
	}
	if (0 == nRet)return false;
	auto retCode = GetRespCode(arrBuf[0].buf);
	if (retCode != 200)
		Error();//http请求错误
	//处理http返回消息
	m_pRecvBuf->Remove(nRet);
	return false;
}

int32_t CHttpReqSocket::EpollOut()
{
	if (!IsConnected())
		ConnectSucc();
	return CClientSocket::EpollOut();
}

int32_t CHttpReqSocket::EpollErr()
{
	if (m_pSendBuf->IsEmpty())return CClientSocket::EpollErr();
	while (!m_listIp.empty())
	{
		SetHost(m_listIp.front().data());
		m_listIp.pop_front();
		if (OpenAsClient() == S_OK)
		{
			g_pSocketThread->EpollAdd(this);
			return S_OK;
		}
	}
	g_pServerBase->PushSocketEvent(enmEventType_HttpErr, this);
	return CClientSocket::EpollErr();
}

int32_t CHttpReqSocket::ConnectSucc()
{
	SetStatus(enmSocketStatus_Connected);
	g_pSocketThread->AddTimerSec(TimerType::enmTimerType_SocketClose, 60, 0, GetSessionIndex());//一分钟后就关闭
	return S_OK;
}

bool CHttpReqSocket::ParaseIp(const std::string& ip)
{
	auto pHost = ::gethostbyname(ip.data());
	if (nullptr == pHost)return false;
	in_addr addr;
	for (int i = 0; pHost->h_addr_list[i] != nullptr; ++i)
	{
		memcpy(&(addr.s_addr), pHost->h_addr_list[i], pHost->h_length);
		auto tmpIp = ::inet_ntoa(addr);
		if (nullptr == tmpIp)return false;
		if (0 == i)
			SetHost(tmpIp);
		else
			m_listIp.emplace_back(tmpIp);
	}
	return true;
}

bool CHttpReqSocket::SetData(bool post, bool json, const std::string& ip, const std::string& page, const std::string& data)
{
	NetBuffer arrBuf[2];
	m_pSendBuf->GetWriteEnable(arrBuf);
	char szPort[8];
	if (80 != GetPort())
		ToStr(GetPort(), szPort, 8);
	else
		szPort[0] = '\0';
	int32_t nRet = 0;
	//Connection: close/Keep-Alive 如果close那么会在收到数据的时候同时获取到对方读挂起事件EPOLL_RDHUP
	//x-www-form-urlencoded为key1=1&key2=2
	if (post)
	{
		nRet = SNPRINTF(arrBuf[0].buf, arrBuf[0].len,
			"POST %s HTTP/1.1\r\nHost: %s%s\r\nContent-type: application/%s; charset=UTF-8\r\nContent-Length: %u\r\nConnection: close\r\n\r\n%s",
			page.data(), ip.data(), szPort,json ?  "json" :"x-www-form-urlencoded", (uint32_t)data.size(), data.data());
		
	}
	else//get需要urlencode编码的
	{
		nRet = SNPRINTF(arrBuf[0].buf, arrBuf[0].len,
			"GET %s?%s HTTP/1.1\r\nHost: %s%s\r\nConnection: close\r\nAccept: */*\r\n\r\n",
			page.data(),data.data(),ip.data(),szPort);
	}
	if (nRet < 0 || (uint32_t)nRet >= arrBuf[0].len)
		return false;
	m_pSendBuf->Add(nRet);
	return true;
}

int32_t CHttpReqSocket::GetRespCode(const char* pMsg)
{
	auto pStart = strchr(pMsg, ' ');
	if (pStart == nullptr)return 0;
	return atoi(pStart);
}
