#ifndef _SEND_SIGNAL_H_
#define _SEND_SIGNAL_H_
#include "define.h"
#define  USEMAP
#ifndef USEMAP
#include <set>
#else
#include <map>
#endif
//发送信号
class CRecvSignal;
class CSendSignal
{
public:
	friend  class CRecvSignal;
	CSendSignal( uint32_t nSignalNum=0);
	virtual ~CSendSignal();
	//信号注册函数pSignal接收信号对象，nSignalType信号类型自己是发送信号者
	bool AddSignal(CRecvSignal *pSignal, uint32_t nSignalType);
	//信号类型数
	uint32_t GetSignalTypeNum()const;
	//指定类型信号接收者数量
	uint32_t GetRecvSignalNum( uint32_t nSignalType)const;
private:
	void RemoveSignal(uint32_t nSignalType,CRecvSignal* pSignal);
	void RemoveSignal();
public://信号触发函数，全部定义为const函数
	virtual void SendOnlineSignal(uint32_t nValue) const;
	virtual void SendOfflineSignal(uint32_t nValue)const;
	virtual void SendOnlineSignalEX(uint32_t nValue) const;
	virtual void SendOfflineSignalEX(uint32_t nValue)const;
	template<class F,class...Args>//信号触发的时候不能删除信号
	void OnSignal(uint16_t type,F&& f,Args...args)
	{
		auto sigIter = m_pRecvSignal.find(type);
		if (sigIter == m_pRecvSignal.end())return;
		++m_nOnSignalNum;
		for (auto iter = sigIter->second.begin(); iter != sigIter->second.end();++iter)
		{
			if(iter->second)
				(iter->first->*f)(std::forward<Args>(args)...);
		}
		--m_nOnSignalNum;
		RemoveSignal();
	}
private:
	//如果signaltype.h中所有的信号都在一个结构体中定义而不是分开定义那么这里用map 而不是动态数组
	//因为那样会导致动态数组很大但是用到的信号很少浪费空间unordered_set更好
	typedef std::map<CRecvSignal*,bool>::const_iterator	SignalConstIter;
#ifndef USEMAP
	typedef std::set<CRecvSignal*>			SignalVector;
	SignalVector *m_pRecvSignal;
	const uint32_t m_nSignalNum;
	SignalConstIter m_iter;
	uint16_t m_type;
#else
	typedef std::map<uint32_t,std::map<CRecvSignal*,bool> >			SignalVector;
	SignalVector m_pRecvSignal;
#endif
	uint16_t m_nOnSignalNum = 0;
	std::multimap<uint32_t, CRecvSignal*>m_delSignal;
};
#endif