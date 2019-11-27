#include "httpconfig.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "log.h"
bool CHttpConfig::LoadCfg()
{
	std::string filename = m_path + "serverconfig.xml";
	rapidxml::file<> file(filename.data());
	rapidxml::xml_document<> doc;
	doc.parse<0>(file.data());
	auto root = doc.first_node("root");
	if (nullptr == root)
	{
		WRITE_ERROR_LOG("get root node error");
		return false;
	}
	auto tmpNode = root->first_node("HttpClient");
	if (nullptr == tmpNode)
	{
		WRITE_ERROR_LOG("get HttpClient node error");
		return false;
	}
	auto tmpAttr = tmpNode->first_attribute("port");
	if (nullptr == tmpAttr)
	{
		WRITE_ERROR_LOG("HttpClient node has no attribute port");
		return false;
	}
	m_clientListenPort = (uint16_t)atoi(tmpAttr->value());
	tmpNode = root->first_node("HttpServer");
	if (nullptr == tmpNode)
	{
		WRITE_ERROR_LOG("get HttpServer node error");
		return false;
	}
	tmpAttr = tmpNode->first_attribute("port");
	if (nullptr == tmpAttr)
	{
		WRITE_ERROR_LOG("HttpServer node has no attribute port");
		return false;
	}
	m_routerListenPort = (uint16_t)atoi(tmpAttr->value());
	tmpNode = root->first_node("Log");
	if (nullptr != tmpNode)
	{
		tmpAttr = tmpNode->first_attribute("level");
		if (nullptr != tmpAttr)
			g_log->SetLogLevel((LogLevel)atoi(tmpAttr->value()));
	}
	return true;
}
