#ifndef _H_TIMER_BASE_H_
#define _H_TIMER_BASE_H_
#include <set>
#include "define.h"
//定时器基类
class CTimerManager;
class CDateTime;
class CTimerBase
{
public:
	friend CTimerManager;
	CTimerBase();
	virtual ~CTimerBase();
	/*TimerID = 0无效 type 自定义类型 nData 可以写自定义的数据*/
	/*nTime 第一次定时器触发时间间隔 微妙/毫秒 nCycleTime = 循环定时器时间 0表示只触发一次 微妙/毫秒*/
	TimerID AddIntervalTimer(TimerType type,uint64_t nTime,uint64_t nCycleTime=0,TimerData data =uint64_t(0));
	/*TimerID = 0无效 type 自定义类型 nData 可以写自定义的数据*/
	/*nTime 第一次定时器触发时间戳 微妙/毫秒 nCycleTime = 循环定时器时间 0表示只触发一次 微妙/毫秒*/
	TimerID AddTimestampTimer(TimerType type,uint64_t nTime,uint64_t nCycleTime=0,TimerData data =uint64_t(0));
	/*TimerID = 0无效 type 自定义类型 nData 可以写自定义的数据*/
	/*nTime 第一次定时器触发时间戳 单位秒 nCycleTime = 循环定时器时间 0表示只触发一次 单位秒*/
	TimerID AddTimerSec(TimerType type,uint32_t nTime,uint32_t nCycleTime=0,TimerData data =uint64_t(0));
	uint32_t DeleteAllTimer();
	bool DeleteTimer(TimerID timerID);
	bool HasTimer(TimerID timerID)const;
	uint32_t GetTimerIdSize()const { return (uint32_t)m_TimerID.size(); }
public:
	bool HasTimer(TimerType type)const;//这个接口性能不高如果需要可以将m_TimerID修改为map
	//需要继承的定时器处理函数
	virtual void OnTimerEvent(const TimerDataInfo& data,const CDateTime& dt)=0;
private:
	bool CancelTimer(TimerID timerID);
private:
	//用set保存所有定时器id
	std::set<TimerID> m_TimerID;
};
template<typename T>
class COnlyTimerEvent:public CTimerBase
{
public:
	virtual void OnTimerEvent(const TimerDataInfo& data, const CDateTime& dt) override
	{
		T::Instance().OnTimerEvent(data, dt);
	}
};
class CTimterEventImpl:public CTimerBase
{
public:
	CTimterEventImpl(CTimerBase* impl) :m_impl(impl) {}
	virtual void OnTimerEvent(const TimerDataInfo& data, const CDateTime& dt) override
	{
		m_impl->OnTimerEvent(data, dt);
	}
private:
	CTimerBase* m_impl;
};
#endif