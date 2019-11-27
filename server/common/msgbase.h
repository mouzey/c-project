#ifndef _MSGBASE_H_
#define _MSGBASE_H_
#include<map>
#include<set>
#include<vector>
#include<string>
#include<cstdint>
#define BASEBILLFIELD "`time`,`serverid`,`roleid`,`loginserver`,`level`,`vip`,`name`,`account`"
#define BASEBILLFORMAT "'%s',%u,%u,%u,%u,%u,'%s','%s'"
#define BASEBILLVALUE CDateTime::GetNowTimeStr().data(),CServer::Instance().GetServerID(),\
	role.GetRoleID(),role.GetServerID(),role.GetLvel(),role.GetVip(),role.GetName().data(),\
	role.GetACcount().data()
class CRole;
struct SSMsgHead;
class CMsgBase
{
public:
	uint32_t m_len;
	uint16_t m_msgId;
	CMsgBase() :m_len(0), m_msgId(0) {}
	CMsgBase(uint16_t msgId) :m_len(0), m_msgId(msgId) {}
	virtual ~CMsgBase(){}
	virtual int32_t Encode(int8_t* pBuf, const uint32_t nSize)const{return 0;}
	virtual int32_t Decode(int8_t *pBuf, const uint32_t nSize){return 0;}
	virtual int32_t Dump(char* pBuf, const uint32_t nSize)const {return 0;}
	virtual bool Encode()const;
	virtual bool Decode();
};
class CBillBase
{
public:
	CBillBase(){}
	virtual ~CBillBase(){}
	virtual int32_t GetSql(char *pBuf,const uint32_t nSize) const {return 0;};
	virtual int32_t GetSql(char *pBuf,const uint32_t nSize,const CRole&) const {return 0;};
};
class CMsgHandleBase
{
public:
	CMsgHandleBase(){}
	~CMsgHandleBase(){}
	virtual void OnEvent(CMsgBase* pMsg,CRole& role,SSMsgHead *pHeader){}
	virtual void OnEvent(CMsgBase* pMsg,uint64_t key,SSMsgHead *pHeader){}
};
struct CMsgHandle;
typedef void (CMsgHandleBase::*EventType1)(CMsgBase* pMsg,CRole& role,SSMsgHead *pHeader);
typedef void (CMsgHandleBase::*EventType2)(CMsgBase* pMsg,uint64_t key,SSMsgHead *pHeader);
typedef void (*EventType3)(CMsgBase* pMsg,CRole& role,SSMsgHead *pHeader);
typedef void (*EventType4)(CMsgBase* pMsg,uint64_t key,SSMsgHead *pHeader);
typedef void (CMsgHandle::*EventType5)(CMsgBase* pMsg,SSMsgHead *pHeader);
typedef CMsgHandle&(CRole::*HandleFun)();
#endif//_MSGBASE_H_