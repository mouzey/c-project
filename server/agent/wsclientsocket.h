#pragma once
#include "agentclientsocket.h"
class CWSClientSocket :public CAgentClientSocket
{
public:
	static CSocket* CreateClient();
	CWSClientSocket();
	virtual ~CWSClientSocket();
	virtual bool SpiltMsg()override;
	virtual bool SendMsg(const char* pMsg, uint32_t size)override;
	virtual void SendWebMsg(const char* pMsg, uint32_t size);
	virtual bool CanSendMsg()const { return IsWSConencted(); }
private:
	bool WSHandshake();
};

