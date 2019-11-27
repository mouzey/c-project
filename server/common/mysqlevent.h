#ifndef _MYSQL_EVENT_H_
#define _MYSQL_EVENT_H_
#include<memory>
#include "define.h"
#include "newqueue.h"
#include "functionevent.h"
//数据库事件
class CMysqlManager;
class CMysqlCallBack;
class CMysqlEventBase
{
public:
	CMysqlEventBase(const char *pszSql,const uint32_t nSize,ServerID id);
	CMysqlEventBase();
	virtual ~CMysqlEventBase();
	char* GetSql(){return m_szSql;}
	uint32_t GetSize()const{return m_nSize;}
	virtual void CallBack(CMysqlManager* pManager,bool succ){}
	virtual void Delete(){delete this;}
	virtual bool StoreRet(){return false;}
	virtual void PopSize(){}
	virtual void PopSizeEx(){}
	ServerID GetServerId()const{return m_serverId;}
	void PushEvent(CFuncEventBase* pEvent, CMysqlManager* pManager);
protected:
	char* m_szSql;
	uint32_t m_nSize;
	ServerID m_serverId;
	//多线程需要在这里加一个线程id之类的东西 CallBack后把CFuncEventBase push到对应的线程CThread::GetLocalThreadType()
};
class CMysqlEventEmpty:public CMysqlEventBase
{
public:
	CMysqlEventEmpty();
	~CMysqlEventEmpty();
	virtual void Delete()override{}
	virtual void PopSize()override{m_queue.Pop(m_nSize);}
	virtual void PopSizeEx()override{m_queue.Pop(m_nSize);m_pServerId->Pop(m_serverId);}
	void Push(uint32_t val,ServerID id){m_queue.Push(val); if(m_pServerId)m_pServerId->Push(id);}
	bool IsFull()const{return m_queue.IsFull();}
	void NewServerID(){m_pServerId = new CNewQueue<ServerID,10000>;}
private:
	CNewQueue<uint32_t,10000> m_queue;
	CNewQueue<ServerID,10000> *m_pServerId;
};
class CMysqlEventCommon:public CMysqlEventBase
{
public:
	CMysqlEventCommon(const char *pszSql,const uint32_t nSize,CMysqlCallBack* pPtr,ServerID id)
		:CMysqlEventBase(pszSql,nSize,id)
		,m_pCallBack(pPtr)
	{}
	virtual void CallBack(CMysqlManager* pManager,bool succ)override;
private:
	CMysqlCallBack* m_pCallBack;
};
class CMysqlEventComData:public CMysqlEventBase
{
public:
	CMysqlEventComData(const char *pszSql,const uint32_t nSize,const FuncData data,CMysqlCallBack* pPtr,ServerID id)
		:CMysqlEventBase(pszSql,nSize,id)
		, m_pCallBack(pPtr)
		,m_data(data)
	{}
	virtual void CallBack(CMysqlManager* pManager,bool succ)override;
private:
	CMysqlCallBack* m_pCallBack;
	FuncData m_data;
};
class CMysqlEventInsert:public CMysqlEventBase
{
public:
	CMysqlEventInsert(const char *pszSql,const uint32_t nSize,CMysqlCallBack* pPtr,ServerID id)
		:CMysqlEventBase(pszSql,nSize,id)
		,m_pCallBack(pPtr)
	{}
	virtual void CallBack(CMysqlManager* pManager,bool succ)override;
private:
	CMysqlCallBack* m_pCallBack;
};
class CMysqlEventInsertData:public CMysqlEventBase
{
public:
	CMysqlEventInsertData(const char *pszSql,const uint32_t nSize,const FuncData data,CMysqlCallBack* pPtr,ServerID id)
		:CMysqlEventBase(pszSql,nSize,id)
		, m_pCallBack(pPtr)
		,m_data(data)
	{}
	virtual void CallBack(CMysqlManager* pManager,bool succ)override;
private:
	CMysqlCallBack* m_pCallBack;
	FuncData m_data;
};
template <class HandleType>
class CMysqlEventFunc:public CMysqlEventBase
{
public:
	CMysqlEventFunc(const char *pszSql,const uint32_t nSize,HandleType handle,bool storeRet,ServerID id)
		:CMysqlEventBase(pszSql,nSize,id)
		,m_Handle(handle)
		,m_storeRet(storeRet)
	{}
	virtual void CallBack(CMysqlManager* pManager,bool succ)override
	{
		m_Handle(pManager,succ);
	}
	virtual bool StoreRet()override{return m_storeRet;}
private:
	HandleType m_Handle;
	bool m_storeRet;
};
template <class HandleType>
class CMysqlEventFuncData:public CMysqlEventBase
{
public:
	CMysqlEventFuncData(const char *pszSql,const uint32_t nSize,HandleType handle,const FuncData data,bool storeRet,ServerID id)
		:CMysqlEventBase(pszSql,nSize,id)
		,m_Handle(handle)
		,m_data(data)
		,m_storeRet(storeRet)
	{}
	virtual void CallBack(CMysqlManager* pManager,bool succ)override
	{
		m_Handle(pManager,m_data,succ);
	}
	virtual bool StoreRet()override{return m_storeRet;}
private:
	HandleType m_Handle;
	FuncData m_data;
	bool m_storeRet;
};
template <typename T>
class CMysqlEventSelect:public CMysqlEventBase
{
public:
	CMysqlEventSelect(const char *pszSql,const uint32_t nSize,CMysqlCallBack* pPtr,ServerID id)
		:CMysqlEventBase(pszSql,nSize,id)
		,m_pCallBack(pPtr)
	{}
	virtual void CallBack(CMysqlManager* pManager,bool succ)override
	{
		if (succ)
		{
			std::shared_ptr<CMysqlRecord> record = std::make_shared<T>();
			record->SaveRecord(pManager);
			PushEvent(new CMysqlSelect(m_pCallBack,&record,true),pManager);
		}
		else
			PushEvent(new CMysqlSelect(m_pCallBack,nullptr,false),pManager);
	}
	virtual bool StoreRet()override{return true;}
private:
	CMysqlCallBack* m_pCallBack;
};
template <typename T>
class CMysqlEventSelectData:public CMysqlEventBase
{
public:
	CMysqlEventSelectData(const char *pszSql,const uint32_t nSize,CMysqlCallBack* pPtr,const FuncData data,ServerID id)
		:CMysqlEventBase(pszSql,nSize,id)
		,m_pCallBack(pPtr)
		,m_data(data)
	{}
	virtual void CallBack(CMysqlManager* pManager,bool succ)override
	{
		if (succ)
		{
			std::shared_ptr<CMysqlRecord> record = std::make_shared<T>();
			record->SaveRecord(pManager);
			PushEvent(new CMysqlSelectData(m_pCallBack,&record,m_data,true), pManager);
		}
		else
			PushEvent(new CMysqlSelectData(m_pCallBack,nullptr,m_data,false), pManager);
	}
	virtual bool StoreRet()override{return true;}
private:
	CMysqlCallBack* m_pCallBack;
	FuncData m_data;
};
class CMysqlEventSelectEx:public CMysqlEventBase
{
public:
	CMysqlEventSelectEx(const char *pszSql,const uint32_t nSize,CMysqlCallBack* pPtr,uint32_t nCase,ServerID id)
		:CMysqlEventBase(pszSql,nSize,id)
		,m_pCallBack(pPtr)
		,m_nCase(nCase)
	{}
	virtual void CallBack(CMysqlManager* pManager,bool succ)override;
	virtual bool StoreRet()override{return true;}
private:
	CMysqlCallBack* m_pCallBack;
	uint32_t m_nCase;
};
class CMysqlEventSelectDataEx:public CMysqlEventBase
{
public:
	CMysqlEventSelectDataEx(const char *pszSql,const uint32_t nSize,CMysqlCallBack* pPtr,const FuncData data,uint32_t nCase,ServerID id)
		:CMysqlEventBase(pszSql,nSize,id)
		,m_pCallBack(pPtr)
		,m_data(data)
		,m_nCase(nCase)
	{}
	virtual void CallBack(CMysqlManager* pManager,bool succ)override;
	virtual bool StoreRet()override{return true;}
private:
	CMysqlCallBack* m_pCallBack;
	FuncData m_data;
	uint32_t m_nCase;
};
#endif