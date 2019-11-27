#include "wssclientsocket.h"
#include "common_api.h"
#include "log.h"
#include "socketthread.h"
#ifndef WIN32
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif
CSocket* CWSSClientSocket::CreateClient()
{
	return new CWSSClientSocket();
}

CWSSClientSocket::CWSSClientSocket()
{
}

CWSSClientSocket::~CWSSClientSocket()
{
	if (m_ssl != nullptr)
	{
		SSL_shutdown(m_ssl);
		SSL_free(m_ssl);
	}
}

bool CWSSClientSocket::OnAccept()
{
	m_ssl = g_pSocketThread->GetNewSSL();
	if (nullptr == m_ssl)
	{
		WRITE_ERROR_LOG("SSL_new error port=%u ip=%s",GetPort(),GetHost());
		return false;
	}
	if (1 != SSL_set_fd(m_ssl, (int)GetSocket()))
	{
		WRITE_ERROR_LOG("SSL_set_fd error port=%u ip=%s",GetPort(), GetHost());
		return false;
	}
	SSL_set_accept_state(m_ssl);
	return true;
}

void CWSSClientSocket::SendWebMsg(const char* pMsg, uint32_t size)
{
	if (!CanSendMsg())
	{
		WRITE_WARING_LOG("can not send msg socketid=%u sessionid=%" PRIu64 " msgid=%04x size=%u",
			GetSocket(), GetSessionIndex(), *(uint16_t*)(pMsg + 4), size);//假设协议头为4字节长度加2字节协议id
		return;
	}
	CClientSocket::SendMsgToBuff(pMsg, size);
}
