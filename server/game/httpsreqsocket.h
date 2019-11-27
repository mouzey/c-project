#pragma once
#include "httpreqsocket.h"
class CHttpsReqSocket :public CHttpReqSocket
{
public:
	CHttpsReqSocket();
	CHttpsReqSocket(uint32_t recvSize, uint32_t sendSize, uint16_t port);
	virtual ~CHttpsReqSocket();
	virtual int32_t EpollOut()override;
	virtual int32_t EpollIn()override;
	virtual int32_t ConnectSucc()override;
	virtual SSL* GetSSL()override { return m_ssl; }
private:
	SSL* m_ssl = nullptr;
};

