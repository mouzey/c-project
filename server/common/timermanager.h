#ifndef _H_TIMER_MANAGER_H_
#define _H_TIMER_MANAGER_H_
#include <map>
#include "localsingleton.h"
//定时器管理器以微秒/毫秒为单位
class CTimerManager:public CLocalSingleton<CTimerManager>
{
public:
	friend CTimerBase;
	CTimerManager();
	~CTimerManager();
private:
	TimerType GetTimerType(TimerID timerID)const;
	void DeleteTimer(TimerID timerID);
	//时间参数为微妙/毫秒
	TimerID AddTimer(CTimerBase* pHandle,TimerType type,uint64_t nTime,uint64_t nCycleTime,TimerData data);
	TimerID AddTimerEx(CTimerBase* pHandle,TimerType type,uint64_t nTime,uint64_t nCycleTime,TimerData data);
	TimerID GetNextTimerID(){return ++m_CurrTimerID;}
public:
	bool DoTimerEvent();
	bool CancelTimer(TimerID timerID);//取消定时器
	bool HasTimer(TimerID timerID)const;
private:
	std::multimap<uint64_t,TimerDataInfo> m_TimerQueue;
	std::map<TimerID,uint64_t> m_TimerIDMap;
	TimerID m_CurrTimerID;
};
#endif