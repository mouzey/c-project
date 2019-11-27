#include "pch.h"
#include "recvsignal.h"
#include "sendsignal.h"
CRecvSignal::CRecvSignal()
{
}
CRecvSignal::~CRecvSignal()
{
	SignalConstIter it = m_pSendSignal.begin();
	for (; it != m_pSendSignal.end();++it)
	{
		it->m_pSendSignal->RemoveSignal(it->m_nSignalType,this);
	}
	m_pSendSignal.clear();
}
bool CRecvSignal::RegisteredSignal( uint32_t nSignalType,CSendSignal*pSendSignal)
{
	CSignalKey stKey(nSignalType,pSendSignal);
	 return m_pSendSignal.insert(stKey).second;
}
void CRecvSignal::RemoveSignal( uint32_t nSignalType,CSendSignal*pSendSignal)
{
	CSignalKey stKey(nSignalType,pSendSignal);
#ifdef _DEBUG_
	if(unlikely(0 == m_pSendSignal.erase(stKey)))
		printf("remove has error\n");
#else
	m_pSendSignal.erase(stKey);
#endif
}
bool CRecvSignal::DeleteSignal( uint32_t nSignalType,CSendSignal *pSendSignal)
{
	CSignalKey stKey(nSignalType,pSendSignal);
	SignalConstIter it = m_pSendSignal.find(stKey);
	if (likely(it != m_pSendSignal.end()))
	{
		it->m_pSendSignal->RemoveSignal(nSignalType,this);
		m_pSendSignal.erase(it);
		return true;
	}
	return false;
}