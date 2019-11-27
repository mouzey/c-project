#include "pch.h"
#include "sslmanager.h"
#include "log.h"
#include"tcpclientsocket.h"
#include "selectthread.h"
#include "epollthread.h"
#include "mysha1.h"
#include "common_api.h"
#ifndef WIN32
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif
CSSLManager::CSSLManager()
{

}
CSSLManager::~CSSLManager()
{
	ERR_free_strings();
	SSL_CTX_free(m_ctx);
}
bool CSSLManager::InitAsServer(const char* cert, const char* key)
{
	SSL_library_init();
	SSL_load_error_strings();
	m_ctx = SSL_CTX_new(SSLv23_server_method());
	if (nullptr == m_ctx)
	{
		WRITE_ERROR_LOG("SSL_CTX_new error");
		return false;
	}
	SSL_CTX_set_verify(m_ctx, SSL_VERIFY_NONE, nullptr);
	if (1 != SSL_CTX_use_certificate_file(m_ctx, cert, SSL_FILETYPE_PEM))
	{
		WRITE_ERROR_LOG("SSL_CTX_use_certificate_file error");
		return false;
	}
	if (1 != SSL_CTX_use_certificate_chain_file(m_ctx, cert))
	{
		WRITE_ERROR_LOG("SSL_CTX_use_certificate_chain_file error");
		return false;
	}
	if (1 != SSL_CTX_use_PrivateKey_file(m_ctx, key, SSL_FILETYPE_PEM))
	{
		WRITE_ERROR_LOG("SSL_CTX_use_PrivateKey_file error");
		return false;
	}
	if (1 != SSL_CTX_check_private_key(m_ctx))
	{
		WRITE_ERROR_LOG("SSL_CTX_check_private_key error");
		return false;
	}
	return true;
}
bool CSSLManager::InitAsClient()
{
	SSL_library_init();
	SSL_load_error_strings();
	m_ctx = SSL_CTX_new(SSLv23_client_method());
	if (nullptr == m_ctx)
	{
		WRITE_ERROR_LOG("SSL_CTX_new error");
		return false;
	}
	return true;
}
bool CSSLManager::AcceptSocket(CClientSocket* pSocket)
{
	auto ssl = SSL_new(m_ctx);
	if (nullptr == ssl)
	{
		WRITE_ERROR_LOG("SSL_new error port=%u ip=%s",pSocket->GetPort(),pSocket->GetHost());
		return false;
	}
	if (1 != SSL_set_fd(ssl, (int)pSocket->GetSocket()))
	{
		WRITE_ERROR_LOG("SSL_set_fd error port=%u ip=%s", pSocket->GetPort(), pSocket->GetHost());
		return false;
	}
	SSL_set_accept_state(ssl);
	pSocket->SetSSL(ssl);
	return true;
}
bool CSSLManager::ConnectSocket(CClientSocket* pSocket)
{
	auto ssl = SSL_new(m_ctx);
	if (nullptr == ssl)
	{
		WRITE_ERROR_LOG("SSL_new error port=%u ip=%s", pSocket->GetPort(), pSocket->GetHost());
		return false;
	}
	if (1 != SSL_set_fd(ssl, (int)pSocket->GetSocket()))
	{
		WRITE_ERROR_LOG("SSL_set_fd error port=%u ip=%s", pSocket->GetPort(), pSocket->GetHost());
		return false;
	}
	SSL_set_connect_state(ssl);
	pSocket->SetSSL(ssl);
	return true;
}
bool CSSLManager::SSLHandshake(CClientSocket* pSocket)
{
	if (pSocket->IsSSLConencted())return true;
	auto nRet = SSL_do_handshake(pSocket->GetSSL());
	if (1 == nRet)
	{
		pSocket->SetStatus(enmSocketStatus_SSLConnected);
		return true;
	}
	auto nErr = SSL_get_error(pSocket->GetSSL(), nRet);
	if (nErr == SSL_ERROR_WANT_READ || nErr == SSL_ERROR_WANT_WRITE)
		return true;
	WRITE_ERROR_LOG("SSL_do_handshake error port=%u ip=%s ret=%d err=%d", pSocket->GetPort(), pSocket->GetHost(),nRet,nErr);
	return false;
}
bool CSSLManager::EpollIn(CClientSocket* pSocket)
{
	if (!pSocket->IsSSLConencted())return SSLHandshake(pSocket);
	int nCount = 0;
	int nRecvSize = 0;
	while (++nCount < 10)
	{
		if (pSocket->m_pRecvBuf->IsFull())//满了
		{
			g_pSocketThread->EpollMod(pSocket);
			return true;
		}
		NetBuffer arrBuf[2];
		pSocket->m_pRecvBuf->GetWriteEnable(arrBuf);
		int nRet = SSL_read(pSocket->GetSSL(),arrBuf[0].buf, arrBuf[0].len);
		if (nRet > 0)
		{
			pSocket->m_pRecvBuf->Add(nRet);
			nRecvSize += nRet;
		}
		else
		{
			auto nErr = SSL_get_error(pSocket->GetSSL(), nRet);
			if(nErr != SSL_ERROR_WANT_READ)
				return false;//socket 已经发生了错误
			break;
		}
	}
	if(nCount >= 10)//达到了循环次数
		g_pSocketThread->EpollMod(pSocket);
	if (nRecvSize > 0)//接收到了数据处理
		RecvData(pSocket);
	return true;
}
bool CSSLManager::EpollOut(CClientSocket* pSocket)
{
	if (!pSocket->IsSSLConencted())return SSLHandshake(pSocket);
	int nCount = 0;
	while (++nCount < 10)
	{
		if (pSocket->m_pSendBuf->IsEmpty())return true;
		NetBuffer arrBuf[2];
		pSocket->m_pSendBuf->GetReadEnable(arrBuf);
		if (pSocket->GetLastSendSize() > 0)//上次数据没有发送成功需要用相同的参数发送
		{
			int nRet = SSL_write(pSocket->GetSSL(), arrBuf[0].buf, pSocket->GetLastSendSize());
			if (nRet >0 && (uint32_t)nRet == pSocket->GetLastSendSize())
			{
				pSocket->m_pSendBuf->Remove(pSocket->GetLastSendSize());
				pSocket->SetLastSendSize(0);
				continue;
			}
			auto nErr = SSL_get_error(pSocket->GetSSL(), nRet);
			if (nErr != SSL_ERROR_WANT_WRITE)
				return false;//socket 已经发生了错误
			return true;
		}
		int nRet = SSL_write(pSocket->GetSSL(), arrBuf[0].buf,arrBuf[0].len);
		if (nRet >0 && (uint32_t)nRet != arrBuf[0].len)
		{
			auto nErr = SSL_get_error(pSocket->GetSSL(), nRet);
			if (nErr != SSL_ERROR_WANT_WRITE)
				return false;//socket 已经发生了错误
			pSocket->SetLastSendSize(arrBuf[0].len);//保存参数
			return true;
		}
		pSocket->m_pSendBuf->Remove(arrBuf[0].len);
	}
	if (nCount >= 10)//达到了循环次数
		g_pSocketThread->EpollMod(pSocket);
	return true;
}
bool CSSLManager::SendWSMsg(CClientSocket* pSocket, char* msg, uint32_t size)
{
	//假设msg前面4个字节是包长 只支持最大包长64k
	int len = size - 4;
	if (len > 0xffff)return false;
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
	if (pSocket->m_pSendBuf->GetFreeSize() < size)//如果队列不够应该加入临时缓冲
		pSocket->SendMsgToBuff(msg, size);
	else
	{
		pSocket->m_pSendBuf->AddOnlyBuf(msg, size);
		g_pSocketThread->EpollMod(pSocket);
	}
	return true;
}
bool CSSLManager::RecvWSMsg(CClientSocket* pSocket)
{
	if (pSocket->m_pRecvBuf->GetSize() < 4)return true;
	if (!pSocket->IsWSConencted())
		return WSHandshake(pSocket);
	static char buff[MaxOneMsg];
	char* pBuff = buff;
	NetBuffer arrBuf[2];
	pSocket->m_pRecvBuf->GetReadEnable(arrBuf);
	if (char(0x82) != arrBuf[0].buf[0])return false;//不是数据包直接关闭socket
	if (arrBuf[0].len >= 4)
		pBuff = arrBuf[0].buf;
	else
	{
		memcpy(pBuff, arrBuf[0].buf, arrBuf[0].len);
		memcpy(pBuff+arrBuf[0].len, arrBuf[1].buf,4 - arrBuf[0].len);
	}
	auto nSize = pBuff[1] & 0x7f;
	if (127 == nSize || pBuff[1] > 0)return false; //不是数据包或者数据包太长直接关闭socket 
	uint32_t headLen = 2;
	if (126 == nSize)
	{
		nSize = uint8_t(pBuff[2]);
		nSize = (nSize << 8) | uint8_t(pBuff[2]);
		headLen =  4;
	}
	uint32_t nDataSize = nSize + headLen + 4;
	if (nDataSize > MaxOneMsg || nSize == 0)return false;//数据有问题直接关闭服务器
	if (nDataSize > pSocket->m_pRecvBuf->GetSize())return true;//数据不完整
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
	for (uint32_t i = headLen+4, j = 0; i < nDataSize; ++i)
	{
		pBuff[i] = pBuff[i] ^ key[j];
		j = 3 == j ? 0 : j + 1;
	}
#ifdef WIN32
#pragma  warning(pop)
#endif
	//pBuff + headLen, nSize;消息地址和长度处理就可以了
	pSocket->m_pRecvBuf->Remove(nDataSize);
	return true;
}
bool CSSLManager::WSHandshake(CClientSocket* pSocket)
{
	NetBuffer arrBuf[2];
	pSocket->m_pRecvBuf->GetReadEnable(arrBuf);
	uint32_t nContentPos = 0;
	uint32_t nContentLen = 0;
	int nRet = pSocket->GetLength(arrBuf[0].buf, arrBuf[0].len, nContentPos, nContentLen);
	if (nRet == -1)return false;//不支持的协议头解析
	if (0 == nRet) return true;//数据不全
	if (memcmp(arrBuf[0].buf, "GET ", 4) != 0)return false;//协议头错误
	auto pBegin = strstr(arrBuf[0].buf, "Sec-WebSocket-Key: ");
	if (nullptr == pBegin)return false;//数据错误
	pBegin += 19;
	auto pEnd = strstr(pBegin, "\r\n");
	if (nullptr == pEnd)return false;//数据错误
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
	pSocket->m_pSendBuf->AddOnlyBuf(str.data(),(uint32_t)str.size());
	pSocket->m_pRecvBuf->Remove(nRet);
	pSocket->SetStatus(enmSocketStatus_WSConnected);
	return true;
}
void CSSLManager::WsClientConnectSucc(CClientSocket* pSocket)
{
	//c++实现websocket客户端连接成功发送握手包
	std::string msg = "GET Sec-WebSocket-Key: test\r\n\r\n";
	pSocket->m_pSendBuf->AddOnlyBuf(msg.data(), (uint32_t)msg.size());
}
void CSSLManager::WsClientSend(CClientSocket* pSocket,char* msg, uint32_t len)
{
	//c++实现websocket客户端发送普通包 如果包大于64k需要使用8字节的长度字段游戏中没必要支持超过64k(0xffff)的包
	static char buff[MaxOneMsg];
	if (len >= 126)
	{
		buff[0] = char(0x82);
		buff[1] = char(126 | 0x80);
		buff[2] = char(len >> 8);
		buff[3] = char(len & 0xff);
	}
	else
	{
		buff[2] = char(0x82);
		buff[3] = char(len |0x80);
	}
	char key[] = "1234";
	memcpy(buff + 4, key, 4);
	for (uint32_t i = 0, j = 0, k = 8; i < len;)
	{
		buff[k++] = msg[i++] ^ key[j++];
		if (4 == j)j = 0;
	}
	if(len >= 126)
		pSocket->m_pSendBuf->AddOnlyBuf(buff,len+8);
	else
		pSocket->m_pSendBuf->AddOnlyBuf(buff+2, len+6);
}
void CSSLManager::WsClientSplitMsg(CClientSocket* pSocket, char* msg, uint32_t len)
{
	//c++实现websocke客户端实现拆包
	if (len < 4)return;
	uint32_t size = 126 == msg[1] ? ((uint32_t(msg[2]) << 8) | uint32_t(msg[3])) : uint32_t(msg[1]);//限制消息报最大64k
	msg += 126 == size ? 2 : 4;
	//msg,len处理消息
}
void CSSLManager::CloseSocket(CClientSocket* pSocket)
{
	SSL_shutdown(pSocket->GetSSL());
	SSL_free(pSocket->GetSSL());
}