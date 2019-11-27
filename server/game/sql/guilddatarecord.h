#ifndef _H_GUILDDATA_RECORD_
#define _H_GUILDDATA_RECORD_
#include <string>
#include "define.h"
#include "mysqlrecord.h"
class CMysqlCallBack;

class CGuilddataRecord :public CMysqlRecord
{
public:
	static uint16_t GetColNum(){return 5;}
	CGuilddataRecord();
	virtual ~CGuilddataRecord();
	void Update(CMysqlCallBack* pCallBack = nullptr, FuncData data=uint64_t(0));
	void Insert();
	static void Delete(int32_t id);
	void Delete(){ Delete(id);}
	static void Select(CMysqlCallBack* pCallBack,int32_t id);
	void Select(CMysqlCallBack* pCallBack){ Select(pCallBack,id); }
	static void Select(CMysqlCallBack* pCallBack, FuncData data,int32_t id);
	void Select(CMysqlCallBack* pCallBack, FuncData data){ Select(pCallBack,data,id);}
	virtual bool SaveRecord(CMysqlManager* pMysql)override;
private:
	bool PostUpdateBuf(CMysqlCallBack* pCallBack = nullptr, FuncData data=uint64_t(0));
	void PostUpdateStr(CMysqlCallBack* pCallBack = nullptr, FuncData data=uint64_t(0));
	bool PostInsertBuf();
	void PostInsertStr();
public:
	int32_t id = 0;
	std::string member ;
	int8_t exp = 0;
	int16_t level = 0;
	std::string createtime ;
};
#endif