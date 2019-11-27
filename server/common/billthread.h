#ifndef _BILL_THREAD_H_
#define _BILL_THREAD_H_
#include <string>
#include <list>
#include <mysql.h>
#include "thread.h"
#include "singleton.h"
#include "newqueue.h"
#include "msgbase.h"
/*
数据库日志线程
跨服实现多线程可以将m_sql定义为一个数组，每个线程只执行一个指定的下标，
将所有的连接均匀分配到指定的下标数组中。但是我觉得一个线程足够.跨服的日志不会很多
本服也可以用多个m_sql实现多线程，当然前提是逻辑单线程 也可以做加锁处理
*/
class CBillThread:public CThread,public CSingleton<CBillThread>
{
private:
	struct SqlConnInfo
	{
		MYSQL* conn=nullptr;//数据库连接sql线程使用
		std::string host;
		std::string user;
		std::string password;
		std::string dbName;
		uint16_t port=0;
	};
public:
	CBillThread();
	~CBillThread();
	int32_t Init(const std::string& host,const std::string& user,const std::string& password,
		const std::string& dbName,uint16_t port,ServerID serverId=0);
	void SetCross(){m_cross=true;}
	void virtual Run()override;
	//本地服serverId会被忽略
	void PostBill(const char*pSql,uint32_t nSize,ServerID serverId=0);
	void PostBill(const std::string& szSql,ServerID serverId=0)
	{PostBill(szSql.data(),(uint32_t)szSql.size());}
	void PostBill(std::string&& szSql,ServerID serverId=0)
	{PostBill(szSql.data(),(uint32_t)szSql.size());}
	void PostBill(CBillBase* pBill,ServerID serverId=0);
	void PostBill(CBillBase* pBill,const CRole* pRole,ServerID serverId=0)
	{
		if(likely(pRole != nullptr))
			PostBill(pBill,*pRole,serverId);
	}
	void PostBill(CBillBase* pBill,const CRole& role,ServerID serverId=0);
private:
	int32_t Connect(std::shared_ptr<SqlConnInfo>& pInf);
	void LocalRun();
	void CrossRun();
	ServerID GetServerID(RoleID roleId){return 0;}
private:
	CNewQueue<char,5*1024*1024> m_sql;
	std::list<std::string> m_list;//这个队列理论上不应该被用到，用到了那估计就的用其他方法了
	std::map<ServerID,std::shared_ptr<SqlConnInfo>> m_ConnMap;
	MYSQL* m_Conn;
	bool m_cross;
};
#endif