#include "wsclientsocket.h"
#include "mysha1.h"
#include "socketthread.h"
#include "common_api.h"
CSocket* CWSClientSocket::CreateClient()
{
	return new CWSClientSocket();
}
CWSClientSocket::CWSClientSocket()
{
}
CWSClientSocket::~CWSClientSocket()
{
}

bool CWSClientSocket::SpiltMsg()
{
	if (!IsWSConencted())
	{
		if (!WSHandshake())
			g_pSocketThread->AddKickSession(GetSessionIndex());
		return false;
	}
	static char buff[MaxOneMsg];
	char* pBuff = nullptr;
	NetBuffer arrBuf[2];
	uint32_t nSize = 0;
	for(uint32_t nCount = 0; nCount < 50;++nCount)
	{
		if (m_pRecvBuf->GetSize() < PACKHEADSIZE)return false;
		m_pRecvBuf->GetReadEnable(arrBuf);
		if (char(0x82) != arrBuf[0].buf[0])
		{
			g_pSocketThread->AddKickSession(GetSessionIndex());
			return false;//不是数据包直接关闭socket
		}
		if (arrBuf[0].len >= 4)
			pBuff = arrBuf[0].buf;
		else
		{
			pBuff = buff;
			memcpy(pBuff, arrBuf[0].buf, arrBuf[0].len);
			memcpy(pBuff + arrBuf[0].len, arrBuf[1].buf, 4 - arrBuf[0].len);
		}
		nSize = pBuff[1] & 0x7f;
		if (127 == nSize || pBuff[1] > 0)
		{
			g_pSocketThread->AddKickSession(GetSessionIndex());
			return false; //不是数据包或者数据包太长直接关闭socket 超过64kws协议头中长度是8字节
		}
		uint32_t headLen = 2;
		if (126 == nSize)
		{
			nSize = uint8_t(pBuff[2]);
			nSize = (nSize << 8) | uint8_t(pBuff[2]);
			headLen = 4;
		}
		uint32_t nDataSize = nSize + headLen + 4;
		if (nDataSize > MaxOneMsg || nSize == 0)
		{
			g_pSocketThread->AddKickSession(GetSessionIndex());
			return false;//数据有问题直接关闭服务器
		}
		if (nDataSize > m_pRecvBuf->GetSize())return false;//数据不完整
		if (nDataSize <= arrBuf[0].len)
			pBuff = arrBuf[0].buf;
		else
		{
			pBuff = buff;
			memcpy(pBuff, arrBuf[0].buf, arrBuf[0].len);
			memcpy(pBuff + arrBuf[0].len, arrBuf[1].buf, (size_t)nDataSize - arrBuf[0].len);
		}
		const char* key = &pBuff[headLen];
#ifdef WIN32
#pragma  warning(push)
#pragma  warning(disable : 6385)
#endif
		for (uint32_t i = headLen + 4, j = 0; i < nDataSize; ++i)
		{
			pBuff[i] = pBuff[i] ^ key[j];
			j = 3 == j ? 0 : j + 1;
		}
#ifdef WIN32
#pragma  warning(pop)
#endif
		DispatchMsg(pBuff + headLen, nSize);//消息处理
		m_pRecvBuf->Remove(nDataSize);
	}
	return true;
}
bool CWSClientSocket::WSHandshake()
{
	if (m_pRecvBuf->GetSize() < PACKHEADSIZE)return true;
	NetBuffer arrBuf[2];
	m_pRecvBuf->GetReadEnable(arrBuf);
	uint32_t nContentPos = 0;
	uint32_t nContentLen = 0;
	int nRet = GetLength(arrBuf[0].buf, arrBuf[0].len, nContentPos, nContentLen);
	if (nRet == -1)return false;//不支持的协议头解析
	if (0 == nRet) return true;//数据不全
	if (memcmp(arrBuf[0].buf, "GET ", 4) != 0)return false;//协议头错误
	auto pBegin = strstr(arrBuf[0].buf, "Sec-WebSocket-Key: ");
	if (nullptr == pBegin)return false;//数据错误
	pBegin += 19;
	auto pEnd = strstr(pBegin, "\r\n");
	if (nullptr == pEnd)return false;//数据错误
	SetStatus(enmSocketStatus_WSConnected);
	std::string str("HTTP/1.1 101 Switching Protocols\r\nConnection: upgrade\r\nUpgrad: websocket\r\nSec-Websocket-Accept: ");
	uint32_t msg[5];
	CSHA1 sha;
	sha.Reset();
	sha.Input(pBegin, uint32_t(pEnd - pBegin));
	sha.Input("258EAFA5-E914-47DA-95CA-C5AB0DC85B11", 36);
	sha.Result(msg);
	for (int i = 0; i < 5; ++i)
		msg[i] = htonl(msg[i]);
	Base64Encode((char*)msg, 20, str);
	str.append("\r\n\r\n");
	SendWebMsg(str.data(), (uint32_t)str.size());
	m_pRecvBuf->Remove(nRet);
	return true;
}
void CWSClientSocket::SendWebMsg(const char* pMsg, uint32_t size)
{
	CAgentClientSocket::SendMsg(pMsg, size);
}
bool CWSClientSocket::SendMsg(const char* pMsg, uint32_t size)
{
	//假设msg前面4个字节是包长 只支持最大包长64k
	int len = size - 4;
	if (len > 0xffff)return false;
	char* msg = (char*)pMsg;
	if (len >= 126)
	{
		msg[0] = char(0x82);
		msg[1] = 126;
		msg[2] = char(len >> 8);
		msg[3] = char(0xff & len);
	}
	else
	{
		msg[2] = char(0x82);
		msg[3] = char(len);
		msg += 2;
		size -= 2;
	}
	SendWebMsg(msg, size);
	return true;
}