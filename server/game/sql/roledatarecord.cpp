#include "roledatarecord.h"
#include "log.h"
#include "mysqlmanager.h"
#include "common_api.h"
#include "snprintf.h"
CRoledataRecord::CRoledataRecord()
{
}
CRoledataRecord::~CRoledataRecord()
{
}
void CRoledataRecord::Update(CMysqlCallBack* pCallBack , FuncData data)
{
	if (!PostUpdateBuf(pCallBack,data))
		PostUpdateStr(pCallBack,data);
}
void CRoledataRecord::Insert()
{
	if (!PostInsertBuf())
		PostInsertStr();
}
void CRoledataRecord::Delete(uint64_t id)
{
	char sql[SHORTSQLSIZE];
	int nRet = SNPRINTF(sql, SHORTSQLSIZE, "delete from roledata where id=%" PRIu64 "",id);
	if (nRet >= SHORTSQLSIZE || nRet < 0)
		WRITE_ERROR_LOG("build roledata delete sql has error ret=%d", nRet);
	else
		CMysqlManager::Instance().PostSql(sql, nRet);
}
void CRoledataRecord::Select(CMysqlCallBack* pCallBack,uint64_t id)
{
	char sql[SHORTSQLSIZE];
	int nRet = SNPRINTF(sql, SHORTSQLSIZE, "select * from roledata where id=%" PRIu64 "",id);
	if (nRet >= SHORTSQLSIZE || nRet < 0)
		WRITE_ERROR_LOG("build role delete sql has error ret=%d", nRet);
	else
		CMysqlManager::Instance().PostSql<CRoledataRecord>(pCallBack, sql, nRet);
}
void CRoledataRecord::Select(CMysqlCallBack* pCallBack, FuncData data,uint64_t id)
{
	char sql[SHORTSQLSIZE];
	int nRet = SNPRINTF(sql, SHORTSQLSIZE, "select * from roledata where id=%" PRIu64 "",id);
	if (nRet >= SHORTSQLSIZE || nRet < 0)
		WRITE_ERROR_LOG("build role delete sql has error ret=%d", nRet);
	else
		CMysqlManager::Instance().PostSql<CRoledataRecord>(pCallBack, sql, nRet, data);
}
bool CRoledataRecord::SaveRecord(CMysqlManager* pMysql)
{
	if (GetColNum() != pMysql->GetColNum() || !pMysql->Fetch())
		return false;
	uint16_t i = 0;
	pMysql->Get(i++,id);
	pMysql->Get(i++,name);
	pMysql->Get(i++,account);
	pMysql->Get(i++,level);
	pMysql->Get(i++,exp);
	pMysql->Get(i++,skill);
	pMysql->Get(i++,item);
	pMysql->Get(i++,activity);
	pMysql->Get(i++,createtime);
	return true;
}
bool CRoledataRecord::PostUpdateBuf(CMysqlCallBack* pCallBack, FuncData data)
{
	char* sql = CMysqlManager::Instance().GetBuildSql();
	uint32_t nSize = (uint32_t)SNPRINTF(sql,SQLMAXSIZE,"update roledata set `level`=%hu,`exp`=%u,`createtime`='%s',",level,exp,createtime.data());
	if (CpySql(sql+nSize,SQLMAXSIZE-nSize,"`name`=",7,nSize)||ESCAPESTR(name,sql+nSize,SQLMAXSIZE-nSize,nSize))return false;
	if (CpySql(sql+nSize,SQLMAXSIZE-nSize,"`account`=",10,nSize)||ESCAPESTR(account,sql+nSize,SQLMAXSIZE-nSize,nSize))return false;
	if (CpySql(sql+nSize,SQLMAXSIZE-nSize,"`skill`=",8,nSize)||ESCAPESTR(skill,sql+nSize,SQLMAXSIZE-nSize,nSize))return false;
	if (CpySql(sql+nSize,SQLMAXSIZE-nSize,"`item`=",7,nSize)||ESCAPESTR(item,sql+nSize,SQLMAXSIZE-nSize,nSize))return false;
	if (CpySql(sql+nSize,SQLMAXSIZE-nSize,"`activity`=",11,nSize)||ESCAPESTR(activity,sql+nSize,SQLMAXSIZE-nSize,nSize))return false;
	nSize -= 1;
	if (CpySql(sql + nSize, SQLMAXSIZE - nSize, " where `id`=", 12, nSize))return false;
	auto nRet = ToStr(id, sql + nSize, SQLMAXSIZE - nSize);
	nSize += nRet;
	if (nRet < 0 || nSize > SQLMAXSIZE)return false;
	if(nullptr == pCallBack)
		CMysqlManager::Instance().PostSql(sql, nSize);
	else if(data.u64 != 0)
		CMysqlManager::Instance().PostSql(pCallBack, sql, nSize,data);
	else
		CMysqlManager::Instance().PostSql(pCallBack, sql, nSize);
	return true;
}
void CRoledataRecord::PostUpdateStr(CMysqlCallBack* pCallBack, FuncData data)
{
	char buf[32];
	std::string sql;
	sql.reserve(SQLMAXSIZE + SHORTSQLSIZE);
	sql.append("update roledata set ",20);
	sql.append("`name`=",7);ESCAPESTR(name, sql);
	sql.append(",`account`=",11);ESCAPESTR(account, sql);
	sql.append(",`level`=",9);sql.append(buf, ToStr(level, buf, 32));
	sql.append(",`exp`=",7);sql.append(buf, ToStr(exp, buf, 32));
	sql.append(",`skill`=",9);ESCAPESTR(skill, sql);
	sql.append(",`item`=",8);ESCAPESTR(item, sql);
	sql.append(",`activity`=",12);ESCAPESTR(activity, sql);
	sql.append(",`createtime`=",14);sql.append(1,'"');sql.append(createtime);sql.append(1,'"');
	
	sql.append(" where `id`=",12);
	sql.append(buf, ToStr(id, buf, 32));
	if (nullptr == pCallBack)
		CMysqlManager::Instance().PostSql((char*)sql.data(), (uint32_t)sql.size());
	else if (data.u64 != 0)
		CMysqlManager::Instance().PostSql(pCallBack, (char*)sql.data(), (uint32_t)sql.size(), data);
	else
		CMysqlManager::Instance().PostSql(pCallBack, (char*)sql.data(), (uint32_t)sql.size());
}
bool CRoledataRecord::PostInsertBuf()
{
	char* sql = CMysqlManager::Instance().GetBuildSql();
	uint32_t  nSize = SNPRINTF(sql ,SQLMAXSIZE, "insert into roledata (`id`,`name`,`account`,`level`,`exp`,`skill`,`item`,`activity`,`createtime` )value(%" PRIu64 ",%hu,%u,'%s',",id,level,exp,createtime.data());
	if (ESCAPESTR(name, sql + nSize, SQLMAXSIZE - nSize, nSize))return false;
	if (ESCAPESTR(account, sql + nSize, SQLMAXSIZE - nSize, nSize))return false;
	if (ESCAPESTR(skill, sql + nSize, SQLMAXSIZE - nSize, nSize))return false;
	if (ESCAPESTR(item, sql + nSize, SQLMAXSIZE - nSize, nSize))return false;
	if (ESCAPESTR(activity, sql + nSize, SQLMAXSIZE - nSize, nSize))return false;
	sql[nSize - 1] = ')';
	CMysqlManager::Instance().PostSql(sql, nSize);
	return true;
}
void CRoledataRecord::PostInsertStr()
{
	char buf[32];
	std::string sql;
	sql.reserve(SQLMAXSIZE + SHORTSQLSIZE);
	sql.append("insert into roledata (`id`,`name`,`account`,`level`,`exp`,`skill`,`item`,`activity`,`createtime` )value(");
	sql.append(buf, ToStr(id, buf, 32));
	sql.append(1,',');	sql.append(buf, ToStr(level, buf, 32));
	sql.append(1,',');	sql.append(buf, ToStr(exp, buf, 32));
	sql.append(1,',');	sql.append(1,'"'); sql.append(createtime); sql.append(1,'"');
	sql.append(1,',');	ESCAPESTR(name, sql);
	sql.append(1,',');	ESCAPESTR(account, sql);
	sql.append(1,',');	ESCAPESTR(skill, sql);
	sql.append(1,',');	ESCAPESTR(item, sql);
	sql.append(1,',');	ESCAPESTR(activity, sql);
	sql.append(1, ')');
	CMysqlManager::Instance().PostSql((char*)sql.data(), (uint32_t)sql.size());
}
