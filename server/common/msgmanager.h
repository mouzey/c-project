#ifndef _MSGMANAGER_H_
#define _MSGMANAGER_H_
#include "msgbase.h"
#include "singleton.h"
#include <unordered_map>
//消息管理器
class CRole;
class CMsgCallBase
{
public:
	CMsgBase* pMsg;
	bool checkRole;
	bool checkCross;
	CMsgCallBase(CMsgBase* msg, bool role, bool cross)
		:pMsg(msg), checkRole(role), checkCross(cross) {}
	virtual~CMsgCallBase() { delete pMsg; }
	virtual void DoMsg(uint64_t key) {}
	virtual void DoMsg(CRole* ptr) {}
};
class CMsgManager:public CSingleton<CMsgManager>
{
public:
	CMsgManager();
	~CMsgManager();
	void AddMsgBody(uint16_t nMsgID, CMsgCallBase *pHandle) { m_handle[nMsgID] = pHandle; }
	void OnMsgEvent(const char* pBuff,uint32_t nSize,uint64_t key);
private:
	std::unordered_map<uint16_t, CMsgCallBase*>m_handle;
};
extern CMsgManager *g_pMsgMgr;
#endif