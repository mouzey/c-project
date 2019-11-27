#ifndef _MYSQL_MANAGER_H
#define _MYSQL_MANAGER_H
#include <list>
#include <string>
#include <mysql.h>
#include <map>
#include "functionevent.h"
#include "mysqlevent.h"
#include "newqueue.h"
#include "log.h"
#include "singleton.h"
#include "timerbase.h"
#include "thread.h"
#if 0//预定义可以减少头文件包含
struct st_mysql;
typedef st_mysql MYSQL;
struct st_mysql_res;
typedef st_mysql_res MYSQL_RES;
typedef char** MYSQL_ROW;
#endif
//数据库线程
class CMysqlManager:public CThread,public CTimerBase,public CSingleton<CMysqlManager>
{
private:
	struct SqlConnInfo
	{
		MYSQL* conn = nullptr;//数据库连接sql线程使用
		std::string host;
		std::string user;
		std::string password;
		std::string dbName;
		uint16_t port = 0;
	};
public:
	CMysqlManager();
	~CMysqlManager();
	int32_t Init(const std::string& host,const std::string& user,const std::string& password,
		const std::string& dbName,uint16_t port,ServerID serverId);
	void SetCross(){m_cross=true;m_emptyEvent.NewServerID();}
	void virtual Run()override;
public:
	////本地服id会被忽略
	//需要多线程的话接口需要加线程标记并且需要加锁
	//以下接口没有回调函数用于delete update insert
	void PostSql(char* pSql,uint32_t nSize,ServerID id=0);
	//以下接口调用void CallBack(uint32_t nAffectRow)用于delete update insert
	void PostSql(CMysqlCallBack* pCallBack,char* pSql,uint32_t nSize,ServerID id=0);
	//以下接口调用void CallBack(uint32_t nAffectRow,MysqlData data)用于delete update insert
	void PostSql(CMysqlCallBack* pCallBack,char* pSql,uint32_t nSize,FuncData data,ServerID id=0);
	//以下接口调用void CallBack(uint32_t nAffectRow,uint64_t nInsertID) insert
	void PostInsert(CMysqlCallBack* pCallBack,char* pSql,uint32_t nSize,ServerID id=0);
	//以下接口调用void CallBack(uint32_t nAffectRow,uint64_t nInsertID,MysqlData data)insert
	void PostInsert(CMysqlCallBack* pCallBack,char* pSql,uint32_t nSize,FuncData data,ServerID id=0);
	//以下接口直接传入一个lambda函数 调用方式为handle(CMysqlManager*)
	template< class Handler>
	void PostSql(Handler handle,char* pSql,uint32_t nSize,ServerID id=0,bool bstore=false)
	{
		if (m_sql.AddOnlyBuf(pSql, nSize) == S_OK)
			PostSql(new CMysqlEventFunc<Handler>(nullptr, nSize, handle, bstore, id));
		else
			PostSql(new CMysqlEventFunc<Handler>(pSql,nSize,handle,bstore,id));
	}
	//以下接口直接传入一个lambda函数 调用方式为handle(CMysqlManager*，data)
	template< class Handler>
	void PostSql(Handler handle,char* pSql,uint32_t nSize,FuncData data,ServerID id=0,bool bstore=false)
	{
		if (m_sql.AddOnlyBuf(pSql,nSize) == S_OK)
			PostSql(new CMysqlEventFuncData<Handler>(nullptr,nSize,handle,data,bstore,id));
		else
			PostSql(new CMysqlEventFuncData<Handler>(pSql,nSize,handle,data,bstore,id));
	}
	//以下接口用于select 返回一个std::shared_ptr<CMysqlRecord>对象
	template< class T>
	void PostSql(CMysqlCallBack* pCallBack,char* pSql,uint32_t nSize,ServerID id=0)
	{
		if (m_sql.AddOnlyBuf(pSql,nSize) == S_OK)
			PostSql(new CMysqlEventSelect<T>(nullptr,nSize,pCallBack,id));
		else
			PostSql(new CMysqlEventSelect<T>(pSql,nSize,pCallBack,id));
	}
	//以下接口用于select 返回一个std::shared_ptr<CMysqlRecord>对象
	template< class T>
	void PostSql(CMysqlCallBack* pCallBack,char* pSql,uint32_t nSize,FuncData data,ServerID id=0)
	{
		if (m_sql.AddOnlyBuf(pSql,nSize) == S_OK)
			PostSql(new CMysqlEventSelectData<T>(nullptr,nSize,pCallBack,data,id));
		else
			PostSql(new CMysqlEventSelectData<T>(pSql,nSize,pCallBack,data,id));
	}
	//以下接口用于select 返回一个std::shared_ptr<CMysqlRecord>对象 用CreateRecord创建
	void PostSql(CMysqlCallBack* pCallBack,char* pSql,uint32_t nSize,uint32_t nCase,ServerID id=0);
	//以下接口用于select 返回一个std::shared_ptr<CMysqlRecord>对象 用CreateRecord创建
	void PostSql(CMysqlCallBack* pCallBack,char* pSql,uint32_t nSize,FuncData data,uint32_t nCase,ServerID id=0);
public:
	uint32_t GetColNum()const {return m_numFields;}
	uint32_t GetAffectRow();
	uint64_t GetInsertID();
	std::shared_ptr<CMysqlRecord> CreateRecord(uint32_t nCase);
	//sql线程做一个队列，如果主线程队列满了先存起来检查指针
	void PushCallBack(CFuncEventBase* pEvent);
	bool PopCallBack(CFuncEventBase*& pEvent) { return m_funEvent.Pop(pEvent) == S_OK; }
	bool CallBackEmpty() { return m_funEvent.IsEmpty(); }
	//sql线程满了主线程自己处理检查指针
	 void PostSql(CMysqlEventBase* pEvent);
	 virtual void OnTimerEvent(const TimerDataInfo& data,const CDateTime& dt)override;
	 void CheckSqlEvent();
	 char* GetBuildSql() {return buildSql;}
private:
	//主线程push sql线程pop 无锁队列
	CNewQueue<CMysqlEventBase*> m_sqlEvent;
	//m_sqlEvent满了临时存储队列只有主线程操作
	std::list<CMysqlEventBase*> m_listSql;
	//sql线程执行sql语句后push的回调事件 sql线程push主线程pop
	CNewQueue<CFuncEventBase*> m_funEvent;
	//m_funEvent满了的时候临时存储队列只有sql线程操作
	std::list<CFuncEventBase*> m_listFun;
	//空事件，只执行sql语句不需要回调的sql
	CMysqlEventEmpty m_emptyEvent;
	//存sql语句，尽量减少内存分配 主线程push sql线程pop
	CNewQueue<char,5*1024*1024> m_sql;
	//用于sql线程缓存m_sql中pop出来的sql语句 sql线程使用
	char* m_pPopSql;
	uint32_t m_MaxPopSize;
	//用于主线成调用mysql_real_escape_string函数的目标缓存，线程不安全设计的只能主线程使用
	char* m_pEscapeSql;
	uint32_t m_MaxEscapeSize;
	//主线程用于构造sql语句使用
	char buildSql[SQLMAXSIZE];
private:
	MYSQL* m_mysqlConn;//数据库连接sql线程使用
	MYSQL* m_EscapeStr;//没有真实连接数据库只用于调用mysql_real_escape_string函数可以多线程调用
	MYSQL_RES* m_res;//mysql_store_result
	MYSQL_ROW m_row;//mysql_fetch_row
	unsigned long* m_length;//mysql_fetch_lengths
	uint32_t m_numFields;//mysql_num_fields
	uint32_t m_numRow;
	bool m_forceKill;//sql无法连接则sql线程会强制杀死进程
	bool m_cross;
	std::map<ServerID,SqlConnInfo> m_ConnMap;
public:
	void FreeRes();
	bool LoadData(const char* sql,size_t len);
	bool LoadData(const char* sql) { return LoadData(sql, strlen(sql)); }
	bool LoadData(std::string sql) { return LoadData(sql.data(),sql.size()); }
	void ExcSql();
	void ExcSqlCross();
	bool Fetch();
	uint32_t EscapeString(const char* pFrom,uint32_t nSize,char * pTo,uint32_t nMaxSize);
	//非线程安全
	void EscapeString(const char*pFrom,uint32_t nSize,std::string& to);
	const char* EscapeString(const char*pFrom,uint32_t nSize);
	std::string EscapeString(const char*pFrom,uint32_t nSize,bool);//最后一个参数填位置的
	void EscapeString(std::string& from, std::string& to);
	bool EscapeString(std::string& from, char* to, uint32_t maxSize, uint32_t& outSize);
	uint32_t GetFieldSize(uint32_t index) const{return uint32_t(m_length[index]);}
	uint32_t GetRowSize()const{return m_numRow;}
	void Get(uint32_t index,uint8_t&val);
	void Get(uint32_t index,int8_t&val);
	void Get(uint32_t index,uint16_t&val);
	void Get(uint32_t index,int16_t&val);
	void Get(uint32_t index,uint32_t&val);
	void Get(uint32_t index,int32_t&val);
	void Get(uint32_t index,uint64_t&val);
	void Get(uint32_t index,int64_t&val);
	void Get(uint32_t index,std::string&val);
	uint32_t Get(uint32_t index,char*val,uint32_t maxSize);
	template <typename T>
	T Get(uint32_t index);
private:
	//循环1分钟连接数据库
	int32_t Connect(SqlConnInfo& stInfo);
	//把所有的sql语句打印到日志中数据库连接出现问题了才会
	void DumpSql();
	void DumpSqlCross();
};
#define ESCAPESTR CMysqlManager::GetInstance().EscapeString
template <>
uint8_t CMysqlManager::Get<uint8_t>(uint32_t index);
template <>
uint16_t CMysqlManager::Get<uint16_t>(uint32_t index);
template <>
uint32_t CMysqlManager::Get<uint32_t>(uint32_t index);
template <>
uint64_t CMysqlManager::Get<uint64_t>(uint32_t index);
template <>
int8_t CMysqlManager::Get<int8_t>(uint32_t index);
template <>
int16_t CMysqlManager::Get<int16_t>(uint32_t index);
template <>
int32_t CMysqlManager::Get<int32_t>(uint32_t index);
template <>
int64_t CMysqlManager::Get<int64_t>(uint32_t index);
template <>
std::string CMysqlManager::Get<std::string>(uint32_t index);
template <>
const char* CMysqlManager::Get<const char*>(uint32_t index);
#endif