#ifndef _H_ROLEDATA_RECORD_
#define _H_ROLEDATA_RECORD_
#include <string>
#include "define.h"
#include "mysqlrecord.h"
class CMysqlCallBack;

class CRoledataRecord :public CMysqlRecord
{
public:
	static uint16_t GetColNum(){return 9;}
	CRoledataRecord();
	virtual ~CRoledataRecord();
	void Update(CMysqlCallBack* pCallBack = nullptr, FuncData data=uint64_t(0));
	void Insert();
	static void Delete(uint64_t id);
	void Delete(){ Delete(id);}
	static void Select(CMysqlCallBack* pCallBack,uint64_t id);
	void Select(CMysqlCallBack* pCallBack){ Select(pCallBack,id); }
	static void Select(CMysqlCallBack* pCallBack, FuncData data,uint64_t id);
	void Select(CMysqlCallBack* pCallBack, FuncData data){ Select(pCallBack,data,id);}
	virtual bool SaveRecord(CMysqlManager* pMysql)override;
private:
	bool PostUpdateBuf(CMysqlCallBack* pCallBack = nullptr, FuncData data=uint64_t(0));
	void PostUpdateStr(CMysqlCallBack* pCallBack = nullptr, FuncData data=uint64_t(0));
	bool PostInsertBuf();
	void PostInsertStr();
public:
	uint64_t id = 0;
	std::string name ;
	std::string account ;
	uint16_t level = 0;
	uint32_t exp = 0;
	std::string skill ;
	std::string item ;
	std::string activity ;
	std::string createtime ;
};
#endif