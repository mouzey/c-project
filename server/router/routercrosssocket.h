#pragma once
#include "tcpclientsocket.h"
#include "singleton.h"
class CRouterCrossSocket :public CClientSocket, public CSingleton<CRouterCrossSocket>
{
public:
	CRouterCrossSocket();
	virtual ~CRouterCrossSocket();
	virtual void SetSessionIndex(SessionID nIndex)override { }
	virtual bool Active(uint32_t nowSec)const override { return true; }
	virtual bool CanSendMsg()const override { return IsConnected() && CClientSocket::CanSendMsg(); }
	virtual void ConnectSucc(bool)override;
	virtual void DispatchMsg(const char* pMsg, uint32_t size)override;
	virtual void ConnectClose()override { Reinit(); }
	virtual void SendHeart()override;
private:
};

