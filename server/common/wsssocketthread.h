#pragma once
#include <string>
#include "selectthread.h"
#include "epollthread.h"
class WSSSocketThread :public CSocketThread
{
public:
	WSSSocketThread();
	virtual~WSSSocketThread();
	virtual bool Init()override;
	virtual SSL* GetNewSSL()override;
private:
	SSL_CTX* m_ctx = nullptr;
	std::string m_cert;
	std::string m_key;
	bool m_clientMethod = false;
};

