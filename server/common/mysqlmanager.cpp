#include "pch.h"
#include "mysqlmanager.h"
#include "log.h"
#include "common_api.h"
CMysqlManager::CMysqlManager():CThread(enmThreadType_Sql)
{
	m_MaxPopSize = 128*1024;
	m_pPopSql = new char[uint64_t(m_MaxPopSize)+1];
	m_MaxEscapeSize = 64*1024;
	m_pEscapeSql = new char[2* uint64_t(m_MaxEscapeSize)+1];
	m_mysqlConn = nullptr;
	m_EscapeStr = nullptr;
	m_res = nullptr;
	m_length = nullptr;
	m_forceKill = false;
	m_cross = false;
	m_numFields = 0;
	m_numRow = 0;
	m_row = nullptr;
	memset(buildSql, 0, sizeof buildSql);
}
CMysqlManager::~CMysqlManager()
{
	for(auto& iter : m_ConnMap)
		mysql_close(iter.second.conn);
	if (nullptr != m_EscapeStr)
	{
		mysql_close(m_EscapeStr);
		m_EscapeStr = nullptr;
	}
	mysql_library_end();//mysql_library_init函数会在mysql_init中自动调用但是是非线程安全的
	delete []m_pPopSql;
	delete []m_pEscapeSql;
}
int32_t CMysqlManager::Init(const std::string& host,const std::string& user,const std::string& password,
							const std::string& dbName,uint16_t port,ServerID serverId)
{
	if (nullptr == m_EscapeStr)
	{
		m_EscapeStr = mysql_init(nullptr);
		if(nullptr == m_EscapeStr)
		{
			WRITE_ERROR_LOG("mysql_init return null");
			return E_NULLPOINTER;
		}
		if(mysql_set_character_set(m_EscapeStr,"utf8"))
		{
			WRITE_ERROR_LOG("mysql_set_character_set error errno=%d errstr=%s",
				mysql_errno(m_EscapeStr),mysql_error(m_EscapeStr));
			mysql_close(m_EscapeStr);
			return E_UNKNOW;
		}
	}
	for (auto&iter:m_ConnMap)
	{
		if (iter.first == serverId && iter.second.host == host && 
			iter.second.dbName == dbName && iter.second.port == port)
			return S_OK;
		if(iter.first == serverId||(iter.second.dbName == dbName && 
			iter.second.port == port && iter.second.host == host ))
		{
			WRITE_ERROR_LOG("init db has eror serverid=%u host=%s db=%s port=%u",
				serverId,host.data(),dbName.data(),port);
			return E_UNKNOW;
		}
	}
	SqlConnInfo stInfo;
	stInfo.conn = nullptr;
	stInfo.dbName = dbName;
	stInfo.host = host;
	stInfo.password = password;
	stInfo.port = port;
	stInfo.user = user;
	auto iter = m_ConnMap.emplace(serverId,stInfo);
	if(iter.second)
		return Connect(iter.first->second);
	return S_OK;
}
void CMysqlManager::PostSql(char* pSql,uint32_t nSize,ServerID id/*=0*/)
{
	if (!m_emptyEvent.IsFull() && m_sql.AddOnlyBuf(pSql,nSize) == S_OK)
	{
		m_emptyEvent.Push(nSize,id);
		PostSql(&m_emptyEvent);
	}
	else
		PostSql(new CMysqlEventBase(pSql,nSize,id));
}
void CMysqlManager::PostSql(CMysqlCallBack* pCallBack,char* pSql,uint32_t nSize,ServerID id/*=0*/)
{
	if (m_sql.AddOnlyBuf(pSql,nSize) == S_OK)
		PostSql(new CMysqlEventCommon(nullptr,nSize,pCallBack,id));
	else
		PostSql(new CMysqlEventCommon(pSql,nSize,pCallBack,id));
}
void CMysqlManager::PostSql(CMysqlCallBack* pCallBack,char* pSql,uint32_t nSize,FuncData data,ServerID id/*=0*/)
{
	if (m_sql.AddOnlyBuf(pSql,nSize) == S_OK)
		PostSql(new CMysqlEventComData(nullptr,nSize,data,pCallBack,id));
	else
		PostSql(new CMysqlEventComData(pSql,nSize,data,pCallBack,id));
}
void CMysqlManager::PostInsert(CMysqlCallBack* pCallBack,char* pSql,uint32_t nSize,ServerID id/*=0*/)
{
	if (m_sql.AddOnlyBuf(pSql,nSize) == S_OK)
		PostSql(new CMysqlEventInsert(nullptr,nSize,pCallBack,id));
	else
		PostSql(new CMysqlEventInsert(pSql,nSize,pCallBack,id));
}
void CMysqlManager::PostInsert(CMysqlCallBack* pCallBack,char* pSql,uint32_t nSize,FuncData data,ServerID id/*=0*/)
{
	if (m_sql.AddOnlyBuf(pSql,nSize) == S_OK)
		PostSql(new CMysqlEventInsertData(nullptr,nSize,data,pCallBack,id));
	else
		PostSql(new CMysqlEventInsertData(pSql,nSize,data,pCallBack,id));
}
void CMysqlManager::PostSql(CMysqlCallBack* pCallBack,char* pSql,uint32_t nSize,uint32_t nCase,ServerID id/*=0*/)
{
	if (m_sql.AddOnlyBuf(pSql,nSize) == S_OK)
		PostSql(new CMysqlEventSelectEx(nullptr,nSize,pCallBack,nCase,id));
	else
		PostSql(new CMysqlEventSelectEx(pSql,nSize,pCallBack,nCase,id));
}
void CMysqlManager::PostSql(CMysqlCallBack* pCallBack,char* pSql,uint32_t nSize,FuncData data,uint32_t nCase,ServerID id/*=0*/)
{
	if (m_sql.AddOnlyBuf(pSql,nSize) == S_OK)
		PostSql(new CMysqlEventSelectDataEx(nullptr,nSize,pCallBack,data,nCase,id));
	else
		PostSql(new CMysqlEventSelectDataEx(pSql,nSize,pCallBack,data,nCase,id));
}
uint32_t CMysqlManager::GetAffectRow()
{
	return static_cast<uint32_t>(mysql_affected_rows(m_mysqlConn));
}
uint64_t CMysqlManager::GetInsertID()
{
	return static_cast<uint64_t>(mysql_insert_id(m_mysqlConn));
}
std::shared_ptr<CMysqlRecord> CMysqlManager::CreateRecord(uint32_t nCase)
{
	return nullptr;
}
void CMysqlManager::PushCallBack(CFuncEventBase* pEvent)
{
	if(unlikely(nullptr == pEvent))
	{
		WRITE_ERROR_LOG("null pointer");
		return ;
	}
	if (unlikely(m_funEvent.IsFull() || !m_listFun.empty()))//感觉用定时器更靠谱啊 但是需要在数据线程调用CTimerManager::GetInstance().DoTimerEvent();
		m_listFun.push_back(pEvent);
	else
		m_funEvent.Push(pEvent);
}
void CMysqlManager::PostSql(CMysqlEventBase* pEvent)
{
	if(unlikely(nullptr == pEvent))
	{
		WRITE_ERROR_LOG("null pointer");
		return ;
	}
	if(unlikely(m_sqlEvent.IsFull() || !m_listSql.empty()))
	{
		if(m_listSql.empty())
			AddIntervalTimer(TimerType::enmTimerType_SqlEvent, 1000/TIMERBASE);
		m_listSql.emplace_back(pEvent);
	}
	else
		m_sqlEvent.Push(pEvent);
}
void CMysqlManager::OnTimerEvent(const TimerDataInfo& data,const CDateTime& dt)
{
	while (!m_sqlEvent.IsFull() && !m_listSql.empty())
	{
		m_sqlEvent.Push(m_listSql.front());
		m_listSql.pop_front();
	}
	if(!m_listSql.empty())
		AddIntervalTimer(TimerType::enmTimerType_SqlEvent, 1000 / TIMERBASE);
}
void CMysqlManager::ExcSql()
{
	if(m_forceKill)//因为数据库连接不上强制杀掉
	{
		DumpSql();
		return;
	}
	CMysqlEventBase* pEvent = nullptr;
	char *pSql = nullptr;
	while(!m_sqlEvent.IsEmpty())
	{
		CheckSqlEvent();
		m_sqlEvent.Pop(pEvent);
		pSql = pEvent->GetSql();
		pEvent->PopSize();
		bool bDel = false;
		if (pSql == nullptr)
		{
			if (unlikely(pEvent->GetSize() > m_MaxPopSize))
			{
				delete[] m_pPopSql;
				m_MaxPopSize = pEvent->GetSize();
				m_pPopSql = new char[uint64_t(m_MaxPopSize)+1];
			}
			pSql = m_pPopSql;
			bDel=m_sql.GetSqlBuf(pSql,pEvent->GetSize());
		}
		if (mysql_real_query(m_mysqlConn,pSql,pEvent->GetSize()))
		{
			WRITE_ERROR_LOG("mysql_real_query has error  errno=%d errstr=%s sql=%.*s",
				mysql_errno(m_mysqlConn),mysql_error(m_mysqlConn),pEvent->GetSize(), pSql);
			//一般不会出现用重新连接来确认是网络问题还是sql问题失败直接强制关了
			if (Connect(m_ConnMap.begin()->second)!= S_OK)
			{
				WRITE_INFO_LOG("dump begin\n%.*s\ndump end",pEvent->GetSize(), pSql);
				m_forceKill = true;
				if(bDel)m_sql.Remove(pEvent->GetSize());
				pEvent->Delete();
				KillSelf();
				return;
			}
			//连接成功后在来一次不行就放弃了
			if (mysql_real_query(m_mysqlConn,pSql,pEvent->GetSize()))
			{
				WRITE_ERROR_LOG("mysql_real_query has error  errno=%d errstr=%s sql=%.*s",
					mysql_errno(m_mysqlConn),mysql_error(m_mysqlConn),pEvent->GetSize(), pSql); 
				pEvent->CallBack(this,false);
				if(bDel)m_sql.Remove(pEvent->GetSize());
				pEvent->Delete();
				continue;
			}
		}
		if (pEvent->StoreRet())
		{
			m_res = mysql_store_result(m_mysqlConn);
			if(nullptr != m_res)
			{
				m_numFields = mysql_num_fields(m_res);
				m_numRow = static_cast<uint32_t>(mysql_num_rows(m_res));
			}
			else if (mysql_errno(m_mysqlConn))
			{
				WRITE_ERROR_LOG("mysql_store_result has error  errno=%d errstr=%s sql=%.*s",
					mysql_errno(m_mysqlConn),mysql_error(m_mysqlConn),pEvent->GetSize(),pSql);
				pEvent->CallBack(this,false);
				if(bDel)m_sql.Remove(pEvent->GetSize());
				pEvent->Delete();
				continue;
			}
			else//sql语句本身没有返回结果集的
				WRITE_WARING_LOG("mysql has no result chech your code sql=%.*s",pEvent->GetSize(), pSql);
		}
		pEvent->CallBack(this,true);
		if(bDel)m_sql.Remove(pEvent->GetSize());
		pEvent->Delete();
		if (m_res != nullptr)
		{
			mysql_free_result(m_res);
			m_res = nullptr;
		}
	}//while
	CheckSqlEvent();
}
void CMysqlManager::ExcSqlCross()
{
	if(m_forceKill)//因为数据库连接不上强制杀掉
	{
		DumpSqlCross();
		return;
	}
	CMysqlEventBase* pEvent = nullptr;
	char *pSql = nullptr;
	auto iter = m_ConnMap.begin();
	ServerID oldServerId=iter->first;
	while(!m_sqlEvent.IsEmpty())
	{
		CheckSqlEvent();
		m_sqlEvent.Pop(pEvent);
		pSql = pEvent->GetSql();
		pEvent->PopSizeEx();
		bool bDel = false;
		if (pSql == nullptr)
		{
			if (unlikely(pEvent->GetSize() > m_MaxPopSize))
			{
				delete[] m_pPopSql;
				m_MaxPopSize = pEvent->GetSize();
				m_pPopSql = new char[uint64_t(m_MaxPopSize)+1];
			}
			pSql = m_pPopSql;
			bDel = m_sql.GetSqlBuf(pSql,pEvent->GetSize());			
		}
		if(pEvent->GetServerId() != oldServerId)
		{
			iter = m_ConnMap.find(pEvent->GetServerId());
			oldServerId = pEvent->GetServerId() ;
		}
		if (unlikely(iter == m_ConnMap.end()))
		{
			WRITE_ERROR_LOG("can not find conn sql=%.*s serverid=%u",
				pEvent->GetSize(),pSql,pEvent->GetServerId());
			oldServerId = ServerID(-1);
			if(bDel)m_sql.Remove(pEvent->GetSize());
			pEvent->Delete();
			continue;
		}
		m_mysqlConn = iter->second.conn;
		if (mysql_real_query(m_mysqlConn,pSql,pEvent->GetSize()))
		{
			WRITE_ERROR_LOG("mysql_real_query has error  errno=%d errstr=%s sql=%.*s",
				mysql_errno(m_mysqlConn),mysql_error(m_mysqlConn),pEvent->GetSize(),pSql);
			//一般不会出现用重新连接来确认是网络问题还是sql问题失败直接强制关了
			if (Connect(iter->second)!= S_OK)
			{
				WRITE_INFO_LOG("dump begin\n%u:%.*s\ndump end",
					pEvent->GetServerId(),pEvent->GetSize(),pSql);
				m_forceKill = true;
				if(bDel)m_sql.Remove(pEvent->GetSize());
				pEvent->Delete();
				KillSelf();
				return;
			}
			//连接成功后在来一次不行就放弃了
			if (mysql_real_query(m_mysqlConn,pSql,pEvent->GetSize()))
			{
				WRITE_ERROR_LOG("mysql_real_query has error  errno=%d errstr=%s sql=%.*s",
					mysql_errno(m_mysqlConn),mysql_error(m_mysqlConn),pEvent->GetSize(),pSql);
				pEvent->CallBack(this,false);
				if(bDel)m_sql.Remove(pEvent->GetSize());
				pEvent->Delete();
				continue;
			}
		}
		if (pEvent->StoreRet())
		{
			m_res = mysql_store_result(m_mysqlConn);
			if(nullptr != m_res)
			{
				m_numFields = mysql_num_fields(m_res);
				m_numRow = static_cast<uint32_t>(mysql_num_rows(m_res));
			}
			else if (mysql_errno(m_mysqlConn))
			{
				WRITE_ERROR_LOG("mysql_store_result has error  errno=%d errstr=%s sql=%.*s",
					mysql_errno(m_mysqlConn),mysql_error(m_mysqlConn),pEvent->GetSize(),pSql);
				pEvent->CallBack(this,false);
				if(bDel)m_sql.Remove(pEvent->GetSize());
				pEvent->Delete();
				continue;
			}
			else//sql语句本身没有返回结果集的
				WRITE_WARING_LOG("mysql has no result chech your code sql=%.*s",pEvent->GetSize(),pSql);
		}
		pEvent->CallBack(this,true);
		if(bDel)m_sql.Remove(pEvent->GetSize());
		pEvent->Delete();
		if (m_res != nullptr)
		{
			mysql_free_result(m_res);
			m_res = nullptr;
		}
	}//while
	CheckSqlEvent();
}
 void CMysqlManager::CheckSqlEvent()
 {
	 while(!m_funEvent.IsFull() && !m_listFun.empty())
	 {
		 m_funEvent.Push(m_listFun.front());
		 m_listFun.pop_front();
	 }
 }
bool CMysqlManager::Fetch()
{
	if(m_res == nullptr)
		return false;
	m_row = mysql_fetch_row(m_res);
	if (nullptr != m_row)
	{
		m_length = mysql_fetch_lengths(m_res);
		return true;
	}
	return false;
}
int32_t CMysqlManager::Connect(SqlConnInfo& stInfo)
{
	uint32_t num = 0;
	while (true)
	{
		if(++num > 60)//测试60次
			return E_UNKNOW;
		MYSQL* Conn = mysql_init(nullptr);
		if(nullptr == Conn)
		{
			WRITE_ERROR_LOG("mysql_init error");
			return E_UNKNOW;
		}
		char reconnect = 1;
		if(mysql_options(Conn, MYSQL_OPT_RECONNECT,&reconnect))
		{
			WRITE_ERROR_LOG("mysql_options  MYSQL_OPT_RECONNECT error errno=%d errstr=%s",
				mysql_errno(Conn),mysql_error(Conn));
			mysql_close(Conn);
			return E_UNKNOW;
		}
		if(mysql_real_connect(Conn,stInfo.host.data(),stInfo.user.data(),stInfo.password.data(),
			stInfo.dbName.data(),stInfo.port,nullptr,0) == nullptr)
		{
			WRITE_ERROR_LOG("mysql_real_connect error errno=%d errstr=%s",
				mysql_errno(Conn),mysql_error(Conn));
			mysql_close(Conn);
			MySleep(1000);
			continue;
		}
		if(mysql_set_character_set(Conn,"utf8"))
		{
			WRITE_ERROR_LOG("mysql_set_character_set error errno=%d errstr=%s",
				mysql_errno(Conn),mysql_error(Conn));
			mysql_close(Conn);
			return E_UNKNOW;
		}
		m_mysqlConn = Conn;
		if(nullptr != stInfo.conn)
			mysql_close(stInfo.conn);
		stInfo.conn = Conn;
		break;
	}
	return S_OK;
}
void CMysqlManager::DumpSql()
{
	CMysqlEventBase* pEvent;
	char *pSql;
	WRITE_INFO_LOG("dump begin");
	while(!m_sqlEvent.IsEmpty())
	{
		m_sqlEvent.Pop(pEvent);
		pSql = pEvent->GetSql();
		pEvent->PopSize();
		bool bDel = false;
		if (pSql == nullptr)
		{
			if (unlikely(pEvent->GetSize() > m_MaxPopSize))
			{
				delete[] m_pPopSql;
				m_MaxPopSize = pEvent->GetSize();
				m_pPopSql = new char[uint64_t(m_MaxPopSize)+1];
			}
			pSql = m_pPopSql;
			bDel = m_sql.GetSqlBuf(pSql,pEvent->GetSize());
		}
		WRITE_INFO_LOG("%.*s",pEvent->GetSize(), pSql);
		if(bDel) m_sql.Remove(pEvent->GetSize());
		pEvent->Delete();
	}
	WRITE_INFO_LOG("dump end");
}
void CMysqlManager::DumpSqlCross()
{
	CMysqlEventBase* pEvent;
	char *pSql;
	WRITE_INFO_LOG("dump begin");
	while(!m_sqlEvent.IsEmpty())
	{
		m_sqlEvent.Pop(pEvent);
		pSql = pEvent->GetSql();
		pEvent->PopSizeEx();
		bool bDel = false;
		if (pSql == nullptr)
		{
			if (unlikely(pEvent->GetSize() > m_MaxPopSize))
			{
				delete[] m_pPopSql;
				m_MaxPopSize = pEvent->GetSize();
				m_pPopSql = new char[uint64_t(m_MaxPopSize)+1];
			}
			pSql = m_pPopSql;
			bDel = m_sql.GetSqlBuf(pSql,pEvent->GetSize());
			
		}
		WRITE_INFO_LOG("%u:%.*s",pEvent->GetServerId(),pEvent->GetSize(),pSql);
		if(bDel) m_sql.Remove(pEvent->GetSize());
		pEvent->Delete();
	}
	WRITE_INFO_LOG("dump end");
}
uint32_t CMysqlManager::EscapeString(const char* pFrom,uint32_t nSize,char * pTo,uint32_t nMaxSize)
{
	if(nMaxSize < (nSize<<1)+1) return 0;//目标缓冲区不够
	return (uint32_t)mysql_real_escape_string(m_EscapeStr,pTo,pFrom,nSize);
}
void CMysqlManager::EscapeString(const char*pFrom,uint32_t nSize,std::string& to)
{
	if (unlikely(nSize > m_MaxEscapeSize))
	{
		delete[] m_pEscapeSql;
		m_MaxEscapeSize = nSize;
		m_pEscapeSql = new char[2* uint64_t(m_MaxEscapeSize)+1];
	}
	auto len = mysql_real_escape_string(m_EscapeStr,m_pEscapeSql,pFrom,nSize);
	to.assign(m_pEscapeSql,len);
}
void CMysqlManager::EscapeString(std::string& from, std::string& to)
{
	if (unlikely(from.size() > m_MaxEscapeSize))
	{
		delete[] m_pEscapeSql;
		m_MaxEscapeSize = (uint32_t)from.size();
		m_pEscapeSql = new char[2 * uint64_t(m_MaxEscapeSize) + 1];
	}
	auto len = mysql_real_escape_string(m_EscapeStr, m_pEscapeSql, from.data(), (uint32_t)from.size());
	to.append(1, '"');
	to.append(m_pEscapeSql, len);
	to.append(1, '"');
}
bool CMysqlManager::EscapeString(std::string& from, char* to,uint32_t maxSize,uint32_t& outSize)
{
	if (maxSize < (from.size() << 1) + 3)return true;
	to[0] = '"';
	auto nRet = mysql_real_escape_string(m_EscapeStr, &to[1], from.data(), (uint32_t)from.size());
	to[nRet + 1] = '"';
	to[nRet + 2] = ',';
	outSize += nRet + 3;
	return false;
}
const char* CMysqlManager::EscapeString(const char*pFrom,uint32_t nSize)
{
	if (unlikely(nSize > m_MaxEscapeSize))
	{
		delete[] m_pEscapeSql;
		m_MaxEscapeSize = nSize;
		m_pEscapeSql = new char[2* uint64_t(m_MaxEscapeSize)+1];
	}
	mysql_real_escape_string(m_EscapeStr,m_pEscapeSql,pFrom,nSize);
	return m_pEscapeSql;
}
std::string CMysqlManager::EscapeString(const char*pFrom,uint32_t nSize,bool)
{
	if (unlikely(nSize > m_MaxEscapeSize))
	{
		delete[] m_pEscapeSql;
		m_MaxEscapeSize = nSize;
		m_pEscapeSql = new char[2* uint64_t(m_MaxEscapeSize)+1];
	}
	auto len = mysql_real_escape_string(m_EscapeStr,m_pEscapeSql,pFrom,nSize);
	return std::string(m_pEscapeSql,len);
}
void CMysqlManager::Get(uint32_t index,uint8_t&val)
{
	if(index < m_numFields && nullptr != m_row[index])
		val = static_cast<uint8_t>(atoi(m_row[index]));
}
void CMysqlManager::Get(uint32_t index,int8_t&val)
{
	if(index < m_numFields && nullptr != m_row[index])
		val = static_cast<int8_t>(atoi(m_row[index]));
}
void CMysqlManager::Get(uint32_t index,uint16_t&val)
{
	if(index < m_numFields && nullptr != m_row[index])
		val = static_cast<uint16_t>(atoi(m_row[index]));
}
void CMysqlManager::Get(uint32_t index,int16_t&val)
{
	if(index < m_numFields && nullptr != m_row[index])
		val = static_cast<int16_t>(atoi(m_row[index]));
}
void CMysqlManager::Get(uint32_t index,uint32_t&val)
{
	if (index < m_numFields && nullptr != m_row[index])
		val = static_cast<uint32_t>(strtoul(m_row[index], nullptr, 0));
}
void CMysqlManager::Get(uint32_t index,int32_t&val)
{
	if(index < m_numFields && nullptr != m_row[index])
		val = atoi(m_row[index]);
}
void CMysqlManager::Get(uint32_t index,uint64_t&val)
{
	if(index < m_numFields && nullptr != m_row[index])
#ifdef _64_PLATFORM_
		val= strtoul(m_row[index],nullptr,0);
#else
		val= STRTOULL(m_row[index],nullptr,0);
#endif // _64_PLATFORM_
}
void CMysqlManager::Get(uint32_t index,int64_t&val)
{
	if(index < m_numFields && nullptr != m_row[index])
#ifdef _64_PLATFORM_
		val = strtol(m_row[index],nullptr,0);
#else
		val = STRTOLL(m_row[index],nullptr,0);
#endif // _64_PLATFORM_
}
void CMysqlManager::Get(uint32_t index,std::string&val)
{
	if(index < m_numFields && nullptr != m_row[index])
		val.assign(m_row[index],m_length[index]);
	else 
		val.clear();
}
uint32_t CMysqlManager::Get(uint32_t index,char*val,uint32_t maxSize)
{
	if(index < m_numFields && nullptr != m_row[index] && m_length[index] < maxSize)
	{
		memcpy(val,m_row[index],m_length[index]);
		val[m_length[index]]='\0';
		return uint32_t(m_length[index]);
	}
	else 
		val[0]='\0';
	return 0;
}
template <typename T>
T CMysqlManager::Get(uint32_t index)
{
	return Get<T>(index);
}
template <>
uint8_t CMysqlManager:: Get<uint8_t>(uint32_t index)
{
	if(index < m_numFields && nullptr != m_row[index])
		return static_cast<uint8_t>(atoi(m_row[index]));
	return 0;
}
template <>
uint16_t CMysqlManager::Get<uint16_t>(uint32_t index)
{
	if(index < m_numFields && nullptr != m_row[index])
		return static_cast<uint16_t>(atoi(m_row[index]));
	return 0;
}
template <>
uint32_t CMysqlManager::Get<uint32_t>(uint32_t index)
{
	if(index < m_numFields && nullptr != m_row[index])
		return static_cast<uint32_t>(strtoul(m_row[index], nullptr, 0));
	return 0;
}
template <>
uint64_t CMysqlManager::Get<uint64_t>(uint32_t index)
{
	if(index < m_numFields && nullptr != m_row[index])
#ifdef _64_PLATFORM_
		return strtoul(m_row[index],nullptr,0);
#else
		return STRTOULL(m_row[index],nullptr,0);
#endif // _64_PLATFORM_
	return 0;
}
template <>
int8_t CMysqlManager::Get<int8_t>(uint32_t index)
{
	if(index < m_numFields && nullptr != m_row[index])
		return static_cast<int8_t>(atoi(m_row[index]));
	return 0;
}
template <>
int16_t CMysqlManager::Get<int16_t>(uint32_t index)
{
	if(index < m_numFields && nullptr != m_row[index])
		return static_cast<int16_t>(atoi(m_row[index]));
	return 0;
}
template <>
int32_t CMysqlManager::Get<int32_t>(uint32_t index)
{
	if(index < m_numFields && nullptr != m_row[index])
		return atoi(m_row[index]);
	return 0;
}
template <>
int64_t CMysqlManager::Get<int64_t>(uint32_t index)
{
	if(index < m_numFields && nullptr != m_row[index])
#ifdef _64_PLATFORM_
		return strtol(m_row[index],nullptr,0);
#else
		return STRTOLL(m_row[index],nullptr,0);
#endif // _64_PLATFORM_
	return 0;
}
template <>
std::string CMysqlManager::Get<std::string>(uint32_t index)
{
	if(index < m_numFields && nullptr != m_row[index])
		return std::string(m_row[index],m_length[index]);
	return std::string();
}
template <>
const char* CMysqlManager::Get<const char*>(uint32_t index)
{
	if(index < m_numFields)
		return m_row[index];
	return nullptr;
}
void CMysqlManager::Run()
{
	if (m_cross)
	{
		while (IsRun())
		{
			//CTimerManager::GetInstance().DoTimerEvent();
			ExcSqlCross();
			MySleep(1);
		}
		ExcSqlCross();
	}
	else
	{
		FreeRes();
		while (IsRun())
		{
			//CTimerManager::GetInstance().DoTimerEvent();
			ExcSql();
			MySleep(1);
		}
		ExcSql();
	}
}
void CMysqlManager::FreeRes()
{
	if (m_res != nullptr)
	{
		mysql_free_result(m_res);
		m_res = nullptr;
	}
}
bool CMysqlManager::LoadData(const char* sql, size_t len)
{
	if (IsRun())return false;//线程已经启动了不能调用这个函数了
	FreeRes();
	if (mysql_real_query(m_mysqlConn, sql, (unsigned long)len))
	{
		WRITE_ERROR_LOG("mysql_real_query has error  errno=%d errstr=%s sql=%.*s",
			mysql_errno(m_mysqlConn), mysql_error(m_mysqlConn),(uint32_t)len, sql);
		return false;
	}
	m_res = mysql_store_result(m_mysqlConn);
	if (nullptr != m_res)
	{
		m_numFields = mysql_num_fields(m_res);
		m_numRow = static_cast<uint32_t>(mysql_num_rows(m_res));
		return true;
	}
	else if (mysql_errno(m_mysqlConn))
		WRITE_ERROR_LOG("mysql_store_result has error  errno=%d errstr=%s sql=%.*s",
			mysql_errno(m_mysqlConn), mysql_error(m_mysqlConn), (uint32_t)len, sql);
	else//sql语句本身没有返回结果集的
		WRITE_WARING_LOG("mysql has no result chech your code sql=%.*s", (uint32_t)len, sql);
	return false;
}