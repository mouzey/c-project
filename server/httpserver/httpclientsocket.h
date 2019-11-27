#pragma once
#include <time.h>
#include "tcpclientsocket.h"
class CHttpClientSocket :public CClientSocket
{
public:
	static CSocket* CreateClient();
	CHttpClientSocket();
	virtual ~CHttpClientSocket();
	virtual bool OnAccept()override;
	virtual bool SpiltMsg() override;
	virtual bool SendMsg(const char* pMsg, uint32_t size)override;
};

