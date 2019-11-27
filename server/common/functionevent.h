#ifndef _H_FUNCTION_EVENT_
#define _H_FUNCTION_EVENT_
#include "mysqlcallback.h"
#include <memory>
//用于实现类似于boost中的post函数就是可以从A线程直接post一个Lambda函数到B线程
class CFuncEventBase
{
public:
	CFuncEventBase(){}
	virtual ~CFuncEventBase(){}
	virtual void CallFunc()=0;
	virtual bool Success(){return true;}
};
template <class HandleType>
class CFunctionEvent:public CFuncEventBase
{
public:
	CFunctionEvent(HandleType handle)
		:m_Handle(handle)
	{}
	virtual void CallFunc()override{m_Handle();}
private:
	HandleType m_Handle;
};
template <class HandleType>
class CFuncEventData:public CFuncEventBase
{
public:
	CFuncEventData(HandleType handle,const FuncData data)
		:m_Handle(handle)
		,m_Data(data)
	{}
	virtual void CallFunc()override{m_Handle(m_Data);}
private:
	HandleType m_Handle;
	FuncData m_Data;
};
template <class HandleType,class T>
class CFuncEventClass:public CFuncEventBase
{
public:
	CFuncEventClass(HandleType handle)
		:m_Handle(handle)
	{}
	virtual void CallFunc()override{(T::GetInstance().*m_Handle)();}
private:
	HandleType m_Handle;
};
template <class HandleType,class T>
class CFuncEventClassData:public CFuncEventBase
{
public:
	CFuncEventClassData(HandleType handle,const FuncData data)
		:m_Handle(handle)
		,m_Data(data)
	{
	}
	virtual void CallFunc()override{(T::GetInstance().*m_Handle)(m_Data);}
private:
	HandleType m_Handle;
	FuncData m_Data;
};
template <class HandleType,class T>
class CFuncEventPtr:public CFuncEventBase
{
public:
	CFuncEventPtr(HandleType handle,T* ptr)
		:m_Handle(handle)
		,m_Ptr(ptr)
	{
	}
	virtual void CallFunc()override{m_Ptr->*m_Handle();}
private:
	HandleType m_Handle;
	T* m_Ptr;
};
template <class HandleType,class T>
class CFuncEventPtrData:public CFuncEventBase
{
public:
	CFuncEventPtrData(HandleType handle,T* ptr,const FuncData data)
		:m_Handle(handle)
		,m_Data(data)
		,m_Ptr(ptr)
	{
	}
	virtual void CallFunc()override{m_Ptr->*m_Handle(m_Data);}
private:
	HandleType m_Handle;
	FuncData m_Data;
	T* m_Ptr;
};
template <class HandleType,class T>
class CFuncEventWPtr:public CFuncEventBase
{
public:
	CFuncEventWPtr(HandleType handle,std::shared_ptr<T>& ptr)
		:m_Handle(handle)
		,m_Ptr(ptr)
	{
	}
	virtual void CallFunc()override
	{
		std::shared_ptr<T> pShr = m_Ptr.lock();
		if(nullptr != pShr)
			pShr->*m_Handle();
	}
private:
	HandleType m_Handle;
	std::weak_ptr<T> m_Ptr;
};
template <class HandleType,class T>
class CFuncEventWPtrData:public CFuncEventBase
{
public:
	//FuncPtrData不能有直接new的数据 因为回调函数无法保证一定会回调，如果没有回调那就会内存泄露
	//需要使用动态内存则使用智能指针
	CFuncEventWPtrData(HandleType handle,std::shared_ptr<T> ptr,const FuncPtrData data)
		:m_Handle(handle)
		,m_Data(data)
		,m_Ptr(ptr)
	{
	}
	virtual void CallFunc()override
	{
		std::shared_ptr<T> pShr = m_Ptr.lock();
		if(nullptr != pShr)
			pShr->*m_Handle(m_Data);
	}
private:
	HandleType m_Handle;
	FuncPtrData m_Data;
	std::weak_ptr<T> m_Ptr;
};
template <class HandleType,class T>
class CFuncEventSPtr:public CFuncEventBase
{
public:
	CFuncEventSPtr(HandleType handle,std::shared_ptr<T> ptr)
		:m_Handle(handle)
		,m_Ptr(ptr)
	{
	}
	virtual void CallFunc()override{m_Ptr->*m_Handle();}
private:
	HandleType m_Handle;
	std::shared_ptr<T> m_Ptr;
};
template <class HandleType,class T>
class CFuncEventSPtrData:public CFuncEventBase
{
public:
	CFuncEventSPtrData(HandleType handle,std::shared_ptr<T> ptr,const FuncData data)
		:m_Handle(handle)
		,m_Data(data)
		,m_Ptr(ptr)
	{
	}
	virtual void CallFunc()override{m_Ptr->*m_Handle(m_Data);}
private:
	HandleType m_Handle;
	FuncData m_Data;
	std::shared_ptr<T> m_Ptr;
};
class CMysqlFuncBase:public CFuncEventBase
{
public:
	CMysqlFuncBase(bool succ):m_succ(succ){}
	~CMysqlFuncBase(){}
	virtual bool Success()override{return m_succ;}
private:
	bool m_succ;
};
class CFuncMysql:public CMysqlFuncBase
{
public:
	CFuncMysql(CMysqlCallBack* pCallBack,int32_t nAffectRow,bool succ)
		:CMysqlFuncBase(succ)
		,m_pCallBack(pCallBack)
		,m_nAffectRow(nAffectRow)
	{
	}
	virtual void CallFunc()override{m_pCallBack->CallBack(Success(), m_nAffectRow);}
private:
	CMysqlCallBack* m_pCallBack;
	uint32_t m_nAffectRow;
};
class CFuncMysqlData:public CMysqlFuncBase
{
public:
	CFuncMysqlData(CMysqlCallBack* pCallBack,int32_t nAffectRow,const FuncData data,bool succ)
		:CMysqlFuncBase(succ)
		,m_pCallBack(pCallBack)
		, m_Data(data)
		,m_nAffectRow(nAffectRow)
	{
	}
	virtual void CallFunc()override{m_pCallBack->CallBack(Success(), m_nAffectRow,m_Data);}
private:
	CMysqlCallBack* m_pCallBack;
	FuncData m_Data;
	uint32_t m_nAffectRow;
};
class CMysqlInsert:public CMysqlFuncBase
{
public:
	CMysqlInsert(CMysqlCallBack* pCallBack,int32_t nAffectRow,uint64_t nInsertID,bool succ)
		:CMysqlFuncBase(succ)
		,m_pCallBack(pCallBack)
		, m_nInsertID(nInsertID)
		,m_nAffectRow(nAffectRow)
	{
	}
	virtual void CallFunc()override{m_pCallBack->CallBack(Success(), m_nAffectRow,m_nInsertID);}
private:
	CMysqlCallBack* m_pCallBack;
	uint64_t m_nInsertID;
	uint32_t m_nAffectRow;
};
class CMysqlInsertData:public CMysqlFuncBase
{
public:
	CMysqlInsertData(CMysqlCallBack* pCallBack,int32_t nAffectRow,uint64_t nInsertID,const FuncData data,bool succ)
		:CMysqlFuncBase(succ)
		,m_pCallBack(pCallBack)
		,m_Data(data)
		,m_nInsertID(nInsertID)
		, m_nAffectRow(nAffectRow)
	{
	}
	virtual void CallFunc()override{m_pCallBack->CallBack(Success(), m_nAffectRow,m_nInsertID,m_Data);}
private:
	CMysqlCallBack* m_pCallBack;
	FuncData m_Data;
	uint64_t m_nInsertID;
	uint32_t m_nAffectRow;
};
class CMysqlSelect:public CMysqlFuncBase
{
public:
	CMysqlSelect(CMysqlCallBack* pCallBack,std::shared_ptr<CMysqlRecord>* pRecord,bool succ)
		:CMysqlFuncBase(succ)
		,m_pCallBack(pCallBack)
	{
		if(pRecord)
			m_pRecord = *pRecord;
	}
	virtual void CallFunc()override{m_pCallBack->CallBack(Success(), m_pRecord);}
private:
	CMysqlCallBack* m_pCallBack;
	std::shared_ptr<CMysqlRecord> m_pRecord;
};
class CMysqlSelectData:public CMysqlFuncBase
{
public:
	CMysqlSelectData(CMysqlCallBack* pCallBack,std::shared_ptr<CMysqlRecord>* pRecord,const FuncData data,bool succ)
		:CMysqlFuncBase(succ)
		,m_pCallBack(pCallBack)
		,m_Data(data)
	{
		if(pRecord)
			m_pRecord = *pRecord;
	}
	virtual void CallFunc()override{m_pCallBack->CallBack(Success(), m_pRecord,m_Data);}
private:
	CMysqlCallBack* m_pCallBack;
	FuncData m_Data;
	std::shared_ptr<CMysqlRecord> m_pRecord;
};
typedef void(*FuncType)();
typedef void(*FuncTypeData)(FuncData);
typedef void(*FuncTypeRefData)(FuncData&);
#endif