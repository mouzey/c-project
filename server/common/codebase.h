#ifndef _CODEBASE_H_
#define _CODEBASE_H_
#include <cstring>
#include <array>
#include <bitset>
#include <forward_list>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <vector>
#include <stack>
#include <list>
#include <cassert>
#include "define.h"
#include "errorcode.h"
#include "snprintf.h"
#include "continertraits.h"
#include "msgbase.h"
//编码解码的基础类
class CCodeBase
{
public:
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const bool nData)
	{
		if(unlikely(nFreeSize < 1))
			return E_UNKNOW;
		memcpy(pBuf,&nData,1);
		++pBuf;
		--nFreeSize;
		return 1;
	}
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const uint8_t nData)
	{
		if(unlikely(nFreeSize < 1))
			return E_UNKNOW;
		memcpy(pBuf,&nData,1);
		++pBuf;
		--nFreeSize;
		return 1;
	}
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const int8_t nData)
	{
		if(unlikely(nFreeSize < 1))
			return E_UNKNOW;
		memcpy(pBuf,&nData,1);
		++pBuf;
		--nFreeSize;
		return 1;
	}
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const uint16_t nData)
	{
		if(unlikely(nFreeSize < 2))
			return E_UNKNOW;
		memcpy(pBuf,&nData,2);
		pBuf +=2;
		nFreeSize -= 2;
		return 2;
	}
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const int16_t nData)
	{
		if(unlikely(nFreeSize < 2))
			return E_UNKNOW;
		memcpy(pBuf,&nData,2);
		pBuf +=2;
		nFreeSize -= 2;
		return 2;
	}
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const uint32_t nData)
	{
		if(unlikely(nFreeSize < 4))
			return E_UNKNOW;
		memcpy(pBuf,&nData,4);
		pBuf +=4;
		nFreeSize -= 4;
		return 4;
	}
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const int32_t nData)
	{
		if(unlikely(nFreeSize < 4))
			return E_UNKNOW;
		memcpy(pBuf,&nData,4);
		pBuf +=4;
		nFreeSize -= 4;
		return 4;
	}
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const uint64_t nData)
	{
		if(unlikely(nFreeSize < 8))
			return E_UNKNOW;
		memcpy(pBuf,&nData,8);
		pBuf +=8;
		nFreeSize -=8;
		return 8;
	}
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const int64_t nData)
	{
		if(unlikely(nFreeSize < 8))
			return E_UNKNOW;
		memcpy(pBuf,&nData,8);
		pBuf +=8;
		nFreeSize -= 8;
		return 8;
	}
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const char* pData)
	{
		uint16_t nLen = (uint16_t)strlen(pData);
		if(unlikely(nFreeSize < nLen+2u))
			return E_UNKNOW;
		memcpy(pBuf,&nLen,2);
		pBuf +=2;
		memcpy(pBuf,pData,nLen);
		pBuf +=nLen;
		nLen += 2;
		nFreeSize -= nLen;
		return nLen;
	}
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const char* pData,const uint16_t nLen)
	{
		if(unlikely(nFreeSize < nLen+2u))
			return E_UNKNOW;
		memcpy(pBuf,&nLen,2);
		pBuf +=2;
		memcpy(pBuf,pData,nLen);
		pBuf +=nLen;
		nFreeSize -= nLen+2;
		return nLen+2;
	}
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const uint8_t* pData,const uint16_t nLen)
	{
		if(unlikely(nFreeSize < nLen+2u))
			return E_UNKNOW;
		memcpy(pBuf,&nLen,2);
		pBuf +=2;
		memcpy(pBuf,pData,nLen);
		pBuf +=nLen;
		nFreeSize -= nLen+2;
		return nLen+2;
	}
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const int8_t* pData,const uint16_t nLen)
	{
		if(unlikely(nFreeSize < nLen+2u))
			return E_UNKNOW;
		memcpy(pBuf,&nLen,2);
		pBuf +=2;
		memcpy(pBuf,pData,nLen);
		pBuf +=nLen;
		nFreeSize -= nLen+2;
		return nLen+2;
	}
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const std::string &stData)
	{
		ASSERT_FRAME(stData.size()<=UINT16_MAX);
		uint16_t nLen = (uint16_t)stData.size();
		if(unlikely(nFreeSize < nLen+2u))
			return E_UNKNOW;
		memcpy(pBuf,&nLen,2);
		pBuf +=2;
		memcpy(pBuf,stData.data(),nLen);
		pBuf +=nLen;
		nLen += 2;
		nFreeSize -= nLen;
		return nLen;
	}
	//
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,bool &nData)
	{
		if(unlikely(nSize < 1))
			return E_UNKNOW;
		memcpy(&nData,pBuf,1);
		++pBuf;
		--nSize;
		return 1;
	}
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,uint8_t &nData)
	{
		if(unlikely(nSize < 1))
			return E_UNKNOW;
		memcpy(&nData,pBuf,1);
		++pBuf;
		--nSize;
		return 1;
	}
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,int8_t &nData)
	{
		if(unlikely(nSize < 1))
			return E_UNKNOW;
		memcpy(&nData,pBuf,1);
		++pBuf;
		--nSize;
		return 1;
	}
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,uint16_t &nData)
	{
		if(unlikely(nSize < 2))
			return E_UNKNOW;
		memcpy(&nData,pBuf,2);
		pBuf += 2;
		nSize -= 2;
		return 2;
	}
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,int16_t &nData)
	{
		if(unlikely(nSize < 2))
			return E_UNKNOW;
		memcpy(&nData,pBuf,2);
		pBuf += 2;
		nSize -= 2;
		return 2;
	}
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,uint32_t &nData)
	{
		if(unlikely(nSize < 4))
			return E_UNKNOW;
		memcpy(&nData,pBuf,4);
		pBuf += 4;
		nSize -= 4;
		return 4;
	}
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,int32_t &nData)
	{
		if(unlikely(nSize < 4))
			return E_UNKNOW;
		memcpy(&nData,pBuf,4);
		pBuf += 4;
		nSize -= 4;
		return 4;
	}
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,uint64_t &nData)
	{
		if(unlikely(nSize < 8))
			return E_UNKNOW;
		memcpy(&nData,pBuf,8);
		pBuf += 8;
		nSize -= 8;
		return 8;
	}
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,int64_t &nData)
	{
		if(unlikely(nSize < 8))
			return E_UNKNOW;
		memcpy(&nData,pBuf,8);
		pBuf += 8;
		nSize -= 8;
		return 8;
	}
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,char* pData,const uint32_t nMaxSize)
	{
		if(unlikely(nSize < 2))
			return E_UNKNOW;
		uint16_t nLen = 0;
		memcpy(&nLen,pBuf,2);
		nSize -=2;
		pBuf += 2;
		if(unlikely(nLen > nSize))
			return E_UNKNOW;
		if (unlikely(nLen > nMaxSize))
			return E_LESSMEMORY;
		memcpy(pData,pBuf,nLen);
		pBuf += nLen;
		nSize -= nLen;
		return nLen+2;
	}
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,int8_t* pData,const uint32_t nMaxSize)
	{
		if(unlikely(nSize < 2))
			return E_UNKNOW;
		uint16_t nLen = 0;
		memcpy(&nLen,pBuf,2);
		nSize -=2;
		pBuf += 2;
		if(unlikely(nLen > nSize))
			return E_UNKNOW;
		if (unlikely(nLen > nMaxSize))
			return E_LESSMEMORY;
		memcpy(pData,pBuf,nLen);
		pBuf += nLen;
		nSize -= nLen;
		return nLen+2;
	}
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,uint8_t* pData,const uint32_t nMaxSize)
	{
		if(unlikely(nSize < 2))
			return E_UNKNOW;
		uint16_t nLen = 0;
		memcpy(&nLen,pBuf,2);
		nSize -=2;
		pBuf += 2;
		if(unlikely(nLen > nSize))
			return E_UNKNOW;
		if (unlikely(nLen > nMaxSize))
			return E_LESSMEMORY;
		memcpy(pData,pBuf,nLen);
		pBuf += nLen;
		nSize -= nLen;
		return nLen+2;
	}
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,std::string& stData)
	{
		if(unlikely(nSize < 2))
			return E_UNKNOW;
		uint16_t nLen = 0;
		memcpy(&nLen,pBuf,2);
		nSize -=2;
		pBuf += 2;
		if(unlikely(nLen > nSize))
			return E_UNKNOW;
		stData.append(reinterpret_cast<const char *>(pBuf),nLen);
		pBuf += nLen;
		nSize -= nLen;
		return nLen+2;
	}
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,bool nValue,const char* pName="")
	{
		int32_t nRet = SNPRINTF(pBuf,nSize,"%s%u ",pName,nValue);
		if (likely(nRet > 0))
		{
			pBuf += nRet;
			nSize -= nRet;
		}
		return nRet;
	}
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,int8_t nValue,const char* pName="")
	{
		int32_t nRet = SNPRINTF(pBuf,nSize,"%s%hhd ",pName,nValue);
		if (likely(nRet > 0))
		{
			pBuf += nRet;
			nSize -= nRet;
		}
		return nRet;
	}
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,uint8_t nValue,const char* pName="")
	{
		int32_t nRet =  SNPRINTF(pBuf,nSize,"%s%hhu ",pName,nValue);
		if (likely(nRet > 0))
		{
			pBuf += nRet;
			nSize -= nRet;
		}
		return nRet;
	}
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,int16_t nValue,const char* pName="")
	{
		int32_t nRet =  SNPRINTF(pBuf,nSize,"%s%hd ",pName,nValue);
		if (likely(nRet > 0))
		{
			pBuf += nRet;
			nSize -= nRet;
		}
		return nRet;
	}
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,uint16_t nValue,const char* pName="")
	{
		int32_t nRet =  SNPRINTF(pBuf,nSize,"%s%hu ",pName,nValue);
		if (likely(nRet > 0))
		{
			pBuf += nRet;
			nSize -= nRet;
		}
		return nRet;
	}
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,int32_t nValue,const char* pName="")
	{
		int32_t nRet =  SNPRINTF(pBuf,nSize,"%s%d ",pName,nValue);
		if (likely(nRet > 0))
		{
			pBuf += nRet;
			nSize -= nRet;
		}
		return nRet;
	}
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,uint32_t nValue,const char* pName="")
	{
		int32_t nRet =  SNPRINTF(pBuf,nSize,"%s%u ",pName,nValue);
		if (likely(nRet > 0))
		{
			pBuf += nRet;
			nSize -= nRet;
		}
		return nRet;
	}
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,int64_t nValue,const char* pName="")
	{
#ifdef _64_PLATFORM_
		int32_t nRet =  SNPRINTF(pBuf,nSize,"%s%ld ",pName,nValue);
#else
		int32_t nRet =  SNPRINTF(pBuf,nSize,"%s%lld ",pName,nValue);
#endif // _64_PLATFORM_
		if (likely(nRet > 0))
		{
			pBuf += nRet;
			nSize -= nRet;
		}
		return nRet;
	}
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,uint64_t nValue,const char* pName="")
	{
#ifdef _64_PLATFORM_
		int32_t nRet =  SNPRINTF(pBuf,nSize,"%s%lu ",pName,nValue);
#else
		int32_t nRet =  SNPRINTF(pBuf,nSize,"%s%llu ",pName,nValue);
#endif // _64_PLATFORM_
		if (likely(nRet > 0))
		{
			pBuf += nRet;
			nSize -= nRet;
		}
		return nRet;
	}
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const std::string &str,const char* pName="")
	{
		int32_t nRet =  SNPRINTF(pBuf,nSize,"%s%s ",pName,str.data());
		if (likely(nRet > 0))
		{
			pBuf += nRet;
			nSize -= nRet;
		}
		return nRet;
	}
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const char *pData,const char* pName="")
	{
		int32_t nRet =  SNPRINTF(pBuf,nSize,"%s%s ",pName,pData);
		if (likely(nRet > 0))
		{
			pBuf += nRet;
			nSize -= nRet;
		}
		return nRet;
	}
	static FORCE_INLINE int32_t DumpFormat(char* &pBuf, uint32_t& nSize,const char *pFormat,...)
	{
		va_list ap;
		va_start(ap,pFormat);
		int nRet = SNPRINTF(pBuf,nSize-1,pFormat,ap);
		va_end(ap);
		if(likely(nRet > 0))
		{
			pBuf += nRet;
			nSize -= nRet;
			pBuf[0] ='\0';
		}
		return nRet;
	}
	template<typename T>
	static FORCE_INLINE int32_t EncodeMap(int8_t *&pBuf,uint32_t &nFreeSize,const T& data)
	{
		ASSERT_FRAME(data.size()<=UINT16_MAX);
		int32_t nRet = Encode(pBuf,nFreeSize,uint16_t(data.size()));
		if(nRet < 0) 
			return nRet;
		int32_t nTotal = nRet;
		for (const auto&iter:data)
		{
			nRet = Encode(pBuf,nFreeSize,iter.first);
			if(nRet < 0) 
				return nRet;
			nTotal += nRet;
			nRet = Encode(pBuf,nFreeSize,iter.second);
			if(nRet < 0) 
				return nRet;
			nTotal += nRet;
		}
		return nTotal;
	}
	template<typename T,typename K>
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const std::map<K,T>& data)
	{
		return EncodeMap(pBuf,nFreeSize,data);
	}
	template<typename T,typename K>
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const std::multimap<K,T>& data)
	{
		return EncodeMap(pBuf,nFreeSize,data);
	}
	template<typename T,typename K>
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const std::unordered_map<K,T>& data)
	{
		return EncodeMap(pBuf,nFreeSize,data);
	}
	template<typename T,typename K>
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const std::unordered_multimap<K,T>& data)
	{
		return EncodeMap(pBuf,nFreeSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t EncodeArr(int8_t *&pBuf,uint32_t &nFreeSize,const T& data)
	{
		ASSERT_FRAME(data.size()<=UINT16_MAX);
		int32_t nRet = Encode(pBuf,nFreeSize,uint16_t(data.size()));
		if(nRet < 0) 
			return nRet;
		int32_t nTotal = nRet;
		for (const auto&iter:data)
		{
			nRet = Encode(pBuf,nFreeSize,iter);
			if(nRet < 0) 
				return nRet;
			nTotal += nRet;
		}
		return nTotal;
	}
	template<typename T>
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const std::set<T>& data)
	{
		return EncodeArr(pBuf,nFreeSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const std::multiset<T>& data)
	{
		return EncodeArr(pBuf,nFreeSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const std::unordered_set<T>& data)
	{
		return EncodeArr(pBuf,nFreeSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const std::unordered_multiset<T>& data)
	{
		return EncodeArr(pBuf,nFreeSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const std::vector<T>& data)
	{
		return EncodeArr(pBuf,nFreeSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const std::list<T>& data)
	{
		return EncodeArr(pBuf,nFreeSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const std::deque<T>& data)
	{
		return EncodeArr(pBuf,nFreeSize,data);
	}
	template<typename T,size_t size>
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const std::array<T,size>& data)
	{
		return EncodeArr(pBuf,nFreeSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const std::forward_list<T>& data)
	{
		return EncodeArr(pBuf,nFreeSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const std::stack<T>& data)
	{
		ASSERT_FRAME(data.size()<=UINT16_MAX);
		int32_t nRet = Encode(pBuf,nFreeSize,uint16_t(data.size()));
		if(nRet < 0) 
			return nRet;
		int32_t nTotal = nRet;
		std::stack<T>& tmp = const_cast<std::stack<T>&>(data);
		while (!tmp.empty())
		{
			nRet = Encode(pBuf,nFreeSize,tmp.top());
			if(nRet < 0) 
				return nRet;
			tmp.pop();
			nTotal+= nRet;
		}
		return nTotal;
	}
	template<typename T>
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const std::priority_queue<T>& data)
	{
		ASSERT_FRAME(data.size()<=UINT16_MAX);
		int32_t nRet = Encode(pBuf,nFreeSize,uint16_t(data.size()));
		if(nRet < 0) 
			return nRet;
		int32_t nTotal = nRet;
		std::priority_queue<T>& tmp=const_cast<std::priority_queue<T>&>(data);
		while (!tmp.empty())
		{
			nRet = Encode(pBuf,nFreeSize,tmp.top());
			if(nRet < 0) 
				return nRet;
			tmp.pop();
			nTotal+= nRet;
		}
		return nTotal;
	}
	template<typename T>
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const T* data)
	{
		return Encode(pBuf,nFreeSize,*data);
	}
	template<typename T,bool>struct CStructCodeHelp{};
	template<typename T>struct CStructCodeHelp<T,true>
	{
		static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const T& data)
		{
			int32_t nRet = data.Encode(pBuf,nFreeSize);
			if (nRet < 0)
				return nRet;
			nFreeSize -= nRet;
			pBuf += nRet;
			return nRet;
		}
		static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,T &data)
		{
			int32_t nRet = data.Decode(pBuf,nSize);
			if (nRet < 0)
				return nRet;
			nSize -= nRet;
			pBuf += nRet;
			return nRet;
		}
		static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const T &data,const char* pName)
		{
			int32_t nRet = CCodeBase::DumpFormat(pBuf,nSize,"%s{",pName);
			if(nRet < 0)
				return nRet;
			int32_t nTotal = nRet;
			nRet = data.dump(pBuf,nSize);
			if(nRet < 0)
				return nRet;
			pBuf += nRet;
			nSize -= nRet;
			nTotal += nRet;
			nRet = CCodeBase::Dump(pBuf,nSize,"}","");
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
			return nTotal;
		}
		static FORCE_INLINE int32_t DumpSql(char *&pBuf,uint32_t &nFreeSize,const T& data)
		{
			int32_t nRet = data.DumpSql(pBuf,nFreeSize);
			if (nRet < 0)
				return nRet;
			nFreeSize -= nRet;
			pBuf += nRet;
			return nRet;
		}
	};
	template<typename T>struct CStructCodeHelp<T,false>
	{
		static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const T& data)
		{
			return 0;
		}
		static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,T &data)
		{
			return 0;
		}
		static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const T &data,const char* pName)
		{
			return 0;
		}
		static FORCE_INLINE int32_t DumpSql(char *&pBuf,uint32_t &nFreeSize,const T& data)
		{
			return 0;
		}
	};
	template<typename T,bool>struct CEnumCodeHelp{};
	template<typename T>struct CEnumCodeHelp<T,true>
	{
		static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const T& data)
		{
			switch (sizeof (T))
			{
			case 1:
				return CCodeBase::Encode(pBuf,nFreeSize,static_cast<int8_t>(data));
			case 2:
				return CCodeBase::Encode(pBuf,nFreeSize,static_cast<int16_t>(data));
			case 4:
				return CCodeBase::Encode(pBuf,nFreeSize,static_cast<int32_t>(data));
			case 8:
				return CCodeBase::Encode(pBuf,nFreeSize,static_cast<int64_t>(data));
			default:
				return -1;
			}
			return -1;
		}
		static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,T &data)
		{
			switch (sizeof (T))
			{
			case 1:
				return CCodeBase::Decode(pBuf,nSize,static_cast<int8_t&>(data));
			case 2:
				return CCodeBase::Decode(pBuf,nSize,static_cast<int16_t&>(data));
			case 4:
				return CCodeBase::Decode(pBuf,nSize,static_cast<int32_t&>(data));
			case 8:
				return CCodeBase::Decode(pBuf,nSize,static_cast<int64_t&>(data));
			default:
				return -1;
			}
			return -1;
		}
		static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const T &data,const char* pName)
		{
			switch (sizeof (T))
			{
			case 1:
				return CCodeBase::Dump(pBuf,nSize,static_cast<uint8_t>(data),pName);
			case 2:
				return CCodeBase::Dump(pBuf,nSize,static_cast<uint16_t>(data),pName);
			case 4:
				return CCodeBase::Dump(pBuf,nSize,static_cast<uint32_t>(data),pName);
			case 8:
				return CCodeBase::Dump(pBuf,nSize,static_cast<uint64_t>(data),pName);
			default:
				return -1;
			}
			return -1;
		}
	};
	template<typename T>struct CEnumCodeHelp<T,false>
	{
		static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const T& data)
		{
			return 0;
		}
		static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,T &data)
		{
			return 0;
		}
		static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const T &data,const char* pName)
		{
			return 0;
		}
	};
	template<typename T>
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const T& data)
	{
		int32_t nRet = CStructCodeHelp<T,std::is_class<T>::value>::Encode(pBuf,nFreeSize,data);
		if(nRet != 0)
			return nRet;
		nRet = CEnumCodeHelp<T,std::is_enum<T>::value>::Encode(pBuf,nFreeSize,data);
		if(nRet != 0)
			return nRet;
		return -1;
	}
	template<typename T>
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const T& data,bool)
	{
		//key not encode
		ASSERT_FRAME(data.size()<=UINT16_MAX);
		int32_t nRet = Encode(pBuf,nFreeSize,uint16_t(data.size()));
		if(nRet < 0) 
			return nRet;
		int32_t nTotal = nRet;
		for (const auto&iter:data)
		{
			nRet = Encode(pBuf,nFreeSize,iter.second);
			if(nRet < 0) 
				return nRet;
			nTotal += nRet;
		}
		return nRet;
	}
	template<typename T>
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const std::queue<T>& data)
	{
		ASSERT_FRAME(data.size()<=UINT16_MAX);
		int32_t nRet = Encode(pBuf,nFreeSize,uint16_t(data.size()));
		if(nRet < 0) 
			return nRet;
		int32_t nTotal = nRet;
		std::queue<T>& tmp=const_cast<std::queue<T>&>(data);
		while (!tmp.empty())
		{
			nRet = Encode(pBuf,nFreeSize,tmp.front());
			if(nRet < 0) 
				return nRet;
			tmp.pop();
			nTotal+= nRet;
		}
		return nTotal;
	}
	template<size_t size>
	static FORCE_INLINE int32_t Encode(int8_t *&pBuf,uint32_t &nFreeSize,const std::bitset<size>& data)
	{
		ASSERT_FRAME(data.size()<=UINT16_MAX);
		int32_t nRet = Encode(pBuf,nFreeSize,uint16_t(data.size()));
		if(nRet < 0) 
			return nRet;
		int32_t nTotal = nRet;
		for (uint32_t i = 0; i < data.size();++i)
		{
			nRet = Encode(pBuf,nFreeSize,data[i]);
			if(nRet < 0) 
				return nRet;
			nTotal+= nRet;
		}
		return nTotal;
	}
	template<typename T>
	static FORCE_INLINE int32_t DecodeMap(int8_t *&pBuf,uint32_t &nSize,T &data)
	{
		uint16_t nNum = 0;
		int32_t nRet = Decode(pBuf,nSize,nNum);
		if(nRet < 0)
			return nRet;
		int32_t nTotal = nRet;
		for (uint32_t i = 0; i < nNum;++i)
		{
			typename T::key_type key;
			nRet = Decode(pBuf,nSize,key);
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
			typename T::mapped_type value;
			nRet = Decode(pBuf,nSize,key);
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
			data.emplace(std::move(key),std::move(value));
		}
		return nTotal;
	}
	template<typename T,typename K>
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,std::map<K,T> &data)
	{
		return DecodeMap(pBuf,nSize,data);
	}
	template<typename T,typename K>
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,std::multimap<K,T> &data)
	{
		return DecodeMap(pBuf,nSize,data);
	}
	template<typename T,typename K>
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,std::unordered_map<K,T> &data)
	{
		return DecodeMap(pBuf,nSize,data);
	}
	template<typename T,typename K>
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,std::unordered_multimap<K,T> &data)
	{
		return DecodeMap(pBuf,nSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t DecodeSet(int8_t *&pBuf,uint32_t &nSize,T &data)
	{
		uint16_t nNum = 0;
		int32_t nRet = Decode(pBuf,nSize,nNum);
		if(nRet < 0)
			return nRet;
		int32_t nTotal = nRet;
		for (uint32_t i = 0; i < nNum;++i)
		{
			typename T::key_type key;
			nRet = Decode(pBuf,nSize,key);
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
			data.emplace(std::move(key));
		}
		return nTotal;
	}
	template<typename T>
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,std::set<T> &data)
	{
		return DecodeSet(pBuf,nSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,std::multiset<T> &data)
	{
		return DecodeSet(pBuf,nSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,std::unordered_set<T> &data)
	{
		return DecodeSet(pBuf,nSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,std::unordered_multiset<T> &data)
	{
		return DecodeSet(pBuf,nSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t DecodeContiner(int8_t *&pBuf,uint32_t &nSize,T &data)
	{
		uint16_t nNum = 0;
		int32_t nRet = Decode(pBuf,nSize,nNum);
		if(nRet < 0)
			return nRet;
		int32_t nTotal = nRet;
		for (uint32_t i = 0; i < nNum;++i)
		{
			typename T::value_type value;
			nRet = Decode(pBuf,nSize, value);
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
			data.push(std::move(value));
		}
		return nTotal;
	}
	template<typename T>
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,std::stack<T> &data)
	{
		return DecodeContiner(pBuf,nSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,std::queue<T> &data)
	{
		return DecodeContiner(pBuf,nSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,std::priority_queue<T> &data)
	{
		return DecodeContiner(pBuf,nSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t DecodeArr(int8_t *&pBuf,uint32_t &nSize,T &data)
	{
		uint16_t nNum = 0;
		int32_t nRet = Decode(pBuf,nSize,nNum);
		if(nRet < 0)
			return nRet;
		int32_t nTotal = nRet;
		for (uint32_t i = 0; i < nNum;++i)
		{
			typename T::value_type value;
			nRet = Decode(pBuf,nSize, value);
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
			data.push_back(std::move(value));
		}
		return nTotal;
	}
	template<typename T>
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,std::vector<T> &data)
	{
		return DecodeArr(pBuf,nSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,std::list<T> &data)
	{
		return DecodeArr(pBuf,nSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,std::deque<T> &data)
	{
		return DecodeArr(pBuf,nSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,T &data)
	{
		int32_t nRet = CStructCodeHelp<T,std::is_class<T>::value>::Decode(pBuf,nSize,data);
		if(nRet != 0)
			return nRet;
		nRet = CEnumCodeHelp<T,std::is_enum<T>::value>::Decode(pBuf,nSize,data);
		if(nRet != 0)
			return nRet;
		return -1;
	}
	template<typename T,bool>struct CCodeMapHelp{};
	template<typename T>struct CCodeMapHelp<T,false>
	{
		static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,T &data){return 0;}
	};
	template<typename T>struct CCodeMapHelp<T,true>
	{
		static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,T &data)
		{
			uint16_t nNum = 0;
			int32_t nRet = Decode(pBuf,nSize,nNum);
			if(nRet < 0)
				return nRet;
			int32_t nTotal = nRet;
			for (uint32_t i = 0; i < nNum;++i)
			{
				typename T::mapped_type value;
				nRet = Decode(pBuf,nSize, value);
				if(nRet < 0)
					return nRet;
				nTotal += nRet;
				data.emplace(i,std::move(value));
			}
			return nTotal;
		}
	};
	template<typename T>
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,T &data,bool)
	{
		return CCodeMapHelp<T,std::is_integral<typename T::key_type>::value>::Decode(pBuf,nSize,data);
	}
	template<typename T>
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,std::forward_list<T> &data)
	{
		//push_front
		uint16_t nNum = 0;
		int32_t nRet = Decode(pBuf,nSize,nNum);
		if(nRet < 0)
			return nRet;
		int32_t nTotal = nRet;
		for (uint32_t i = 0; i < nNum;++i)
		{
			typename T::value_type value;
			nRet = Decode(pBuf,nSize, value);
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
			data.push_front(std::move(value));
		}
		return nTotal;
	}
	template<size_t size>
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,std::bitset<size> &data)
	{
		uint16_t nNum = 0;
		int32_t nRet = Decode(pBuf,nSize,nNum);
		if(nRet < 0)
			return nRet;
		int32_t nTotal = nRet;
		for (uint32_t i = 0; i < nNum;++i)
		{
			nRet = Decode(pBuf,nSize,data[i]);
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
		}
		return nTotal;
	}
	template<typename T,size_t size>
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,std::array<T,size> &data)
	{
		uint16_t nNum = 0;
		int32_t nRet = Decode(pBuf,nSize,nNum);
		if(nRet < 0)
			return nRet;
		int32_t nTotal = nRet;
		for (uint32_t i = 0; i < nNum;++i)
		{
			nRet = Decode(pBuf,nSize,data[i]);
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
		}
		return nTotal;
	}
	template<typename T>
	static FORCE_INLINE int32_t Decode(int8_t *&pBuf,uint32_t &nSize,T *data)
	{
		return -1;//to do
	}
	template<typename T>
	static FORCE_INLINE int32_t DumpMap(char* &pBuf, uint32_t& nSize,const T &data,const char* pName="")
	{
		int32_t nRet = DumpFormat(pBuf,nSize,"%s{size=%u",pName,(uint32_t)data.size());
		if(nRet < 0)
			return nRet;
		int32_t nTotal = nRet;
		bool flag = false;
		for (const auto&iter:data)
		{
			nRet = Dump(pBuf,nSize,"[","");
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
			nRet = Dump(pBuf,nSize,iter.first,"");
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
			nRet = Dump(pBuf,nSize,"]=","");
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
			nRet = Dump(pBuf,nSize,iter.second,"");
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
		}
		nRet = Dump(pBuf,nSize,"}","");
		if(nRet < 0)
			return nRet;
		nTotal += nRet;
		return nTotal;
	}
	template<typename T,typename K>
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const std::map<K,T> &data,const char* pName="")
	{ 
		return DumpMap(pBuf,nSize,data,pName);
	}
	template<typename T,typename K>
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const std::multimap<K,T> &data,const char* pName="")
	{ 
		return DumpMap(pBuf,nSize,data,pName);
	}
	template<typename T,typename K>
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const std::unordered_map<K,T> &data,const char* pName="")
	{ 
		return DumpMap(pBuf,nSize,data,pName);
	}
	template<typename T,typename K>
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const std::unordered_multimap<K,T> &data,const char* pName="")
	{ 
		return DumpMap(pBuf,nSize,data,pName);
	}
	template<typename T>
	static FORCE_INLINE int32_t DumpArr(char* &pBuf, uint32_t& nSize,const T &data,const char* pName="")
	{
		int32_t nRet = DumpFormat(pBuf,nSize,"%s{size=%u",pName,(uint32_t)data.size());
		if(nRet < 0)
			return nRet;
		int32_t nTotal = nRet;
		uint32_t index = 0;
		for (const auto&iter:data)
		{
			nRet = DumpFormat(pBuf,nSize,"[%u]=",index);
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
			++index;
			nRet = Dump(pBuf,nSize,iter,"");
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
		}
		nRet = Dump(pBuf,nSize,"}","");
		if(nRet < 0)
			return nRet;
		nTotal += nRet;
		return nTotal;
	}
	template<typename T>
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const std::set<T> &data,const char* pName="")
	{ 
		return DumpArr(pBuf,nSize,data,pName);
	}
	template<typename T>
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const std::multiset<T> &data,const char* pName="")
	{ 
		return DumpArr(pBuf,nSize,data,pName);
	}
	template<typename T>
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const std::unordered_set<T> &data,const char* pName="")
	{ 
		return DumpArr(pBuf,nSize,data,pName);
	}
	template<typename T>
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const std::unordered_multiset<T> &data,const char* pName="")
	{ 
		return DumpArr(pBuf,nSize,data,pName);
	}
	template<typename T>
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const std::vector<T> &data,const char* pName="")
	{ 
		return DumpArr(pBuf,nSize,data,pName);
	}
	template<typename T>
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const std::list<T> &data,const char* pName="")
	{ 
		return DumpArr(pBuf,nSize,data,pName);
	}
	template<typename T>
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const std::deque<T> &data,const char* pName="")
	{ 
		return DumpArr(pBuf,nSize,data,pName);
	}
	template<typename T>
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const std::forward_list<T> &data,const char* pName="")
	{ 
		return DumpArr(pBuf,nSize,data,pName);
	}
	template<typename T,size_t size>
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const std::array<T,size> &data,const char* pName="")
	{ 
		return DumpArr(pBuf,nSize,data,pName);
	}
	template<typename T>
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const std::stack<T> &data,const char* pName="")
	{ 
		int32_t nRet = DumpFormat(pBuf,nSize,"%s{size=%u",pName,(uint32_t)data.size());
		if(nRet < 0)
			return nRet;
		int32_t nTotal = nRet;
		std::stack<T>&  tmp = const_cast<std::stack<T>&>(data);
		std::stack<T> back;
		uint32_t index = 0;
		while (!tmp.empty())
		{
			nRet = DumpFormat(pBuf,nSize,"[%u]=",index);
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
			++index;
			nRet = Dump(pBuf,nSize,tmp.top(),"");
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
			back.push(tmp.top());
			tmp.pop();
		}
		nRet = Dump(pBuf,nSize,"}","");
		if(nRet < 0)
			return nRet;
		nTotal += nRet;
		tmp.swap(back);
		return nTotal;
	}
	template<typename T>
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const std::queue<T> &data,const char* pName="")
	{ 
		int32_t nRet = DumpFormat(pBuf,nSize,"%s{size=%u",pName,(uint32_t)data.size());
		if(nRet < 0)
			return nRet;
		int32_t nTotal = nRet;
		std::queue<T>&  tmp = const_cast<std::queue<T>&>(data);
		std::queue<T> back;
		uint32_t index=0;
		while (!tmp.empty())
		{
			nRet = DumpFormat(pBuf,nSize,"[%u]=",index);
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
			++index;
			nRet = Dump(pBuf,nSize,tmp.front(),"");
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
			back.push(tmp.front());
			tmp.pop();
		}
		nRet = Dump(pBuf,nSize,"}","");
		if(nRet < 0)
			return nRet;
		nTotal += nRet;
		tmp.swap(back);
		return nTotal;
	}
	template<typename T>
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const T &data,const char* pName="")
	{ 
		int32_t nRet = CStructCodeHelp<T,std::is_class<T>::value>::Dump(pBuf,nSize,data,pName);
		if(nRet != 0)
			return nRet;
		nRet = CEnumCodeHelp<T,std::is_enum<T>::value>::Dump(pBuf,nSize,data,pName);
		if(nRet != 0)
			return nRet;
		return -1;
	}
	template<typename T>
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const std::priority_queue<T> &data,const char* pName="")
	{
		int32_t nRet = DumpFormat(pBuf,nSize,"%s{size=%u",pName,(uint32_t)data.size());
		if(nRet < 0)
			return nRet;
		int32_t nTotal = nRet;
		std::priority_queue<T>&  tmp = const_cast<std::priority_queue<T>&>(data);
		std::priority_queue<T> back;
		uint32_t index = 0;
		while (!tmp.empty())
		{
			nRet = DumpFormat(pBuf,nSize,"[%u]=",index);
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
			++index;
			nRet = Dump(pBuf,nSize,tmp.top(),"");
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
			back.push(tmp.top());
			tmp.pop();
		}
		nRet = Dump(pBuf,nSize,"}","");
		if(nRet < 0)
			return nRet;
		nTotal += nRet;
		tmp.swap(back);
		return nTotal;
	}
	template<size_t size>
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const std::bitset<size> &data,const char* pName="")
	{
		int32_t nRet = DumpFormat(pBuf,nSize,"%s{size=%u",pName,(uint32_t)data.size());
		if(nRet < 0)
			return nRet;
		int32_t nTotal = nRet;
		for (uint32_t index = 0;index < data.size();++index)
		{
			nRet = DumpFormat(pBuf,nSize,"[%u]=",index);
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
			nRet = Dump(pBuf,nSize,data[index]);
			if(nRet < 0)
				return nRet;
			nTotal += nRet;
		}
		nRet = Dump(pBuf,nSize,"}","");
		if(nRet < 0)
			return nRet;
		nTotal += nRet;
		return nTotal;
	}
	template<typename T>
	static FORCE_INLINE int32_t Dump(char* &pBuf, uint32_t& nSize,const T *data,const char* pName="")
	{ 
		return Dump(pBuf,nSize,*data,pName);
	}
	//dump for sql return char *
	template<typename T>
	static FORCE_INLINE char * DumpSqlMap(char *&pBuf,uint32_t &nFreeSize,const T& data)
	{
		char * pRet = pBuf;
		bool flag = false;
		int32_t nRet;
		for (const auto&iter:data)
		{
			if (flag)
			{
				nRet = Dump(pBuf,nFreeSize,"|","");
				if(nRet < 0) 
					return pRet;
			}
			nRet = DumpSql(pBuf,nFreeSize,iter.first);
			if(nRet < 0) 
				return pRet;
			nRet = Dump(pBuf,nFreeSize,"-","");
			if(nRet < 0) 
				return pRet;
			nRet = DumpSql(pBuf,nFreeSize,iter.second);
			if(nRet < 0) 
				return pRet;
			flag = true;
		}
		return pRet;
	}
	template<typename T,typename K>
	static FORCE_INLINE char * DumpSql(char *&pBuf,uint32_t &nFreeSize,const std::map<K,T>& data)
	{
		return DumpSqlMap(pBuf,nFreeSize,data);
	}
	template<typename T,typename K>
	static FORCE_INLINE char * DumpSql(char *&pBuf,uint32_t &nFreeSize,const std::multimap<K,T>& data)
	{
		return DumpSqlMap(pBuf,nFreeSize,data);
	}
	template<typename T,typename K>
	static FORCE_INLINE char * DumpSql(char *&pBuf,uint32_t &nFreeSize,const std::unordered_map<K,T>& data)
	{
		return DumpSqlMap(pBuf,nFreeSize,data);
	}
	template<typename T,typename K>
	static FORCE_INLINE char * DumpSql(char *&pBuf,uint32_t &nFreeSize,const std::unordered_multimap<K,T>& data)
	{
		return DumpSqlMap(pBuf,nFreeSize,data);
	}
	template<typename T>
	static FORCE_INLINE char * DumpSqlArr(char *&pBuf,uint32_t &nFreeSize,const T& data)
	{
		char * pRet = pBuf;
		bool flag = false;
		int32_t nRet;
		for (const auto&iter:data)
		{
			if (flag)
			{
				nRet = Dump(pBuf,nFreeSize,"|","");
				if(nRet < 0) 
					return pRet;
			}
			nRet = DumpSql(pBuf,nFreeSize,iter);
			if(nRet < 0) 
				return pRet;
			flag = true;
		}
		return pRet;
	}
	template<typename T>
	static FORCE_INLINE char * DumpSql(char *&pBuf,uint32_t &nFreeSize,const std::set<T>& data)
	{
		return DumpSqlArr(pBuf,nFreeSize,data);
	}
	template<typename T>
	static FORCE_INLINE char * DumpSql(char *&pBuf,uint32_t &nFreeSize,const std::multiset<T>& data)
	{
		return DumpSqlArr(pBuf,nFreeSize,data);
	}
	template<typename T>
	static FORCE_INLINE char * DumpSql(char *&pBuf,uint32_t &nFreeSize,const std::unordered_set<T>& data)
	{
		return DumpSqlArr(pBuf,nFreeSize,data);
	}
	template<typename T>
	static FORCE_INLINE char * DumpSql(char *&pBuf,uint32_t &nFreeSize,const std::unordered_multiset<T>& data)
	{
		return DumpSqlArr(pBuf,nFreeSize,data);
	}
	template<typename T>
	static FORCE_INLINE char * DumpSql(char *&pBuf,uint32_t &nFreeSize,const std::vector<T>& data)
	{
		return DumpSqlArr(pBuf,nFreeSize,data);
	}
	template<typename T>
	static FORCE_INLINE char * DumpSql(char *&pBuf,uint32_t &nFreeSize,const std::list<T>& data)
	{
		return DumpSqlArr(pBuf,nFreeSize,data);
	}
	template<typename T>
	static FORCE_INLINE char * DumpSql(char *&pBuf,uint32_t &nFreeSize,const std::deque<T>& data)
	{
		return DumpSqlArr(pBuf,nFreeSize,data);
	}
	template<typename T>
	static FORCE_INLINE char * DumpSql(char *&pBuf,uint32_t &nFreeSize,const std::forward_list<T>& data)
	{
		return DumpSqlArr(pBuf,nFreeSize,data);
	}
	template<typename T,size_t size>
	static FORCE_INLINE char * DumpSql(char *&pBuf,uint32_t &nFreeSize,const std::array<T,size>& data)
	{
		return DumpSqlArr(pBuf,nFreeSize,data);
	}
	template<typename T>
	static FORCE_INLINE char * DumpSql(char *&pBuf,uint32_t &nFreeSize,const std::stack<T>& data)
	{
		char * pRet = pBuf;
		bool flag = false;
		int32_t nRet;
		std::stack<T>& tmp = const_cast<std::stack<T>&>(data);
		while (!tmp.empty())
		{
			if (flag)
			{
				nRet = Dump(pBuf,nFreeSize,"|","");
				if(nRet < 0) 
					return pRet;
			}
			nRet = DumpSql(pBuf,nFreeSize,tmp.top());
			if(nRet < 0) 
				return pRet;
			tmp.pop();
			flag = true;
		}
		return pRet;
	}
	template<typename T>
	static FORCE_INLINE char * DumpSql(char *&pBuf,uint32_t &nFreeSize,const std::priority_queue<T>& data)
	{
		char * pRet = pBuf;
		bool flag = false;
		int32_t nRet;
		std::priority_queue<T>&tmp=const_cast<std::priority_queue<T>&>(data);
		while (!tmp.empty())
		{
			if (flag)
			{
				nRet = Dump(pBuf,nFreeSize,"|","");
				if(nRet < 0) 
					return pRet;
			}
			nRet = DumpSql(pBuf,nFreeSize,tmp.top());
			if(nRet < 0) 
				return pRet;
			tmp.pop();
			flag = true;
		}
		return pRet;
	}
	template<typename T>
	static FORCE_INLINE char * DumpSql(char *&pBuf,uint32_t &nFreeSize,const T& data)
	{
		char * pRet = pBuf;
		if(0!= CStructCodeHelp<T,std::is_class<T>::value>::DumpSql(pBuf,nFreeSize,data))
			return pRet;
		if(0 != CEnumCodeHelp<T,std::is_enum<T>::value>::Dump(pBuf,nFreeSize,data))
			return pRet;
		Dump(pBuf,nFreeSize,data);
		return pRet;
	}
	template<typename T>
	static FORCE_INLINE char * DumpSql(char *&pBuf,uint32_t &nFreeSize,const T& data,bool)
	{
		// key not encode
		bool flag = false;
		int32_t nRet;
		char * pRet = pBuf;
		for (const auto&iter:data)
		{
			if (flag)
			{
				nRet = Dump(pBuf,nFreeSize,"|","");
				if(nRet < 0) 
					return pRet;
			}
			nRet = DumpSql(pBuf,nFreeSize,iter.second);
			if(nRet < 0) 
				return pRet;
			flag = true;
		}
		return pRet;
	}
	template<typename T>
	static FORCE_INLINE char * DumpSql(char *&pBuf,uint32_t &nFreeSize,const std::queue<T>& data)
	{
		//front/pop
		bool flag = false;
		int32_t nRet;
		char * pRet = pBuf;
		std::queue<T>& tmp = const_cast<std::queue<T>&>(data);
		while (!tmp.empty())
		{
			if (flag)
			{
				nRet = Dump(pBuf,nFreeSize,"|","");
				if(nRet < 0) 
					return pRet;
			}
			nRet = DumpSql(pBuf,nFreeSize,tmp.front());
			if(nRet < 0) 
				return pRet;
			tmp.pop();
			flag = true;
		}
		return pRet;
	}
	template<size_t size>
	static FORCE_INLINE char * DumpSql(char *&pBuf,uint32_t &nFreeSize,const std::bitset<size>& data)
	{
		bool flag = false;
		int32_t nRet;
		char * pRet = pBuf;
		for (uint32_t i = 0; i < data.size();++i)
		{
			if (flag)
			{
				nRet = Dump(pBuf,nFreeSize,"|","");
				if(nRet < 0) 
					return pRet;
			}
			nRet = Dump(pBuf,nFreeSize,data[i],"");
			if(nRet < 0) 
				return pRet;
			flag = true;
		}
		return pRet;
	}
	template<typename T>
	static FORCE_INLINE char * DumpSql(char *&pBuf,uint32_t &nFreeSize,const T* data)
	{
		return DumpSql(pBuf,nFreeSize,*data);
	}
};

class CCode
{
private:
#define MAXCODEBUFFSIZE 64*1024*1024//最大编码缓冲区
#define MAXCONTAINERSSIZE 0xFFFF//最大数组大小
	static char m_szBuff[MAXCODEBUFFSIZE];
	static const char* m_pBuff;
	static uint32_t m_nSize;
	static uint32_t m_nReadSize;
	static bool m_error;
private:
	static void Clear() { m_nSize = 0; m_error = false; }
	static void SetData(const char* pBuff, uint32_t size) { m_pBuff = pBuff; m_nSize = size; m_nReadSize = 0; }
	static bool Encode(const void* val, uint32_t inSize)
	{
		if (m_error || inSize + m_nSize > MAXCODEBUFFSIZE)
		{
			m_error =true;
			return false;
		}
		memcpy(&m_szBuff[m_nSize], val, inSize);
		m_nSize += inSize;
		return true;
	}
	static bool Decode(void* val, uint32_t outSize)
	{
		if (m_error || outSize + m_nReadSize > m_nSize)
		{
			m_error = true;
			return false;
		}
		memcpy(val, &m_pBuff[m_nReadSize], outSize);
		m_nReadSize += outSize;
		return true;
	}
	static bool CheckSize(size_t size)
	{
		if (size <= MAXCONTAINERSSIZE)
			return true;
		m_error = true;
		return false;
	}
	template <typename T>
	class IsMap
	{
		template <class C> static uint8_t check(typename C::mapped_type*, typename C::key_type*) { return 0; }
		template <class C> static uint16_t check(...) { return 0; }
	public:
		enum { value = (sizeof( uint8_t) == sizeof (check<T>(nullptr, nullptr))) };
	};
	template <typename T>
	class IsSet
	{
		template <class C> static uint8_t check(typename C::key_type*) { return 0; }
		template <class C> static uint16_t check(...) { return 0; }
	public:
		enum { value = (sizeof (uint8_t) == sizeof (check<T>(nullptr))) };
	};
	template <typename T>
	class IsVector
	{
		template <class C> static uint8_t check(typename C::value_type*, typename C::iterator*) { return 0; }
		template <class C> static uint16_t check(...) { return 0; }
	public:
		enum { value = (sizeof (uint8_t) == sizeof( check<T>(nullptr, nullptr))) };
	};
public:
	static bool HasError() { return m_error; }
	static uint32_t GetEncodeSize() { return m_nSize; }
	static const char* GetEncodeBuff() { return m_szBuff; }
	static bool Encode(const CMsgBase&& msg)
	{
		Clear();
		return msg.Encode();
	}
	static bool Encode(const CMsgBase& msg)
	{
		Clear();
		return msg.Encode();
	}
	static bool Encode(const CMsgBase* msg)
	{
		Clear();
		return msg->Encode();
	}
	static bool Decode(const char *pBuff, uint32_t nSize, CMsgBase* msg)
	{
		SetData(pBuff, nSize);
		return msg->Decode();
	}
	static bool Decode(const char *pBuff, uint32_t nSize, CMsgBase& msg)
	{
		SetData(pBuff, nSize);
		return msg.Decode();
	}
	//编码
	static bool Encode(const bool val) { return Encode(&val, 1); }
	static bool Encode(const uint8_t val) { return Encode(&val, 1); }
	static bool Encode(const int8_t val) { return Encode(&val, 1); }
	static bool Encode(const uint16_t val) { return Encode(&val, 2); }
	static bool Encode(const int16_t val) { return Encode(&val, 2); }
	static bool Encode(const uint32_t val) { return Encode(&val, 4); }
	static bool Encode(const int32_t val) { return Encode(&val, 4); }
	static bool Encode(const uint64_t val) { return Encode(&val, 8); }
	static bool Encode(const int64_t val) { return Encode(&val, 8); }
	static bool Encode(const float val) { return Encode(&val, 4); }
	static bool Encode(const double val) { return Encode(&val, 8); }
	static bool Encode(const std::string& val)
	{
		return CheckSize(val.size()) && Encode((uint16_t)val.size()) && Encode(val.data(), (uint32_t)val.size());
	}
	template<class T>//enum
	static std::enable_if_t<std::is_enum<T>::value, bool> Encode(const T val) { return Encode(&val, (uint32_t)(sizeof val)); }
	template<class T>//struct or class
	static std::enable_if_t<!IsVector<T>::value&& std::is_class<T>::value, bool> Encode(const T& val) { val.Encode(); return HasError(); }
	template<class T>//map multimap unordered_map unordered_multimap
	static std::enable_if_t<IsMap<T>::value, bool> Encode(const T& val)
	{
		if (!CheckSize(val.size()) || !Encode(uint16_t(val.size())))
			return false;
		for (auto& iter : val)
			if (!Encode(iter.first) || !Encode(iter.second))
				return false;
		return true;
	}
	template<class T>//set multiset unordered_set unordered_multiset vector list  forward_list deque array
	static  std::enable_if_t<IsVector<T>::value && !IsMap<T>::value, bool> Encode(const T& val)
	{
		if (!CheckSize(val.size()) || !Encode(uint16_t(val.size())))
			return false;
		for (auto& iter : val)
			if (!Encode(iter))
				return false;
		return true;
	}
	template <size_t nSize>
	static bool Encode(const std::bitset<nSize>& val)
	{
		if (!CheckSize(val.size()) || !Encode(uint16_t(nSize)))
			return false;
		if (nSize > 8 && nSize <= 64)
			return Encode((uint64_t)val.to_ullong());
		for (uint32_t i = 0; i < nSize; ++i)
			if (!Encode(val[i]))
				return false;
		return true;
	}
	template<class T, size_t nSize>
	static bool Encode(const T(&val)[nSize])//普通数组
	{
		if (!CheckSize(nSize) || !Encode(uint16_t(nSize)))
			return false;
		for (auto& iter : val)
			if (!Encode(iter))
				return false;
		return true;
	}
	//解码
	static bool Decode(bool& val) { return Decode(&val, 1); }
	static bool Decode(uint8_t& val) { return Decode(&val, 1); }
	static bool Decode(int8_t& val) { return Decode(&val, 1); }
	static bool Decode(uint16_t& val) { return Decode(&val, 2); }
	static bool Decode(int16_t& val) { return Decode(&val, 2); }
	static bool Decode(uint32_t& val) { return Decode(&val, 4); }
	static bool Decode(int32_t& val) { return Decode(&val, 4); }
	static bool Decode(uint64_t& val) { return Decode(&val, 8); }
	static bool Decode(int64_t& val) { return Decode(&val, 8); }
	static bool Decode(float& val) { return Decode(&val, 4); }
	static bool Decode(double& val) { return Decode(&val, 8); }
	static bool Decode(std::string& val)
	{
		uint16_t len = 0;
		if (!Decode(&len, 2))
			return false;
		if (len + m_nReadSize > m_nSize)
		{
			m_error = true;
			return false;
		}
		val.assign(&m_pBuff[m_nReadSize], len);
		m_nReadSize += len;
		return true;
	}
	template<class T>//enum
	static std::enable_if_t<std::is_enum<T>::value, bool> Decode(T& val) { return Decode(&val, (uint32_t)(sizeof val)); }
	template<class T>//struct or class
	static std::enable_if_t<!IsVector<T>::value && std::is_class<T>::value, bool> Decode(T& val) { return val.Decode(); }
	template<class T>//map multimap unordered_map unordered_multimap
	static std::enable_if_t<IsMap<T>::value, bool> Decode(T& val)
	{
		uint16_t len = 0;
		if (!Decode(&len, 2))
			return false;
		typename T::key_type key;
		typename T::mapped_type tmp;
		for (uint32_t i = 0; i < len; ++i)
		{
			if (!Decode(key) || !Decode(tmp))
				return false;
			val.emplace(std::move(key), std::move(tmp));
		}
		return true;
	}
	template<class T>//set multiset unordered_set unordered_multiset
	static  std::enable_if_t<IsVector<T>::value && !IsMap<T>::value && IsSet<T>::value, bool> Decode(T& val)
	{
		uint16_t len = 0;
		if (!Decode(&len, 2))
			return false;
		T tmp;
		for (uint32_t i = 0; i < len; ++i)
		{
			if (!Decode(tmp))
				return false;
			val.emplace(std::move(tmp));
		}
		return true;
	}
	template<class T>//vector list deque
	static  std::enable_if_t<IsVector<T>::value && !IsSet<T>::value, bool> Decode(T& val)
	{
		uint16_t len = 0;
		if (!Decode(&len, 2))
			return false;
		T tmp;
		for (uint32_t i = 0; i < len; ++i)
		{
			if (!Decode(tmp))
				return false;
			val.emplace_back(std::move(tmp));
		}
		return true;
	}
	template<class T>//forward_list
	static  bool Decode(std::forward_list<T>& val)
	{
		uint16_t len = 0;
		if (!Decode(&len, 2))
			return false;
		T tmp;
		for (uint32_t i = 0; i < len; ++i)
		{
			if (!Decode(tmp))
				return false;
			val.emplace_front(std::move(tmp));
		}
		return true;
	}
	template<class T, size_t nSize>//array
	static  bool Decode(std::array<T, nSize>& val)
	{
		uint16_t len = 0;
		if (!Decode(&len, 2))
			return false;
		auto maxSize = len > nSize ? nSize : len;
		for (uint32_t i = 0; i < nSize; ++i)
			if (!Decode(val[i]))
				return false;
		T tmp;
		for (uint32_t i = len; i < maxSize; ++i)
			val[i] = tmp;
		for (uint32_t i = maxSize; i < len; ++i)
			if (!Decode(tmp))
				return false;
		return true;
	}
	template <size_t nSize>
	static bool Decode(std::bitset<nSize>& val)
	{
		uint16_t len = 0;
		if (!Decode(&len, 2))
			return false;
		if (len > 8 && len <= 64)
		{
			uint64_t tmp = 0;
			if (!Decode(&tmp, 8))
				return false;
			val = std::bitset<nSize>(tmp);
			return true;
		}
		bool tmp;
		auto maxSize = len > nSize ? nSize : len;
		for (uint32_t i = 0; i < maxSize; ++i)
		{
			if (!Decode(tmp))
				return false;
			val[i] = tmp;
		}
		for (uint32_t i = len; i < maxSize; ++i)
			val[i] = false;
		for (uint32_t i = maxSize; i < len; ++i)
			if (!Decode(tmp))
				return false;
		return true;
	}
	template<class T, size_t nSize>
	static bool Decode(T(&val)[nSize])//普通数组
	{
		uint16_t len = 0;
		if (!Decode(&len, 2))
			return false;
		auto maxSize = len > nSize ? nSize : len;
		for (uint32_t i = 0; i < maxSize; ++i)
			if (!Decode(val[i]))
				return false;
		T tmp;
		for (uint32_t i = len; i < maxSize; ++i)
			val[i] = tmp;
		for (uint32_t i = maxSize; i < len; ++i)
			if (!Decode(tmp))
				return false;
		return true;
	}
	//不支持 stack queue priority_queue
};
#endif