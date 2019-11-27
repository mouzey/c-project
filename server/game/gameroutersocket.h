#pragma once
#include<time.h>
#include "tcpclientsocket.h"
class CGameRouterSocket :public CClientSocket
{
public:
	static CSocket* CreateClient();
	CGameRouterSocket();
	virtual ~CGameRouterSocket();
	virtual bool Active(uint32_t nowSec)const override { return nowSec > m_actTime + MAXSOCKETCLOSETIME; }
	virtual int32_t EpollIn()override { m_actTime = ::time(nullptr); return CClientSocket::EpollIn(); }
	virtual void DispatchMsg(const char* pMsg, uint32_t size) override;
	virtual bool OnAccept()override;
	virtual int32_t EpollErr()override;
	virtual void SetSessionIndex(SessionID nIndex) { CClientSocket::SetSessionIndex(nIndex); SetKey(nIndex); }
private:
	time_t m_actTime;
};

