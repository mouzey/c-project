#pragma once
#include<cstdint>
#include<string>
#include"singleton.h"
class CAgentConfig:public CSingleton<CAgentConfig>
{
public:
	bool LoadCfg();
	uint32_t GetClientRecvSize()const {return m_clientRecvBuffSize;}
	uint32_t GetClientSendSize()const { return m_clientSendBuffSize; }
	uint32_t GetRouterRecvSize()const { return m_routerRecvBuffSize; }
	uint32_t GetRouterSendSize()const { return m_routerSendBuffSize; }
	uint32_t GetClientPort()const { return m_clientListenPort; }
	uint32_t GetRouterPort()const { return m_routerListenPort; }
	void SetPath(std::string&& path) { m_path = path; }
	const std::string& GetPath()const { return m_path;}
	bool UseSSL()const { return m_ssl; }
private:
	uint32_t m_clientRecvBuffSize = 64*1024;
	uint32_t m_clientSendBuffSize = 256 * 1024;
	uint32_t m_routerRecvBuffSize = 1024 * 1024;
	uint32_t m_routerSendBuffSize = 1024 * 1024;
	uint16_t m_clientListenPort = 28888;
	uint16_t m_routerListenPort = 26666;
	std::string m_path;
	bool m_ssl = false;
};

