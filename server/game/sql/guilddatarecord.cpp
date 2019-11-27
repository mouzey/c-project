#include "guilddatarecord.h"
#include "log.h"
#include "mysqlmanager.h"
#include "common_api.h"
#include "snprintf.h"
CGuilddataRecord::CGuilddataRecord()
{
}
CGuilddataRecord::~CGuilddataRecord()
{
}
void CGuilddataRecord::Update(CMysqlCallBack* pCallBack , FuncData data)
{
	if (!PostUpdateBuf(pCallBack,data))
		PostUpdateStr(pCallBack,data);
}
void CGuilddataRecord::Insert()
{
	if (!PostInsertBuf())
		PostInsertStr();
}
void CGuilddataRecord::Delete(int32_t id)
{
	char sql[SHORTSQLSIZE];
	int nRet = SNPRINTF(sql, SHORTSQLSIZE, "delete from guilddata where id=%d",id);
	if (nRet >= SHORTSQLSIZE || nRet < 0)
		WRITE_ERROR_LOG("build guilddata delete sql has error ret=%d", nRet);
	else
		CMysqlManager::Instance().PostSql(sql, nRet);
}
void CGuilddataRecord::Select(CMysqlCallBack* pCallBack,int32_t id)
{
	char sql[SHORTSQLSIZE];
	int nRet = SNPRINTF(sql, SHORTSQLSIZE, "select * from guilddata where id=%d",id);
	if (nRet >= SHORTSQLSIZE || nRet < 0)
		WRITE_ERROR_LOG("build role delete sql has error ret=%d", nRet);
	else
		CMysqlManager::Instance().PostSql<CGuilddataRecord>(pCallBack, sql, nRet);
}
void CGuilddataRecord::Select(CMysqlCallBack* pCallBack, FuncData data,int32_t id)
{
	char sql[SHORTSQLSIZE];
	int nRet = SNPRINTF(sql, SHORTSQLSIZE, "select * from guilddata where id=%d",id);
	if (nRet >= SHORTSQLSIZE || nRet < 0)
		WRITE_ERROR_LOG("build role delete sql has error ret=%d", nRet);
	else
		CMysqlManager::Instance().PostSql<CGuilddataRecord>(pCallBack, sql, nRet, data);
}
bool CGuilddataRecord::SaveRecord(CMysqlManager* pMysql)
{
	if (GetColNum() != pMysql->GetColNum() || !pMysql->Fetch())
		return false;
	uint16_t i = 0;
	pMysql->Get(i++,id);
	pMysql->Get(i++,member);
	pMysql->Get(i++,exp);
	pMysql->Get(i++,level);
	pMysql->Get(i++,createtime);
	return true;
}
bool CGuilddataRecord::PostUpdateBuf(CMysqlCallBack* pCallBack, FuncData data)
{
	char* sql = CMysqlManager::Instance().GetBuildSql();
	uint32_t nSize = (uint32_t)SNPRINTF(sql,SQLMAXSIZE,"update guilddata set `exp`=%hhd,`level`=%hd,`createtime`='%s',",exp,level,createtime.data());
	if (CpySql(sql+nSize,SQLMAXSIZE-nSize,"`member`=",9,nSize)||ESCAPESTR(member,sql+nSize,SQLMAXSIZE-nSize,nSize))return false;
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
void CGuilddataRecord::PostUpdateStr(CMysqlCallBack* pCallBack, FuncData data)
{
	char buf[32];
	std::string sql;
	sql.reserve(SQLMAXSIZE + SHORTSQLSIZE);
	sql.append("update guilddata set ",21);
	sql.append("`member`=",9);ESCAPESTR(member, sql);
	sql.append(",`exp`=",7);sql.append(buf, ToStr(exp, buf, 32));
	sql.append(",`level`=",9);sql.append(buf, ToStr(level, buf, 32));
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
bool CGuilddataRecord::PostInsertBuf()
{
	char* sql = CMysqlManager::Instance().GetBuildSql();
	uint32_t  nSize = SNPRINTF(sql ,SQLMAXSIZE, "insert into guilddata (`id`,`member`,`exp`,`level`,`createtime` )value(%d,%hhd,%hd,'%s',",id,exp,level,createtime.data());
	if (ESCAPESTR(member, sql + nSize, SQLMAXSIZE - nSize, nSize))return false;
	sql[nSize - 1] = ')';
	CMysqlManager::Instance().PostSql(sql, nSize);
	return true;
}
void CGuilddataRecord::PostInsertStr()
{
	char buf[32];
	std::string sql;
	sql.reserve(SQLMAXSIZE + SHORTSQLSIZE);
	sql.append("insert into guilddata (`id`,`member`,`exp`,`level`,`createtime` )value(");
	sql.append(buf, ToStr(id, buf, 32));
	sql.append(1,',');	sql.append(buf, ToStr(exp, buf, 32));
	sql.append(1,',');	sql.append(buf, ToStr(level, buf, 32));
	sql.append(1,',');	sql.append(1,'"'); sql.append(createtime); sql.append(1,'"');
	sql.append(1,',');	ESCAPESTR(member, sql);
	sql.append(1, ')');
	CMysqlManager::Instance().PostSql((char*)sql.data(), (uint32_t)sql.size());
}
