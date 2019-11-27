#include "pch.h"
#include "threadevent.h"
CThreadEvent::CThreadEvent():m_impl(this)
{
}

CThreadEvent::~CThreadEvent()
{
}
void CThreadEvent::OnTimerEvent(const TimerDataInfo& data, const CDateTime& dt)
{
	while (!m_listEvent.empty() && m_EventQueue.Push(m_listEvent.front()) == S_OK)
		m_listEvent.pop_front();
	if (m_listEvent.empty())
		m_impl.AddIntervalTimer(TimerType::enmTimerType_FunEvent, 50);
}
int32_t CThreadEvent::Post(CFuncEventBase* pFunc)
{
	//加临时队列配合定时器处理队列满的情况
	if (!m_listEvent.empty() || m_EventQueue.Push(pFunc) != S_OK)
	{
		if (m_listEvent.empty())
			m_impl.AddIntervalTimer(TimerType::enmTimerType_FunEvent,50);
		m_listEvent.emplace_back(pFunc);
	}
	return S_OK;
}