#ifndef _SERVER_BASE_H_
#define _SERVER_BASE_H_
#include"define.h"
#include "timerbase.h"
class CSocket;
class CServerBase: public CTimerBase
{
public:
	CServerBase();
	virtual ~CServerBase();
public:
	virtual void PushSocketEvent(EventType type, CSocket* pSocket) {}
	virtual void SendMsg(const char* msg, uint32_t size) {}
	virtual void SendMsgByZone(const char* msg, uint32_t size,uint32_t serverId) {}
	virtual void SendMsgByRole(const char* msg, uint32_t size, uint64_t roleId) {}
	virtual void RegisterZone(uint64_t roleId, uint32_t serverId) {}
	virtual void RegisterRole(uint64_t roleId, uint64_t sessionId) {}
};
extern CServerBase* g_pServerBase;
#endif