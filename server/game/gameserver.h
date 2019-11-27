#pragma once
#include<map>
#include<unordered_map>
#include<list>
#include "serverbase.h"
#include "newqueue.h"
#include "singleton.h"
class CClientSocket;
class CGameServer :public CServerBase,public CSingleton<CGameServer>
{
public:
	CGameServer();
	virtual ~CGameServer();
	static void SigHandle(int sig);
	virtual void PushSocketEvent(EventType type, CSocket* pSocket)override;
	virtual void OnTimerEvent(const TimerDataInfo& data, const CDateTime& dt)override;
	void Start();
	void Stop() { m_run = false; }
	void RegiterRouterSocket(CClientSocket* pSocket);
	bool Init();
	virtual void SendMsg(const char* msg, uint32_t size)override;//本服发送接口
	virtual void SendMsgByZone(const char* msg, uint32_t size, uint32_t serverId)override;//跨服发送接口
	virtual void SendMsgByRole(const char* msg, uint32_t size, uint64_t roleId)override;//通用发送接口
	uint32_t GetZoneId(uint64_t roleId);
	uint64_t GetRoleId(uint64_t sessionId);
	virtual void RegisterZone(uint64_t roleId, uint32_t serverId) { m_roleZone[roleId] = serverId; }
	virtual void RegisterRole(uint64_t roleId, uint64_t sessionId) { m_roleSession[sessionId] = roleId; }
private:
	void Run();
private:
	CNewQueue<EventData, 5 * 1024 * 1024> m_queueEvent;//网络事件队列
	std::list<EventData> m_listEvent;//m_queueEvent满了的时候会放入这个队列
	std::list<SessionID> m_listSession;//一次循环没有处理完等下次循环在处理的session
	std::map<SessionID, CClientSocket*> m_mapSocket;//网络连接列表
	CTimterEventImpl m_timerImpl;//网络线程使用的定时器接口
	std::unordered_map<uint64_t, uint32_t> m_roleZone;
	std::unordered_map<uint64_t, uint64_t> m_roleSession;
	bool m_run = true;
};

