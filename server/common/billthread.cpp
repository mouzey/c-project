#include "pch.h"
#include "billthread.h"
#include "log.h"
#include "common_api.h"
CBillThread::CBillThread():CThread(enmThreadType_Bill)
{
	m_Conn = nullptr;
	m_cross = false;
}
CBillThread::~CBillThread()
{
	for (auto&iter:m_ConnMap)
		mysql_close(iter.second->conn);
	m_ConnMap.clear();
}
int32_t CBillThread::Init(const std::string& host,const std::string& user,const std::string& password,
						  const std::string& dbName,uint16_t port,ServerID serverId/*=0*/)
{
	for (auto&iter:m_ConnMap)
	{
		if (iter.first == serverId)
			return S_OK;
		if(iter.second->host == host && iter.second->dbName == dbName && iter.second->port == port)
		{
			m_ConnMap.emplace(serverId,iter.second);
			return S_OK;
		}
	}
	auto pInfo = std::make_shared<SqlConnInfo>();
	pInfo->conn = nullptr;
	pInfo->host = host;
	pInfo->dbName = dbName;
	pInfo->password = password;
	pInfo->port = port;
	pInfo->user = user;
	auto iter = m_ConnMap.emplace(serverId,pInfo);
	if (iter.second)
		return Connect(iter.first->second);
	else
		WRITE_WARING_LOG("this zone has same config serverid=%u",serverId);
	return S_OK;
}
int32_t CBillThread::Connect(std::shared_ptr<SqlConnInfo>& pInf)
{
	uint32_t num = 0;
	while (true)
	{
		++num;
		if(num > 60)//测试60次
		{
			m_Conn = nullptr;
			return E_UNKNOW;
		}
		m_Conn = mysql_init(nullptr);
		if(nullptr == m_Conn)
		{
			WRITE_ERROR_LOG("mysql_init error");
			return E_UNKNOW;
		}
		char reconnect = 1;
		if(mysql_options(m_Conn, MYSQL_OPT_RECONNECT,&reconnect))
		{
			WRITE_ERROR_LOG("mysql_options  MYSQL_OPT_RECONNECT error errno=%d errstr=%s",
				mysql_errno(m_Conn),mysql_error(m_Conn));
			mysql_close(m_Conn);
			return E_UNKNOW;
		}
		if(mysql_real_connect(m_Conn,pInf->host.data(),pInf->user.data(),
			pInf->password.data(),pInf->dbName.data(),pInf->port,nullptr,0) == nullptr)
		{
			WRITE_ERROR_LOG("mysql_real_connect error errno=%d errstr=%s",
				mysql_errno(m_Conn),mysql_error(m_Conn));
			mysql_close(m_Conn);
			MySleep(1000);
			continue;
		}
		if(mysql_set_character_set(m_Conn,"utf8"))
		{
			WRITE_ERROR_LOG("mysql_set_character_set error errno=%d errstr=%s",
				mysql_errno(m_Conn),mysql_error(m_Conn));
			mysql_close(m_Conn);
			return E_UNKNOW;
		}
		if(nullptr != pInf->conn)
			mysql_close(pInf->conn);
		pInf->conn = m_Conn;
		break;
	}
	return S_OK;
}
void CBillThread::Run()
{
	if (m_cross)
		CrossRun();
	else
		LocalRun();
}
void CBillThread::LocalRun()
{
	char szBuf[MaxOneLogSize];
	uint32_t nSize;
	while (IsRun())
	{
		if(nullptr == m_Conn && Connect(m_ConnMap.begin()->second)!= S_OK)
			KillSelf();
		while(!m_sql.IsEmpty())
		{
			char *pSql = szBuf;
			bool bDel = m_sql.GetBuf(pSql,nSize,MaxOneLogSize,nullptr);
			if (mysql_real_query(m_Conn,pSql,nSize))
			{
				WRITE_ERROR_LOG("mysql_real_query has error  errno=%d errstr=%s sql=%.*s",
					mysql_errno(m_Conn),mysql_error(m_Conn),nSize,pSql);
				//一般不会出现用重新连接来确认是网络问题还是sql问题
				if (Connect(m_ConnMap.begin()->second) != S_OK)
				{
					WRITE_ERROR_LOG("dump begin\n%.*s\ndump end", nSize, pSql);
					KillSelf();
				}
				//连接成功后在来一次不行就放弃了
				else if (mysql_real_query(m_Conn,pSql,nSize))
				{
					WRITE_ERROR_LOG("mysql_real_query has error  errno=%d errstr=%s sql=%.*s",
						mysql_errno(m_Conn),mysql_error(m_Conn),nSize,pSql);
				}
			}
			if(bDel) m_sql.Remove(nSize+(sizeof(uint32_t)));
		}
		MySleep(1);
	}
	while (!m_sql.IsEmpty())//退出的时候把日志打印出来
	{
		char *pSql = szBuf;
		bool bDel = m_sql.GetBuf(pSql,nSize,MaxOneLogSize,nullptr);
		if (m_Conn == nullptr || mysql_real_query(m_Conn,pSql,nSize))
		{
			WRITE_ERROR_LOG("mysql_real_query has error  errno=%d errstr=%s sql=%.*s",
				mysql_errno(m_Conn),mysql_error(m_Conn),nSize,pSql);
		}
		if(bDel) m_sql.Remove(nSize+sizeof nSize);
	}
}
void CBillThread::CrossRun()
{
	char szBuf[MaxOneLogSize];
	uint32_t nSize;
	ServerID serverId = 0;
	auto iter = m_ConnMap.begin();
	ServerID oldServerId=iter->first;
	while (IsRun())
	{
		while(!m_sql.IsEmpty())
		{
			char *pSql = szBuf;
			bool bDel = m_sql.GetBuf(pSql,nSize,MaxOneLogSize,&serverId);
			if(serverId != oldServerId)
			{
				iter = m_ConnMap.find(serverId);
				oldServerId = serverId;
			}
			if (unlikely(iter == m_ConnMap.end()))
			{
				WRITE_ERROR_LOG("can not find conn sql=%.*s serverid=%u",nSize,pSql,serverId);
				oldServerId = ServerID(-1);
			}
			else if (mysql_real_query(iter->second->conn,pSql,nSize))
			{
				WRITE_ERROR_LOG("mysql_real_query has error  errno=%d errstr=%s sql=%.*s",
					mysql_errno(iter->second->conn),mysql_error(iter->second->conn),nSize,pSql);
				//一般不会出现用重新连接来确认是网络问题还是sql问题失败直接强制关了
				if (Connect(iter->second) != S_OK)
				{
					WRITE_ERROR_LOG("dump begin\n%.*s\ndump end", nSize, pSql);
					KillSelf();
				}
				//连接成功后在来一次不行就放弃了
				else if (mysql_real_query(iter->second->conn,pSql,nSize))
				{
					WRITE_ERROR_LOG("mysql_real_query has error  errno=%d errstr=%s sql=%.*s",
						mysql_errno(iter->second->conn),mysql_error(iter->second->conn),nSize,pSql);
				}
			}
			if(bDel) m_sql.Remove(nSize+sizeof nSize + sizeof serverId);
		}
		MySleep(1);
	}
	while (!m_sql.IsEmpty())
	{
		char *pSql = szBuf;
		bool bDel = m_sql.GetBuf(pSql,nSize,MaxOneLogSize,&serverId);
		if(serverId != oldServerId)
		{
			iter = m_ConnMap.find(serverId);
			oldServerId = serverId;
		}
		if (unlikely(iter == m_ConnMap.end()))
		{
			WRITE_ERROR_LOG("can not find conn sql=%.*s serverid=%u",nSize,pSql,serverId);
			oldServerId = ServerID(-1);
		}
		else if (mysql_real_query(iter->second->conn,pSql,nSize))
		{
			WRITE_ERROR_LOG("mysql_real_query has error  errno=%d errstr=%s sql=%.*s",
				mysql_errno(iter->second->conn),mysql_error(iter->second->conn),nSize,pSql);
		}
		if(bDel) m_sql.Remove(nSize+sizeof nSize + sizeof serverId);
	}
}
void CBillThread::PostBill(const char*pSql,uint32_t nSize,ServerID serverId/*=0*/)
{
	if (nSize > MaxOneLogSize)
	{
		WRITE_ERROR_LOG("bill sql is to long max=%u",MaxOneLogSize);
		return ;
	}
	while (unlikely(!m_list.empty()))//出现的可能比较小
	{
		auto iter = m_list.begin();
		if(iter->size()+ 4 > m_sql.GetFreeSize())
			break;
		m_sql.AddBuf(iter->data(),(uint32_t)iter->size());
		m_list.pop_front();
	}
	if (m_cross)
	{
		if (m_sql.GetFreeSize() < nSize + sizeof(ServerID) + 4)
		{
			std::string str((const char*)&serverId,sizeof(ServerID));
			str.append(pSql,nSize);
			m_list.emplace_back(std::move(str));
			WRITE_INFO_LOG("mysql may slow");
		}
		else
			m_sql.AddBuf(pSql,nSize,serverId);
	}
	else if (m_sql.GetFreeSize() < nSize+4)
	{
		m_list.emplace_back(pSql,nSize);
		WRITE_INFO_LOG("mysql may slow");
	}
	else
		m_sql.AddBuf(pSql,nSize);
}
void CBillThread::PostBill(CBillBase* pBill,ServerID serverId/*=0*/)
{
	char szBuf[MaxOneLogSize];
	int32_t nSize = pBill->GetSql(szBuf,MaxOneLogSize);
	if (nSize > 0)
		PostBill(szBuf,(uint32_t)nSize,serverId);
	else
		WRITE_ERROR_LOG("get bill sql has error ret=%d",nSize);
}
void CBillThread::PostBill(CBillBase* pBill,const CRole& role,ServerID serverId/*=0*/)
{
	if(m_cross && 0 == serverId)
		serverId = GetServerID(0);
	char szBuf[MaxOneLogSize];
	int32_t nSize = pBill->GetSql(szBuf,MaxOneLogSize,role);
	if (nSize > 0)
		PostBill(szBuf,(uint32_t)nSize,serverId);
	else
		WRITE_ERROR_LOG("get bill sql has error ret=%d",nSize);
}