#pragma once
#include<cstdint>
#include<string>
#include"singleton.h"
class CGameConfig :public CSingleton<CGameConfig>
{
public:
	bool LoadCfg();
	void SetPath(std::string&& path) { m_path = path; }
	const std::string& GetPath()const { return m_path; }

	uint16_t GetGamePort()const { return m_gamePort; }
	uint32_t GetGameRecvSize()const { return m_gameRecvSize; }
	uint32_t GetGameSendSize()const { return m_gameSendSize; }
	bool IsCrossServer()const { return m_isCross; }
	const std::string GetDbIp()const { return m_dbIp; }
	const std::string GetDbUser()const { return m_dbUser; }
	const std::string GetDbPassWord()const { return m_dbPassWord; }
	const std::string GetDbTable()const { return m_dbTableName; }
	uint16_t GetDbPort()const { return m_dbPort; }
	const std::string GetDbLogIp()const { return m_dbLogIp; }
	const std::string GetDbLogUser()const { return m_dbLogUser; }
	const std::string GetDbLogPassWord()const { return m_dbLogPassWord; }
	const std::string GetDbLogTable()const { return m_dbLogTableName; }
	uint16_t GetDbLogPort()const { return m_dbLogPort; }
	bool UseSSL()const { return m_ssl; }
private:
	bool m_ssl = false;
	bool m_isCross = false;
	std::string m_path;
	uint32_t m_gameRecvSize = 1024 * 1024;
	uint32_t m_gameSendSize = 1024 * 1024;
	uint16_t m_gamePort = 27777;
	uint16_t m_zoneId = 1;
	std::string m_dbIp;
	std::string m_dbUser;
	std::string m_dbPassWord;
	std::string m_dbTableName;
	uint16_t m_dbPort = 3306;
	uint16_t m_dbLogPort = 3306;
	std::string m_dbLogIp;
	std::string m_dbLogUser;
	std::string m_dbLogPassWord;
	std::string m_dbLogTableName;
};

