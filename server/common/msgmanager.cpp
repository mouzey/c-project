#include "pch.h"
#include "msgmanager.h"
#include "log.h"
#include "codebase.h"
CMsgManager *g_pMsgMgr = &(CMsgManager::Instance());
CMsgManager::CMsgManager()
{
}
CMsgManager::~CMsgManager()
{
}
void CMsgManager::OnMsgEvent(const char* pBuff, uint32_t nSize, uint64_t key)
{
	uint16_t nMsgID = *(uint16_t*)(pBuff+4);
	auto iter = m_handle.find(nMsgID);
	if (iter == m_handle.end())
	{
		WRITE_ERROR_LOG("has no this msg id=%u key=%" PRIu64,nMsgID,key);
		return;
	}
	if (!CCode::Decode(pBuff, nSize, iter->second->pMsg))
	{
		WRITE_ERROR_LOG("decode msg has error id=%u key=%" PRIu64,nMsgID,key);
		return;
	}
	if (iter->second->checkRole)
	{
		iter->second->DoMsg(nullptr);
	}
	else
	{
		iter->second->DoMsg(key);
	}
}