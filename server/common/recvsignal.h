#ifndef _RECV_SIGNAL_H_
#define _RECV_SIGNAL_H_
#include <set>
#ifndef WIN32
#include <stddef.h>
#endif
#include "define.h"
//接收信号的基类
class CSendSignal;
class CRecvSignal;
struct CSignalKey
{
	const uint32_t m_nSignalType;
	CSendSignal*   m_pSendSignal;
	CSignalKey(uint32_t nSignalType,CSendSignal*pSendSignal):
	  m_nSignalType(nSignalType),
	  m_pSendSignal(pSendSignal)
	  {
	  }
	bool operator <(const CSignalKey &stKey) const
	{
		if(m_nSignalType < stKey.m_nSignalType)
			return true;
		else if(m_nSignalType > stKey.m_nSignalType)
			return false;
		else if(m_pSendSignal < stKey.m_pSendSignal)
			return true;
		return false;
	}
	bool operator ==(const CSignalKey &stKey) const
	{
		if(m_nSignalType == stKey.m_nSignalType && m_pSendSignal == stKey.m_pSendSignal)
			return true;
		return false;
	}
};
struct CHashKey
{
	size_t operator()(const CSignalKey &key) const
	{
		size_t  nValue = key.m_nSignalType;
#ifdef _64_PLATFORM_
		return ((nValue<<32)|(size_t)key.m_pSendSignal);//返回64位
#else 
		return (nValue|(size_t)key.m_pSendSignal);//返回32位
#endif
	}
};
#define RGFUNC(N)virtual void Online##N(uint32_t){}\
	virtual void Offline##N(uint32_t){}\
	virtual void OnlineEX##N(uint32_t){}\
	virtual void OfflineEX##N(uint32_t){}\
	virtual void OnlineA##N(uint32_t){}\
	virtual void OfflineA##N(uint32_t){}\
	virtual void OnlineEXA##N(uint32_t){}\
	virtual void OfflineEXA##N(uint32_t){}\
	virtual void OnlineB##N(uint32_t){}\
	virtual void OfflineB##N(uint32_t){}\
	virtual void OnlineEXB##N(uint32_t){}\
	virtual void OfflineEXB##N(uint32_t){}\
	virtual void OnlineAa##N(uint32_t){}\
	virtual void OfflineAa##N(uint32_t){}\
	virtual void OnlineEXAa##N(uint32_t){}\
	virtual void OfflineEXAa##N(uint32_t){}\
	virtual void OnlineBa##N(uint32_t){}\
	virtual void OfflineBa##N(uint32_t){}\
	virtual void OnlineEXBa##N(uint32_t){}\
	virtual void OfflineEXBa##N(uint32_t){}
class CRecvSignal
{
public:
	friend class CSendSignal;
	virtual ~CRecvSignal();
	CRecvSignal();
	//注册信号的数量
	uint32_t GetSendSignalNum()const{return (uint32_t)m_pSendSignal.size();}
	//信号删除函数nSignalType信号类型pSendSignal发送信号对象自己是接收信号
	bool DeleteSignal(uint32_t nSignalType,CSendSignal *pSendSignal);
private:
	void RemoveSignal(uint32_t nSignalType,CSendSignal*pSendSignal);
	bool RegisteredSignal(uint32_t nSignalType,CSendSignal*pSendSignal);
public://信号的接收函数 需要在具体类中自己实现自己注册的信号对应的虚函数
	RGFUNC(1);
	RGFUNC(2);
	RGFUNC(3);
	RGFUNC(4);
	RGFUNC(5);
	RGFUNC(6);
	RGFUNC(7);
	RGFUNC(8);
	RGFUNC(9);
	RGFUNC(10);
	RGFUNC(11);
	RGFUNC(12);
	RGFUNC(13);
	RGFUNC(14);
	RGFUNC(15);
	RGFUNC(16);
	RGFUNC(17);
	RGFUNC(18);
	RGFUNC(19);
	RGFUNC(20);
	RGFUNC(21);
	RGFUNC(22);
	RGFUNC(23);
	RGFUNC(24);
	RGFUNC(25);
	RGFUNC(26);
	RGFUNC(27);
	RGFUNC(28);
	RGFUNC(29);
	RGFUNC(30);
	RGFUNC(31);
	RGFUNC(32);
	RGFUNC(33);
	RGFUNC(34);
	RGFUNC(35);
	RGFUNC(36);
	RGFUNC(37);
	RGFUNC(38);
	RGFUNC(39);
	RGFUNC(40);
	virtual void Online(uint32_t){}
	virtual void Offline(uint32_t){}
	virtual void OnlineEX(uint32_t){}
	virtual void OfflineEX(uint32_t){}
	virtual void OnlineA(uint32_t){}
	virtual void OfflineA(uint32_t){}
	virtual void OnlineEXA(uint32_t){}
	virtual void OfflineEXA(uint32_t){}
	virtual void OnlineB(uint32_t){}
	virtual void OfflineB(uint32_t){}
	virtual void OnlineEXB(uint32_t){}
	virtual void OfflineEXB(uint32_t){}
private:
	//注册的信号比较少用set 如果很多的话用unordered_set更好
	typedef std::set<CSignalKey>			SignalVector;
	typedef SignalVector::const_iterator	SignalConstIter;
	SignalVector m_pSendSignal;
};
#endif
