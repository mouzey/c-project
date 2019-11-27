#pragma once
#include<time.h>
#include "tcpclientsocket.h"
class CAgentClientSocket :public CClientSocket
{
public:
	static CSocket* CreateClient();
	CAgentClientSocket();
	virtual ~CAgentClientSocket();
	virtual bool Active(uint32_t nowSec)const override { return nowSec > m_actTime + MAXSOCKETCLOSETIME; }
	virtual int32_t EpollIn()override { m_actTime = ::time(nullptr); return CClientSocket::EpollIn(); }
	virtual void DispatchMsg(const char* pMsg, uint32_t size) override;
private:
	time_t m_actTime;
};

