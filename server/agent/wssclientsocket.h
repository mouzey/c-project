#pragma once
#include "wsclientsocket.h"
class CWSSClientSocket :public CWSClientSocket
{
public:
	static CSocket* CreateClient();
	CWSSClientSocket();
	virtual ~CWSSClientSocket();
	virtual bool OnAccept()override;//接受连接
	virtual int32_t EpollIn()override { return CClientSocket::HttpsIn(); }
	virtual int32_t EpollOut()override {return CClientSocket::HttpsOut();}
	virtual SSL* GetSSL()override { return m_ssl; }
private:
	virtual uint32_t GetLastSendSize()const override{ return m_lastSendSize; }
	virtual void SetLastSendSize(uint32_t val) override { m_lastSendSize = val; }
	virtual void SendWebMsg(const char* pMsg, uint32_t size)override;
private:
	SSL* m_ssl = nullptr;
	uint32_t m_lastSendSize = 0;
};

