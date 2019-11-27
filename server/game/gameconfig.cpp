#include "gameconfig.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "log.h"
bool CGameConfig::LoadCfg()
{
	std::string filename = m_path + "serverconfig.xml";
	rapidxml::file<> file(filename.data());
	auto doc = std::make_shared<rapidxml::xml_document<>>();
	doc->parse<0>(file.data());
	auto root = doc->first_node("root");
	if (nullptr == root)
	{
		WRITE_ERROR_LOG("get root node error");
		return false;
	}
	auto tmpNode = root->first_node("Log");
	if (nullptr != tmpNode)
	{
		auto tmpAttr = tmpNode->first_attribute("level");
		if (nullptr != tmpAttr)
			g_log->SetLogLevel((LogLevel)atoi(tmpAttr->value()));
	}
	tmpNode = root->first_node("Common");
	if (nullptr == tmpNode)
	{
		WRITE_ERROR_LOG("get Common node error");
		return false;
	}
	auto tmpAttr = tmpNode->first_attribute("iscross");
	if (nullptr != tmpAttr)
		m_isCross = atoi(tmpAttr->value()) != 0;
	if (m_isCross)
	{
		tmpNode = root->first_node("CrossServer");
		if (nullptr == tmpNode)
		{
			WRITE_ERROR_LOG("get CrossServer node error");
			return false;
		}
		tmpAttr = tmpNode->first_attribute("port");
		if (nullptr == tmpAttr)
		{
			WRITE_ERROR_LOG("CrossServer node has no attribute port");
			return false;
		}
		m_gamePort = (uint16_t)atoi(tmpAttr->value());
	}
	else
	{
		tmpAttr = tmpNode->first_attribute("zoneid");
		if (nullptr == tmpAttr)
		{
			WRITE_ERROR_LOG("Common node has no zoneid port");
			return false;
		}
		m_zoneId = (uint16_t)atoi(tmpAttr->value());
		tmpNode = root->first_node("GameServer");
		if (nullptr == tmpNode)
		{
			WRITE_ERROR_LOG("get GameServer node error");
			return false;
		}
		tmpAttr = tmpNode->first_attribute("port");
		if (nullptr == tmpAttr)
		{
			WRITE_ERROR_LOG("GameServer node has no attribute port");
			return false;
		}
		m_gamePort = (uint16_t)atoi(tmpAttr->value());
		tmpNode = root->first_node("db");
		if (nullptr == tmpNode)
		{
			WRITE_ERROR_LOG("get db node error");
			return false;
		}
		tmpAttr = tmpNode->first_attribute("port");
		if (nullptr == tmpAttr)
		{
			WRITE_ERROR_LOG("db node has no port port");
			return false;
		}
		m_dbPort = (uint16_t)atoi(tmpAttr->value());
		tmpAttr = tmpNode->first_attribute("ip");
		if (nullptr == tmpAttr)
		{
			WRITE_ERROR_LOG("db node has no attribute ip");
			return false;
		}
		m_dbIp = tmpAttr->value();
		tmpAttr = tmpNode->first_attribute("user");
		if (nullptr == tmpAttr)
		{
			WRITE_ERROR_LOG("db node has no attribute user");
			return false;
		}
		m_dbUser = tmpAttr->value();
		tmpAttr = tmpNode->first_attribute("password");
		if (nullptr == tmpAttr)
		{
			WRITE_ERROR_LOG("db node has no attribute password");
			return false;
		}
		m_dbPassWord = tmpAttr->value();
		tmpAttr = tmpNode->first_attribute("tablename");
		if (nullptr == tmpAttr)
		{
			WRITE_ERROR_LOG("db node has no attribute tablename");
			return false;
		}
		m_dbTableName = tmpAttr->value();
		tmpNode = root->first_node("dbLog");
		if (nullptr == tmpNode)
		{
			WRITE_ERROR_LOG("get dbLog node error");
			return false;
		}
		tmpAttr = tmpNode->first_attribute("port");
		if (nullptr == tmpAttr)
		{
			WRITE_ERROR_LOG("dbLog node has no port port");
			return false;
		}
		m_dbLogPort = (uint16_t)atoi(tmpAttr->value());
		tmpAttr = tmpNode->first_attribute("ip");
		if (nullptr == tmpAttr)
		{
			WRITE_ERROR_LOG("dbLog node has no attribute ip");
			return false;
		}
		m_dbLogIp = tmpAttr->value();
		tmpAttr = tmpNode->first_attribute("user");
		if (nullptr == tmpAttr)
		{
			WRITE_ERROR_LOG("dbLog node has no attribute user");
			return false;
		}
		m_dbLogUser = tmpAttr->value();
		tmpAttr = tmpNode->first_attribute("password");
		if (nullptr == tmpAttr)
		{
			WRITE_ERROR_LOG("dbLog node has no attribute password");
			return false;
		}
		m_dbLogPassWord = tmpAttr->value();
		tmpAttr = tmpNode->first_attribute("tablename");
		if (nullptr == tmpAttr)
		{
			WRITE_ERROR_LOG("dbLog node has no attribute tablename");
			return false;
		}
		m_dbLogTableName = tmpAttr->value();
	}
	return true;
}
