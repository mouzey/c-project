#pragma once
#include<cstdint>
#include<string>
#include"singleton.h"
class CRouterConfig:public CSingleton<CRouterConfig>
{
public:
	bool LoadCfg();
	void SetPath(std::string&& path) { m_path = path; }
	const std::string& GetPath()const { return m_path; }

	const std::string& GetAgentIp()const { return m_agentIp; }
	uint16_t GetAgentPort()const { return m_agentPort; }
	uint32_t GetAgentRecvSize()const { return m_agentRecvSize; }
	uint32_t GetAgentSendSize()const { return m_agentSendSize; }

	const std::string& GetHttpIp()const { return m_httpServerIp; }
	uint16_t GetHttpPort()const { return m_httpServerPort; }
	uint32_t GetHttpRecvSize()const { return m_httpServerRecvSize; }
	uint32_t GetHttpSendSize()const { return m_httpServerSendSize; }

	const std::string& GetGameIp()const { return m_gameIp; }
	uint16_t GetGamePort()const { return m_gamePort; }
	uint32_t GetGameRecvSize()const { return m_gameRecvSize; }
	uint32_t GetGameSendSize()const { return m_gameSendSize; }

	const std::string& GetCrossIp()const { return m_crossIp; }
	uint16_t GetCrossPort()const { return m_crossPort; }
	uint32_t GetCrossRecvSize()const { return m_crossRecvSize; }
	uint32_t GetCrossSendSize()const { return m_crossSendSize; }
private:
	std::string m_agentIp;
	uint32_t m_agentRecvSize = 1024 * 1024;
	uint32_t m_agentSendSize = 1024 * 1024;
	uint16_t m_agentPort = 26666;

	uint16_t m_httpServerPort = 25555;
	std::string m_httpServerIp;
	uint32_t m_httpServerRecvSize = 1024 * 1024;
	uint32_t m_httpServerSendSize = 1024 * 1024;

	std::string m_gameIp;
	uint32_t m_gameRecvSize = 1024 * 1024;
	uint32_t m_gameSendSize = 1024 * 1024;
	uint16_t m_gamePort = 27777;

	uint16_t m_crossPort = 29999;
	std::string m_crossIp;
	uint32_t m_crossRecvSize = 1024 * 1024;
	uint32_t m_crossSendSize = 1024 * 1024;

	std::string m_path;
	uint16_t m_zoneId = 1;

	uint16_t m_dbPort = 3306;
	std::string m_dbIp;
	std::string m_dbUser;
	std::string m_dbPassWord;
	std::string m_dbTableName;

	uint16_t m_dbLogPort = 3306;
	std::string m_dbLogIp;
	std::string m_dbLogUser;
	std::string m_dbLogPassWord;
	std::string m_dbLogTableName;
};

