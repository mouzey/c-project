#pragma once
#include<map>
#include<list>
#include "newqueue.h"
#include "serverbase.h"
#include "singleton.h"
class CClientSocket;
class CHttpServer :public CServerBase, public CSingleton<CHttpServer>
{
public:
	CHttpServer();
	virtual ~CHttpServer();
	static void SigHandle(int sig);
	virtual void PushSocketEvent(EventType type, CSocket* pSocket)override;
	virtual void OnTimerEvent(const TimerDataInfo& data, const CDateTime& dt)override;
	void Start();
	void Stop() { m_run = false; }
	void SendMsgToSocket(SessionID id, const char* msg, uint32_t size);//http返回发送函数
	bool SendMsgToZone(uint32_t id, const char* msg, uint32_t size);//发送到服务器函数
	void RegiterHttpSocket(CClientSocket* pSocket);
	void RegiterRouterSocket(CClientSocket* pSocket);
private:
	void Run();
private:
	CNewQueue<EventData, 5 * 1024 * 1024> m_queueEvent;//网络事件队列
	std::list<EventData> m_listEvent;//m_queueEvent满了的时候会放入这个队列
	std::list<SessionID> m_listSession;//一次循环没有处理完等下次循环在处理的session
	std::map<SessionID, CClientSocket*> m_mapSocket;//网络连接列表
	std::map<uint32_t, CClientSocket*> m_zoneSocket;//服务器连接列表
	CTimterEventImpl m_timerImpl;//网络线程使用的定时器接口
	bool m_run = true;
};

