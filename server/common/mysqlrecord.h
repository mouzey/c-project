#ifndef _MYSQL_RECORD_H_
#define _MYSQL_RECORD_H_
class CMysqlManager;
class CMysqlRecord
{
public:
	CMysqlRecord(){}
	virtual ~CMysqlRecord(){}
	virtual bool SaveRecord(CMysqlManager*)=0;
};
#endif