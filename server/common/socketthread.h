#pragma once
#include <map>
#include <list>
#include "thread.h"
#include "timerbase.h"
#include"singleton.h"
#include "newqueue.h"
class CSocket;
class CClientSocket;
class SocketThreadBase :public CThread,public CTimerBase
{
public:
	SocketThreadBase();
	virtual ~SocketThreadBase();
	virtual int32_t EpollAdd(CSocket* pSocket);
	virtual int32_t EpollMod(CSocket *pSocket) { return S_OK; }
	virtual void OnTimerEvent(const TimerDataInfo& data, const CDateTime& dt)override;
	void AddConnectSocket(CClientSocket* pSocket);
	void AddKickSession(SessionID id);
	SessionID GetSessionID() { return ++m_nSeesionIndex; }
	virtual SSL* GetNewSSL() { return nullptr; }
protected:
	virtual int32_t EpollDelete(const SOCKET fd) { return S_OK; }
	virtual bool ErrorSocket(CSocket *pSocket);
	virtual bool Init();
	void virtual Run() override;
	void AddConnectSocket();
	void KickSession();
	bool CanSleep()const {return  m_doAllTimer && m_waitConnect.IsEmpty() && m_kickSession.IsEmpty();}
protected:
	std::map<SessionID, CSocket*> m_ClientSocket;
private:
	SessionID m_nSeesionIndex;
	CTimterEventImpl m_impl;//给主线程使用的定时器接口
	bool m_doAllTimer = false;

	CNewQueue<CClientSocket*> m_waitConnect;//等待连接的socket主线程push 网络线程pop
	std::list<CClientSocket*> m_connectList;

	CNewQueue<SessionID> m_kickSession;//主线程踢掉
	std::list<SessionID> m_kickList;
};
extern SocketThreadBase* g_pSocketThread;