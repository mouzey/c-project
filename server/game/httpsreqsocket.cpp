#include "httpsreqsocket.h"
#include "common_api.h"
#include "socketthread.h"
#include "log.h"
#ifndef WIN32
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif
CHttpsReqSocket::CHttpsReqSocket()
{

}

CHttpsReqSocket::CHttpsReqSocket(uint32_t recvSize, uint32_t sendSize, uint16_t port):CHttpReqSocket(recvSize,sendSize,port)
{

}
CHttpsReqSocket::~CHttpsReqSocket()
{
	if (m_ssl != nullptr)
	{
		SSL_shutdown(m_ssl);
		SSL_free(m_ssl);
	}
}
int32_t CHttpsReqSocket::EpollOut()
{
	if (!IsConnected() && S_OK != ConnectSucc())
		return E_UNKNOW;
	return CClientSocket::HttpsOut();
}

int32_t CHttpsReqSocket::EpollIn()
{
	if (!IsConnected() && S_OK != ConnectSucc())
		return E_UNKNOW;
	return CClientSocket::HttpsIn();
}

int32_t CHttpsReqSocket::ConnectSucc()
{
	m_ssl = g_pSocketThread->GetNewSSL();
	if (nullptr == m_ssl)
	{
		WRITE_ERROR_LOG("SSL_new error port=%u ip=%s", GetPort(),GetHost());
		return E_UNKNOW;
	}
	if (1 != SSL_set_fd(m_ssl, (int)GetSocket()))
	{
		WRITE_ERROR_LOG("SSL_set_fd error port=%u ip=%s", GetPort(), GetHost());
		return E_UNKNOW;
	}
	SSL_set_connect_state(m_ssl);
	SetStatus(enmSocketStatus_Connected);
	g_pSocketThread->AddTimerSec(TimerType::enmTimerType_SocketClose, 60, 0, GetSessionIndex());//一分钟后就关闭
	return S_OK;
}
