#pragma once
#include "socket.h"
//¼àÌýsocket
typedef CSocket* (*Factory)();
class CListenSocket :public CSocket
{
public:
	CListenSocket(Factory factory,uint32_t nPort,const char* host=nullptr);
	~CListenSocket(void);
	virtual bool IsListenSocket()const override{return true;}
	virtual int32_t EpollIn()override;
	virtual int32_t EpollErr()override;
private:
	int32_t OpenAsServer();
	CSocket* CreateNewScoket(){return m_factory();}
private:
	Factory m_factory;
};

