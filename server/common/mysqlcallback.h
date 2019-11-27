#ifndef _MYSQL_CALLBACK_H_
#define _MYSQL_CALLBACK_H_
#include "define.h"
#include "mysqlrecord.h"
//数据库回调基类
class CMysqlCallBack
{
public:
	CMysqlCallBack(){}
	virtual ~CMysqlCallBack(){}
public:
	//insert update delete 用的回调
	virtual void CallBack(bool succ,uint32_t nAffectRow){}
	virtual void CallBack(bool succ, uint32_t nAffectRow,FuncData data){}
	//有自增长id的insert的回调
	virtual void CallBack(bool succ, uint32_t nAffectRow,uint64_t nInsertID){}
	virtual void CallBack(bool succ, uint32_t nAffectRow,uint64_t nInsertID,FuncData data){}
	//select 用的回调 
	virtual void CallBack(bool succ, std::shared_ptr<CMysqlRecord> pRecord){}
	virtual void CallBack(bool succ, std::shared_ptr<CMysqlRecord> pRecord,FuncData data){}
private:

};
#endif