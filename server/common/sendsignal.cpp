#include "pch.h"
#include <assert.h>
#include "recvsignal.h"
#include "sendsignal.h"
#include "signaltype.h"
#ifndef USEMAP
CSendSignal::CSendSignal(uint32_t nSignalNum):m_nSignalNum(nSignalNum)
{
	m_pRecvSignal = new SignalVector[nSignalNum];
	ASSERT_FRAME(m_pRecvSignal != NULL);
}
CSendSignal::~CSendSignal()
{
	for (uint32_t i = 0; i < m_nSignalNum;++i)
	{
		SignalConstIter it = m_pRecvSignal[i].begin();
		for (; it != m_pRecvSignal[i].end();++it)
		{
			(*it)->RemoveSignal(i,this);
		}
		m_pRecvSignal[i].clear();
	}
	delete [] m_pRecvSignal;
}
bool CSendSignal::AddSignal(CRecvSignal *pSignal, uint32_t nSignalType)
{
	if(unlikely(!pSignal->RegisteredSignal(nSignalType,this)))
		return false;
	if(unlikely(!m_pRecvSignal[nSignalType].insert(pSignal).second))
	{
		pSignal->RemoveSignal(nSignalType,this);
		return false;
	}
	return true;
}
void CSendSignal::RemoveSignal( uint32_t nSignalType,CRecvSignal* pSignal)
{
#ifdef _DEBUG_
	if(0 == m_pRecvSignal[nSignalType].erase(pSignal))
		printf("remove has error\n");
#else
	if (nSignalType == m_type)
	{
		auto iter = m_pRecvSignal[nSignalType].find(pSignal);
		if(iter != m_pRecvSignal[nSignalType].end())
		{
			if(iter == m_iter)
				++m_iter;
			m_pRecvSignal[nSignalType].erase(iter);
		}
	}
	else
	{
		m_pRecvSignal[nSignalType].erase(pSignal);
	}
	
#endif
}
void CSendSignal::SendOnlineSignal(uint32_t nValue)const
{
	SignalConstIter it = m_pRecvSignal[SignalTypeLogin::ONLINE_SNG].begin();
	for (; it != m_pRecvSignal[SignalTypeLogin::ONLINE_SNG].end();++it)
	{
		(*it)->Online(nValue);
	}
}
 void CSendSignal::SendOfflineSignal(uint32_t nValue)const
{
	SignalConstIter it = m_pRecvSignal[SignalTypeLogin::OFFLIN_SGN].begin();
	for (; it != m_pRecvSignal[SignalTypeLogin::OFFLIN_SGN].end();++it)
	{
		(*it)->Offline(nValue);
	}
}
 void CSendSignal::SendOnlineSignalEX(uint32_t nValue)const
 {
	 SignalConstIter it = m_pRecvSignal[SignalTypeRole::ONLINE_SNG].begin();
	 for (; it != m_pRecvSignal[SignalTypeRole::ONLINE_SNG].end();++it)
	 {
		 (*it)->OnlineEX(nValue);
	 }
 }
 void CSendSignal::SendOfflineSignalEX(uint32_t nValue)const
 {
	 SignalConstIter it = m_pRecvSignal[SignalTypeRole::OFFLIN_SGN].begin();
	 for (; it != m_pRecvSignal[SignalTypeRole::OFFLIN_SGN].end();++it)
	 {
		 (*it)->OfflineEX(nValue);
	 }
 }
 uint32_t CSendSignal::GetRecvSignalNum(const uint32_t nSignalType)const
 {
	 return m_pRecvSignal[nSignalType].size();
 }
 uint32_t CSendSignal::GetSignalTypeNum()const
 {
	 return m_nSignalNum;
 }
#else
CSendSignal::CSendSignal( uint32_t nSignalNum)
{
}
CSendSignal::~CSendSignal()
{
	for (SignalVector::iterator iter = m_pRecvSignal.begin(); iter != m_pRecvSignal.end();++iter)
	{
		auto it = iter->second.begin();
		for (;it != iter->second.end();++it)
		{
			(it->first)->RemoveSignal(iter->first,this);
		}
		iter->second.clear();
	}
	m_pRecvSignal.clear();
}
bool CSendSignal::AddSignal(CRecvSignal *pSignal, uint32_t nSignalType)
{
	if(unlikely(!pSignal->RegisteredSignal(nSignalType,this)))
		return false;
	if(unlikely(!m_pRecvSignal[nSignalType].emplace(pSignal,true).second))
	{
		pSignal->RemoveSignal(nSignalType,this);
		return false;
	}
	return true;
}
void CSendSignal::RemoveSignal( uint32_t nSignalType,CRecvSignal* pSignal)
{
	SignalVector::iterator iter = m_pRecvSignal.find(nSignalType);
	if(unlikely(iter == m_pRecvSignal.end()))
	{
#ifdef _DEBUG_
		printf("remove has error\n");
#endif
		return ;
	}
#ifdef _DEBUG_
	if(unlikely(0 == iter->second.erase(pSignal)))
		printf("remove has error\n");
#else
	if (0 == m_nOnSignalNum)
		iter->second.erase(pSignal);
	else
	{
		auto sigIter = iter->second.find(pSignal);
		if (sigIter != iter->second.end())
		{
			m_delSignal.emplace(nSignalType, pSignal);
			sigIter->second = false;
		}
	}
#endif
}
void CSendSignal::RemoveSignal()
{
	if (0 != m_nOnSignalNum || m_delSignal.empty())return;
	for (auto& iter : m_delSignal)
		RemoveSignal(iter.first, iter.second);
	m_delSignal.clear();
}
void CSendSignal::SendOnlineSignal(uint32_t nValue)const
{
	SignalVector::const_iterator iter = m_pRecvSignal.find(SignalTypeLogin::ONLINE_SNG);
	if(iter == m_pRecvSignal.end())
		return;
	SignalConstIter it = iter->second.begin();
	for (; it != iter->second.end();++it)
	{
		(it->first)->Online(nValue);
	}
}
void CSendSignal::SendOfflineSignal(uint32_t nValue)const
{
	SignalVector::const_iterator iter = m_pRecvSignal.find(SignalTypeLogin::OFFLIN_SGN);
	if(iter == m_pRecvSignal.end())
		return;
	SignalConstIter it = iter->second.begin();
	for (; it != iter->second.end();++it)
	{
		(it->first)->Offline(nValue);
	}
}
void CSendSignal::SendOnlineSignalEX(uint32_t nValue)const
{
	SignalVector::const_iterator iter = m_pRecvSignal.find(SignalTypeRole::ONLINE_SNG);
	if(iter == m_pRecvSignal.end())
		return;
	SignalConstIter it = iter->second.begin();
	for (; it != iter->second.end();++it)
	{
		(it->first)->OnlineEX(nValue);
	}
}
void CSendSignal::SendOfflineSignalEX(uint32_t nValue)const
{
	SignalVector::const_iterator iter = m_pRecvSignal.find(SignalTypeRole::OFFLIN_SGN);
	if(iter == m_pRecvSignal.end())
		return;
	SignalConstIter it = iter->second.begin();
	for (; it != iter->second.end();++it)
	{
		(it->first)->OfflineEX(nValue);
	}
}
uint32_t CSendSignal::GetRecvSignalNum(const uint32_t nSignalType)const
{
	SignalVector::const_iterator iter = m_pRecvSignal.find(nSignalType);
	if(iter != m_pRecvSignal.end())
		return (uint32_t)iter->second.size();
	return 0;
}
uint32_t CSendSignal::GetSignalTypeNum()const
{
	return (uint32_t)m_pRecvSignal.size();
}
#endif