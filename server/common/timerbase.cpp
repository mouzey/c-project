#include "pch.h"
#include "timerbase.h"
#include "timermanager.h"
CTimerBase::CTimerBase()
{
}

CTimerBase::~CTimerBase()
{
	DeleteAllTimer();
}
TimerID CTimerBase::AddIntervalTimer(TimerType type,uint64_t nTime,uint64_t nCycleTime/*=0*/,TimerData data/*=0*/)
{
	TimerID timerID= CTimerManager::GetInstance().AddTimer(this,type,nTime,nCycleTime,data);
	m_TimerID.emplace(timerID);
	return timerID;
}
TimerID CTimerBase::AddTimestampTimer(TimerType type,uint64_t nTime,uint64_t nCycleTime/*=0*/,TimerData data/*=0*/)
{
	TimerID timerID= CTimerManager::GetInstance().AddTimerEx(this,type,nTime,nCycleTime,data);
	m_TimerID.emplace(timerID);
	return timerID;
}
TimerID CTimerBase::AddTimerSec(TimerType type,uint32_t nTime,uint32_t nCycleTime/*=0*/,TimerData data /*=uint64_t(0)*/)
{
	uint64_t nTemp = uint64_t(nTime)*1000* TIMERBASE;
	uint64_t nCycle = uint64_t(nCycleTime)*1000* TIMERBASE;
	TimerID timerID= CTimerManager::GetInstance().AddTimerEx(this,type,nTemp,nCycle,data);
	m_TimerID.emplace(timerID);
	return timerID;
}
bool CTimerBase::DeleteTimer(TimerID timerID)
{
	auto iter = m_TimerID.find(timerID);
	if(iter == m_TimerID.end())
		return false;
	CTimerManager::GetInstance().DeleteTimer(timerID);
	m_TimerID.erase(iter);
	return true;
}
uint32_t CTimerBase::DeleteAllTimer()
{
	for (auto iter = m_TimerID.begin(); iter != m_TimerID.end();++iter)
	{
		CTimerManager::GetInstance().DeleteTimer(*iter);
	}
	uint32_t nCount = static_cast<uint32_t>(m_TimerID.size());
	m_TimerID.clear();
	return nCount;
}
bool CTimerBase::CancelTimer(TimerID timerID)
{
	auto iter = m_TimerID.find(timerID);
	if(iter == m_TimerID.end())
		return false;
	m_TimerID.erase(iter);
	return true;
}
bool CTimerBase::HasTimer(TimerID timerID)const
{
	auto iter = m_TimerID.find(timerID);
	if(iter == m_TimerID.end())
		return false;
	return true;
}
bool CTimerBase::HasTimer(TimerType type)const
{
	for (auto iter = m_TimerID.begin(); iter != m_TimerID.end();++iter)
	{
		if (CTimerManager::GetInstance().GetTimerType(*iter) == type)
		{
			return true;
		}
	}
	return false;
}