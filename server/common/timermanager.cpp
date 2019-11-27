#include "pch.h"
#include "timermanager.h"
#include "timerbase.h"
#include "datetime.h"
CTimerManager::CTimerManager()
{
	m_CurrTimerID = 0;
}
CTimerManager::~CTimerManager()
{
}
TimerType CTimerManager::GetTimerType(TimerID timerID)const
{
	auto iter = m_TimerIDMap.find(timerID);
	if(iter == m_TimerIDMap.end())
		return TimerType::enmTimerType_None;
	auto range_iter = m_TimerQueue.equal_range(iter->second);
	for (auto it = range_iter.first; it != range_iter.second;++it)
	{
		if (it->second.id == timerID)
		{
			return it->second.type;
		}
	}
	return TimerType::enmTimerType_None;
}
void CTimerManager::DeleteTimer(TimerID timerID)
{
	auto iter = m_TimerIDMap.find(timerID);
	if(iter != m_TimerIDMap.end())
	{
		auto range_iter = m_TimerQueue.equal_range(iter->second);
		for (auto it = range_iter.first; it != range_iter.second;++it)
		{
			if (it->second.id == timerID)
			{
				m_TimerQueue.erase(it);
				break;
			}
		}
		m_TimerIDMap.erase(iter);
	}
}
TimerID CTimerManager::AddTimer(CTimerBase* pHandle,TimerType type,uint64_t nTime,uint64_t nCycleTime,TimerData data)
{
	TimerDataInfo stData;
	stData.id = GetNextTimerID();
	stData.nCycleTime = nCycleTime;
	stData.type = type;
	stData.pHandle = pHandle;
	stData.data = data;
	nTime += CDateTime::CurrDateTime().GetMicroseconds()/ TIMERBASE;
	m_TimerQueue.emplace(nTime,stData);
	m_TimerIDMap.emplace(stData.id,nTime);
	return stData.id;
}
TimerID CTimerManager::AddTimerEx(CTimerBase* pHandle,TimerType type,uint64_t nTime,uint64_t nCycleTime,TimerData data)
{
	TimerDataInfo stData;
	stData.id = GetNextTimerID();
	stData.nCycleTime = nCycleTime;
	stData.type = type;
	stData.pHandle = pHandle;
	stData.data = data;
	m_TimerQueue.emplace(nTime,stData);
	m_TimerIDMap.emplace(stData.id,nTime);
	return stData.id;
}
bool CTimerManager::DoTimerEvent()
{
	CDateTime dt = CDateTime::CurrDateTime();
	auto nowTime = dt.GetMicroseconds() / TIMERBASE;
	uint32_t nCount = 0;
	while (++nCount < 50 && !m_TimerQueue.empty())
	{
		auto iter = m_TimerQueue.begin();
		if(iter->first > nowTime)
			break;
		TimerDataInfo &stData = iter->second;
		if (stData.nCycleTime  > 0)
		{
			uint64_t nEndTime = nowTime +stData.nCycleTime;
			m_TimerQueue.emplace(nEndTime,stData);
			m_TimerIDMap[stData.id] = nEndTime;
		}
		else
		{
			m_TimerIDMap.erase(stData.id);
			stData.pHandle->CancelTimer(stData.id);
		}
		//OnTimerEvent可能删除pHandle所以CancelTimer调用要在前面
		stData.pHandle->OnTimerEvent(stData,dt);
		m_TimerQueue.erase(iter);
	}
	return nCount < 50;
}
bool CTimerManager::CancelTimer(TimerID timerID)
{
	auto iter = m_TimerIDMap.find(timerID);
	if(iter != m_TimerIDMap.end())
	{
		auto range_iter = m_TimerQueue.equal_range(iter->second);
		for (auto it = range_iter.first; it != range_iter.second;++it)
		{
			if (it->second.id == timerID)
			{
				it->second.pHandle->CancelTimer(timerID);
				m_TimerQueue.erase(it);
				break;
			}
		}
		m_TimerIDMap.erase(iter);
		return true;
	}
	return false;
}
bool CTimerManager::HasTimer(TimerID timerID)const
{
	auto iter = m_TimerIDMap.find(timerID);
	if(iter != m_TimerIDMap.end())
		return true;
	return false;
}