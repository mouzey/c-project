#include "pch.h"
#include "mysqlevent.h"
#include "mysqlmanager.h"
#include "functionevent.h"
CMysqlEventBase::CMysqlEventBase(const char* pszSql, const uint32_t nSize, ServerID id)
{
	if (pszSql != nullptr)
	{
		m_szSql = new char[nSize +size_t( 1)];
		memcpy(m_szSql, pszSql, nSize);
		m_szSql[nSize] = '\0';
	}
	else
		m_szSql = nullptr;
	m_nSize = nSize;
	m_serverId = id;
}
CMysqlEventBase::CMysqlEventBase() :m_szSql(nullptr), m_nSize(0), m_serverId(0)
{
}
CMysqlEventBase::~CMysqlEventBase()
{
	if (nullptr != m_szSql)
	{
		delete[] m_szSql;
		m_szSql = nullptr;
	}
}
void CMysqlEventBase::PushEvent(CFuncEventBase* pEvent, CMysqlManager* pManager)
{
	pManager->PushCallBack(pEvent);
}
CMysqlEventEmpty::CMysqlEventEmpty()
{
	m_pServerId = nullptr;
}
CMysqlEventEmpty::~CMysqlEventEmpty()
{
	if (m_pServerId)
		delete m_pServerId;
}
void CMysqlEventCommon::CallBack(CMysqlManager* pManager, bool succ)
{
	pManager->PushCallBack(new CFuncMysql(m_pCallBack, succ ? pManager->GetAffectRow() : 0, succ));
}
void CMysqlEventComData::CallBack(CMysqlManager* pManager, bool succ)
{
	pManager->PushCallBack(new CFuncMysqlData(m_pCallBack, succ ? pManager->GetAffectRow() : 0, m_data, succ));
}
void CMysqlEventInsert::CallBack(CMysqlManager* pManager, bool succ)
{
	if (succ)
		pManager->PushCallBack(new CMysqlInsert(m_pCallBack, pManager->GetAffectRow(), pManager->GetInsertID(), true));
	else
		pManager->PushCallBack(new CMysqlInsert(m_pCallBack, 0, 0, false));
}
void CMysqlEventInsertData::CallBack(CMysqlManager* pManager, bool succ)
{
	if (succ)
		pManager->PushCallBack(new CMysqlInsertData(m_pCallBack, pManager->GetAffectRow(), pManager->GetInsertID(), m_data, true));
	else
		pManager->PushCallBack(new CMysqlInsertData(m_pCallBack, 0, 0, m_data, false));
}
void CMysqlEventSelectEx::CallBack(CMysqlManager* pManager, bool succ)
{
	if (succ)
	{
		std::shared_ptr<CMysqlRecord> record = pManager->CreateRecord(m_nCase);
		record->SaveRecord(pManager);
		pManager->PushCallBack(new CMysqlSelect(m_pCallBack, &record, true));
	}
	else
		pManager->PushCallBack(new CMysqlSelect(m_pCallBack, nullptr, false));
}
void CMysqlEventSelectDataEx::CallBack(CMysqlManager* pManager, bool succ)
{
	if (succ)
	{
		std::shared_ptr<CMysqlRecord> record = pManager->CreateRecord(m_nCase);
		record->SaveRecord(pManager);
		pManager->PushCallBack(new CMysqlSelectData(m_pCallBack, &record, m_data, true));
	}
	else
		pManager->PushCallBack(new CMysqlSelectData(m_pCallBack, nullptr, m_data, false));
}