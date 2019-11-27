#pragma once
#include<time.h>
#include "tcpclientsocket.h"
#include "singleton.h"
class CAgentRouterSocket :public CClientSocket,public CSingleton<CAgentRouterSocket>
{
public:
	virtual ~CAgentRouterSocket();
	CAgentRouterSocket();
	static CSocket* CreateClient();
	virtual bool Active(uint32_t nowSec)const override { return nowSec > m_actTime + MAXSOCKETCLOSETIME; }
	virtual int32_t EpollIn()override { m_actTime = ::time(nullptr); return CClientSocket::EpollIn(); }
	virtual bool OnAccept()override;
	virtual int32_t EpollErr()override;
	virtual void DispatchMsg(const char* pMsg, uint32_t size)override;
private:
	time_t m_actTime;
};

