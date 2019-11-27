#pragma once
#include"serverbase.h"
#include "singleton.h"
#include "newqueue.h"
#include <list>
#include <map>
class CClientSocket;
class CAgentServer :public CServerBase,public CSingleton<CAgentServer>
{
public:
	CAgentServer();
	virtual ~CAgentServer();
	static void SigHandle(int sig);
	virtual void PushSocketEvent(EventType type, CSocket* pSocket)override;
	virtual void OnTimerEvent(const TimerDataInfo& data, const CDateTime& dt)override;
	void Start();
	void Stop() { m_run = false;}
	void SendMsgToSocket(SessionID id, const char* msg, uint32_t size);
private:
	void Run();
	void SendOfflineMsgToServer(SessionID id);
private:
	CNewQueue<EventData, 5 * 1024 * 1024> m_queueEvent;//网络事件队列
	std::list<EventData> m_listEvent;//m_queueEvent满了的时候会放入这个队列
	std::list<SessionID> m_listSession;//一次循环没有处理完等下次循环在处理的session
	std::map<SessionID, CClientSocket*> m_mapSocket;//网络连接列表
	CTimterEventImpl m_timerImpl;//网络线程使用的定时器接口
	bool m_run = true;
};

