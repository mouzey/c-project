#include "pch.h"
#include "configmanager.h"
#include "log.h"
 CCfgManager::~CCfgManager()
 {
	 for (auto& iter : m_holder)
		 delete iter.second;
 }
 bool CCfgManager::AddLoader(CBaseHolder* pLoader, LoadCfgOrder id , bool reload)
 { 
	 unsigned int tmp = id;
	 return m_holder.emplace((tmp << 1) | (reload ? 1 : 0), pLoader),true;
 }
bool CCfgManager::Load()
{
	for (auto& iter : m_holder)//在配置加载之前加载的数据
		if (iter.first < LoadCfgOrder_PreData && !iter.second->LoadData())
			return WRITE_ERROR_LOG("pre load data has error info=%s", iter.second->GetInfo()), false;
	for (auto& iter : m_holder)//加载配置
		if (!iter.second->LoadCfg())
			return WRITE_ERROR_LOG("load cfg has error info=%s", iter.second->GetInfo()), false;
	for (auto& iter : m_holder)//加载数据
		if (iter.first > LoadCfgOrder_PreData && !iter.second->LoadData())
			return WRITE_ERROR_LOG("load data has error info=%s", iter.second->GetInfo()), false;
	for (auto& iter : m_holder)//初始化数据
		if (!iter.second->Init())
			return WRITE_ERROR_LOG("init data has error info=%s", iter.second->GetInfo()), false;
	return true;
}
bool CCfgManager::ReLoad()
{
	for (auto& iter : m_holder)//重新加载配置
		if ((iter.first & LoadCfgOrder_Reload) > 0 && !iter.second->ReloadCfg())
			return WRITE_ERROR_LOG("reload cfg has error info=%s", iter.second->GetInfo()), false;
	for (auto& iter : m_holder)//重新初始化数据
		if ((iter.first & LoadCfgOrder_Reload) > 0 && !iter.second->Reinit())
			return WRITE_ERROR_LOG("reinit data has error info=%s", iter.second->GetInfo()), false;
	return true;
}