#ifndef _H_NEWQUEUE_H
#define _H_NEWQUEUE_H
#include <assert.h>
#ifndef WIN32
#include <string.h>
#endif
#include "define.h"
#include "errorcode.h"
//无锁队列 A线程只读 B线程只写
template <class T,uint32_t MaxQueueSize=1024*1024>
class CNewQueue
{
public:
	CNewQueue(uint32_t nSize=0)
	{
		m_nHead = 0;
		m_nTail =0;
		m_nMaxSize = nSize > 0 ? nSize : MaxQueueSize;
		m_pBuf = new T[m_nMaxSize];
		ASSERT_FRAME(m_pBuf != NULL);
	}
	~CNewQueue()
	{
		m_nHead = 0;
		m_nTail = 0;
		if (m_pBuf)delete [] m_pBuf;
		m_pBuf = NULL;
	}
public:
	void Init()
	{
		m_nHead = 0;
		m_nTail = 0;
	}
	uint32_t GetHead()const
	{
		return m_nHead;
	}
	void SetHead(uint32_t nHead)
	{
		m_nHead = nHead;
	}
	uint32_t GetTail()const
	{
		return m_nTail;
	}
	void SetTail(uint32_t nTail)
	{
		m_nTail = nTail;
	}
	void GetInfo(uint32_t &nHead,uint32_t &nTail)const
	{
		nHead = m_nHead;
		nTail = m_nTail;
	}
	void SetInfo(uint32_t nHead,uint32_t nTail)
	{
		m_nHead = nHead;
		m_nTail = nTail;
	}
	bool IsEmpty()const
	{
		return (m_nTail == m_nHead);
	}
	bool IsFull()const
	{
		return((m_nTail+1)%m_nMaxSize == m_nHead);
	}
	uint32_t GetSize()const
	{
		return(m_nTail+m_nMaxSize-m_nHead)%m_nMaxSize;
	}
	uint32_t GetMaxSize()const
	{
		return m_nMaxSize;
	}
	uint32_t GetFreeSize()const
	{
		return  m_nMaxSize -1-GetSize();
	}
	//删除
	void Remove(uint32_t nSize)
	{
		m_nHead = (m_nHead+nSize)%m_nMaxSize;
	}
	void Add(uint32_t nSize)
	{
		m_nTail = (m_nTail+nSize)%m_nMaxSize;
	}
	//获取消息并且删除
	int32_t GetBuf(T *pBuf,uint32_t &nSize,uint32_t nMaxBufSize)
	{
		if(unlikely(NULL == pBuf)) return E_NULLPOINTER;
		if(GetSize() < sizeof(uint32_t)/sizeof(T)) return S_OK;
		uint32_t nHead = m_nHead;
		uint32_t nTail = m_nTail;
		uint32_t nLength = 0;
		T *p = reinterpret_cast<T*>(&nLength);
		for (uint32_t i = 0; i < sizeof(uint32_t)/sizeof(T); ++i)
		{
			p[i] = m_pBuf[nHead];
			nHead = (nHead+1)%m_nMaxSize;
		}
		uint32_t nRemoveSize = nLength + sizeof(uint32_t);
		if (GetSize() < nRemoveSize) return S_OK;
		if(unlikely(nRemoveSize >= m_nMaxSize))//数据有问题可能是加入数据的时候不完整
		{
			m_nHead = 0;
			m_nTail = 0;
			return E_UNKNOW;
		}
		if(unlikely(nLength > nMaxBufSize)) //传入的pBuf内存不够大。
		{
			//m_nHead = (m_nHead+nRemoveSize)%m_nMaxSize;
			return E_LESSMEMORY;
		}
		nSize = nLength;
		if (nHead < nTail)
		{
			memcpy(pBuf,&m_pBuf[nHead],nLength);
			m_nHead = (m_nHead+nRemoveSize)%m_nMaxSize;
			return S_OK;
		}
		uint32_t n = m_nMaxSize-nHead;
		if (n >= nLength)
		{
			memcpy(pBuf,&m_pBuf[nHead],nLength);
			m_nHead = (m_nHead+nRemoveSize)%m_nMaxSize;
			return S_OK;
		}
		memcpy(pBuf,&m_pBuf[nHead],n);
		memcpy(&pBuf[n],m_pBuf,nLength-n);
		m_nHead = (m_nHead+nRemoveSize)%m_nMaxSize;
		return S_OK;
	}
	//获取消息并且删除 为日志特化
	bool GetBuf(char *&pBuf,uint32_t &nSize,uint32_t nMaxBufSize, ServerID*id)
	{
		uint32_t nHead = m_nHead;
		uint32_t nTail = m_nTail;
		char *p = reinterpret_cast<char*>(&nSize);
		for (uint32_t i = 0; i < sizeof(uint32_t); ++i)
		{
			p[i] = m_pBuf[nHead];
			nHead = (nHead+1)%m_nMaxSize;
		}
		if (nullptr != id)
		{
			p = reinterpret_cast<char*>(id);
			for (uint32_t i = 0; i < sizeof(ServerID); ++i)
			{
				p[i] = m_pBuf[nHead];
				nHead = (nHead+1)%m_nMaxSize;
			}
		}
		if (nHead < nTail)
		{
			pBuf = &m_pBuf[nHead];
			return true;
		}
		uint32_t n = m_nMaxSize-nHead;
		if (n >= nSize)
		{
			pBuf = &m_pBuf[nHead];
			return true;
		}
		memcpy(pBuf,&m_pBuf[nHead],n);
		memcpy(&pBuf[n],m_pBuf,nSize-n);
		m_nHead = (m_nHead + sizeof(uint32_t) + nSize + (id != nullptr ? sizeof(ServerID) : 0)) % m_nMaxSize;
		return false;
	}
	//获取sql消息并且删除 这个是特化所以没有安全检查
	bool GetSqlBuf(char *&pBuf,uint32_t nSize)
	{
		uint32_t nHead = m_nHead;
		uint32_t nTail = m_nTail;
		if (nHead < nTail)
		{
			pBuf= &m_pBuf[nHead];
			return true;
		}
		uint32_t n = m_nMaxSize-nHead;
		if (n >= nSize)
		{
			pBuf= &m_pBuf[nHead];
			return true;
		}
		memcpy(pBuf,&m_pBuf[nHead],n);
		memcpy(&pBuf[n],m_pBuf,nSize-n);
		m_nHead = (m_nHead+nSize)%m_nMaxSize;
		return false;
	}
	//size会放在buf前面
	int32_t AddBuf(const T *pBuf,uint32_t nSize)
	{
		if(unlikely(0 == nSize)) return S_OK;
		if(unlikely(NULL == pBuf)) return E_NULLPOINTER;
		uint32_t nFree = GetFreeSize();
		uint32_t nAddSize = nSize + sizeof(uint32_t)/sizeof(T);
		if (nFree < nAddSize) return E_LESSMEMORY;
		uint32_t nTail = m_nTail;
		uint32_t nHead = m_nHead;
		const T *p = reinterpret_cast<const T*>(&nSize);
		for (uint32_t i = 0 ; i < sizeof(uint32_t)/sizeof(T); ++i)
		{
			m_pBuf[nTail] = p[i];
			nTail = (nTail+1)%m_nMaxSize;
		}
		if (nTail < nHead)
		{
			memcpy(&m_pBuf[nTail],pBuf,nSize);
			m_nTail = (m_nTail+nAddSize)%m_nMaxSize;
			return S_OK;
		}
		uint32_t n = m_nMaxSize-nTail;
		if (n >= nSize)
		{
			memcpy(&m_pBuf[nTail],pBuf,nSize);
			m_nTail = (m_nTail+nAddSize)%m_nMaxSize;
			return S_OK;
		}
		memcpy(&m_pBuf[nTail],pBuf,n);
		memcpy(m_pBuf,&pBuf[n],nSize-n);
		m_nTail = (m_nTail+nAddSize)%m_nMaxSize;
		return S_OK;
	}
	//size会放在buf前面 为跨服日志做的特化
	int32_t AddBuf(const T *pBuf,uint32_t nSize,ServerID id)
	{
		if(unlikely(0 == nSize)) return S_OK;
		if(unlikely(NULL == pBuf)) return E_NULLPOINTER;
		uint32_t nFree = GetFreeSize();
		uint32_t nAddSize = nSize + (sizeof(uint32_t)+sizeof(ServerID))/sizeof(T);
		if (nFree < nAddSize) return E_LESSMEMORY;
		uint32_t nTail = m_nTail;
		uint32_t nHead = m_nHead;
		const T *p = reinterpret_cast<const T*>(&nSize);
		for (uint32_t i = 0 ; i < sizeof(uint32_t)/sizeof(T); ++i)
		{
			m_pBuf[nTail] = p[i];
			nTail = (nTail+1)%m_nMaxSize;
		}
		p = reinterpret_cast<const T*>(&id);
		for (uint32_t i = 0 ; i < sizeof(ServerID)/sizeof(T); ++i)
		{
			m_pBuf[nTail] = p[i];
			nTail = (nTail+1)%m_nMaxSize;
		}
		if (nTail < nHead)
		{
			memcpy(&m_pBuf[nTail],pBuf,nSize);
			m_nTail = (m_nTail+nAddSize)%m_nMaxSize;
			return S_OK;
		}
		uint32_t n = m_nMaxSize-nTail;
		if (n >= nSize)
		{
			memcpy(&m_pBuf[nTail],pBuf,nSize);
			m_nTail = (m_nTail+nAddSize)%m_nMaxSize;
			return S_OK;
		}
		memcpy(&m_pBuf[nTail],pBuf,n);
		memcpy(m_pBuf,&pBuf[n],nSize-n);
		m_nTail = (m_nTail+nAddSize)%m_nMaxSize;
		return S_OK;
	}
	//获取消息指针不删除
	int32_t GetBuf(T **pBuf,uint32_t &nHead,uint32_t &nTail)
	{
		nHead = m_nHead;
		nTail = m_nTail;
		*pBuf = &m_pBuf[nHead];
		return S_OK;
	}
	//只放入消息
	int32_t AddOnlyBuf(const T *pBuf,uint32_t nSize)
	{
		if(unlikely(0 == nSize)) return S_OK;
		if(unlikely(NULL == pBuf)) return E_NULLPOINTER;
		uint32_t nFree = GetFreeSize();
		if (nFree < nSize) return E_LESSMEMORY;
		uint32_t nTail = m_nTail;
		uint32_t nHead = m_nHead;
		if (nTail < nHead)
		{
			memcpy(&m_pBuf[nTail],pBuf,nSize);
			m_nTail = (m_nTail+nSize)%m_nMaxSize;
			return S_OK;
		}
		uint32_t n = m_nMaxSize-nTail;
		if (n >= nSize)
		{
			memcpy(&m_pBuf[nTail],pBuf,nSize);
			m_nTail = (m_nTail+nSize)%m_nMaxSize;
			return S_OK;
		}
		memcpy(&m_pBuf[nTail],pBuf,n);
		memcpy(m_pBuf,&pBuf[n],nSize-n);
		m_nTail = (m_nTail+nSize)%m_nMaxSize;
		return S_OK;
	}
	int32_t AddMsg(const NetBuffer arrBuf[2], SessionID id)
	{
		uint32_t nSize = arrBuf[0].len + arrBuf[1].len + (sizeof id);
		if(GetFreeSize() < nSize)return E_LESSMEMORY;
		uint32_t nTail = m_nTail;
		uint32_t n = m_nMaxSize - nTail;
		if (n >= arrBuf[0].len)
		{
			memcpy(m_pBuf + nTail, arrBuf[0].buf, arrBuf[0].len);
			n -= arrBuf[0].len;
			nTail += arrBuf[0].len;
		}
		else
		{
			memcpy(m_pBuf + nTail, arrBuf[0].buf, n);
			nTail = arrBuf[0].len - n;
			memcpy(m_pBuf, arrBuf[0].buf + n, nTail);
			n = m_nHead;//不需要准确 队列已经切换为队尾在前对头在后了
		}
		if (arrBuf[1].len > 0)
		{
			if (n >= arrBuf[1].len)
			{
				memcpy(m_pBuf + nTail, arrBuf[1].buf, arrBuf[1].len);
				n -= arrBuf[1].len;
				nTail += arrBuf[1].len;
			}
			else
			{
				memcpy(m_pBuf + nTail, arrBuf[1].buf, n);
				nTail = arrBuf[1].len - n;
				memcpy(m_pBuf, arrBuf[1].buf + n, nTail);
				n = m_nHead;//不需要准确 队列已经切换为队尾在前对头在后了
			}
		}
		if(n >= (sizeof id))
			memcpy(m_pBuf + nTail, &id, sizeof id);
		else
		{
			const T *p = reinterpret_cast<const T*>(&id);
			memcpy(m_pBuf + nTail, p, n);
			memcpy(m_pBuf, p + n, (sizeof id) - n);
		}
		nTail = m_nTail;
		auto p = reinterpret_cast<const T*>(&nSize);
		for (uint32_t i = 0; i < sizeof(uint32_t) / sizeof(T); ++i)
		{
			m_pBuf[nTail] = p[i];
			nTail = (nTail + 1) % m_nMaxSize;
		}
		m_nTail = (m_nTail + nSize) % m_nMaxSize;
		return S_OK;
	}
	//可读取的数据缓冲区
	void GetReadEnable(NetBuffer arrBuf[2])
	{
		if(m_nHead <= m_nTail)
		{
			arrBuf[0].buf = &m_pBuf[m_nHead];
			arrBuf[0].len = m_nTail-m_nHead;
			arrBuf[1].buf = m_pBuf;
			arrBuf[1].len = 0;
		}
		else
		{
			arrBuf[0].buf = &m_pBuf[m_nHead];
			arrBuf[0].len = m_nMaxSize-m_nHead;
			arrBuf[1].buf = m_pBuf;
			arrBuf[1].len = m_nTail;
		}
	}
	//可写入的数据缓冲区
	void GetWriteEnable(NetBuffer arrBuf[2])
	{
		if(m_nHead > m_nTail || 0 == m_nHead)
		{
			arrBuf[0].buf = &m_pBuf[m_nTail];
			arrBuf[0].len = GetFreeSize();
			arrBuf[1].len = 0;
		}
		else
		{
			arrBuf[0].buf = &m_pBuf[m_nTail];
			arrBuf[0].len = m_nMaxSize-m_nTail;
			arrBuf[1].buf = m_pBuf;
			arrBuf[1].len = m_nHead-1;
		}
	}
	int32_t Push(T Data)
	{
		if(IsFull())
			return E_LESSMEMORY;
		m_pBuf[m_nTail] = Data;
		m_nTail = (m_nTail+1)%m_nMaxSize;
		return S_OK;
	}
	int32_t Pop(T &Data)
	{
		if(IsEmpty())
			return E_UNKNOW;
		Data = m_pBuf[m_nHead];
		m_nHead = (m_nHead+1)%m_nMaxSize;
		return S_OK;
	}
private:
	uint32_t m_nHead;
	uint32_t m_nTail;
	T  *m_pBuf;
	uint32_t m_nMaxSize;

};
#endif