#ifndef _H_THREAD_EVENT_H_
#define _H_THREAD_EVENT_H_
#include "newqueue.h"
#include "functionevent.h"
#include "thread.h"
#include"timerbase.h"
#include<list>
//线程之间的post
class CThreadEvent:public CTimerBase
{
public:
	CThreadEvent();
	~CThreadEvent();
	virtual void OnTimerEvent(const TimerDataInfo& data, const CDateTime& dt);
	bool IsEmpty()const {return m_EventQueue.IsEmpty();}
	int32_t Post(CFuncEventBase* pFunc);
	int32_t Pop(CFuncEventBase*& pFunc)
	{
		return m_EventQueue.Pop(pFunc);
	}
	template< class Handler>
	int32_t Post(Handler handle)
	{
		return Post(new CFunctionEvent<Handler>(handle));
	}
	template< class Handler>
	int32_t Post(Handler handle,FuncData data)
	{
		return Post(new CFuncEventData<Handler>(handle,data));
	}
	template< class Handler,class E>
	int32_t Post(Handler handle)
	{
		return Post(new CFuncEventClass<Handler,E>(handle));
	}
	template<class Handler,class E>
	int32_t Post(Handler handle,FuncData data)
	{
		return Post(new CFuncEventClassData<Handler,E>(handle,data));
	}
	template< class Handler,class E>
	int32_t Post(Handler handle,E* ptr)
	{
		return Post(new CFuncEventPtr<Handler,E>(handle,ptr));
	}
	template<class Handler,class E>
	int32_t Post(Handler handle,E* ptr,FuncData data)
	{
		return Post(new CFuncEventPtrData<Handler,E>(handle,ptr,data));
	}
	template< class Handler,class E>
	int32_t Post(Handler handle,std::shared_ptr<E>& ptr)
	{
		return Post(new CFuncEventWPtr<Handler,E>(handle,ptr));
	}
	template<class Handler,class E>
	int32_t Post(Handler handle,std::shared_ptr<E>& ptr,FuncPtrData data)
	{
		return Post(new CFuncEventWPtrData<Handler,E>(handle,ptr,data));
	}
	template< class Handler,class E>
	int32_t Post(Handler handle,std::shared_ptr<E>& ptr,SharedPtrTag)
	{
		return Post(new CFuncEventSPtr<Handler,E>(handle,ptr));
	}
	template<class Handler,class E>
	int32_t Post(Handler handle,std::shared_ptr<E>& ptr,FuncData data)
	{
		return Post(new CFuncEventSPtrData<Handler,E>(handle,ptr,data));
	}
private:
	CNewQueue<CFuncEventBase*> m_EventQueue;
	std::list<CFuncEventBase*> m_listEvent;//m_EventQueue满了的时候存
	CTimterEventImpl m_impl;//避免多线程使用定时器 如果不会那不用该对象
	//多线程使用的加锁 可以考虑直接使用list加锁 有性能问题再考虑无锁队列 注意多线程添加定时器问题
};
#endif