#ifndef _COMMON_LOCALSINGLETON_H_
#define _COMMON_LOCALSINGLETON_H_
#include <cassert>
#include "define.h"
//线程本地存储单例
template<typename T>
class CLocalSingleton
{
public:
	//创建实例
	static T& CreateInstance()
	{
		return GetInstance();
	}
	//获取实例
	static T& GetInstance()
	{
		if (nullptr == m_pInstance)
		{
			m_pInstance = new T();
#ifndef  WIN32
			CLocalSingleton<T>::GetGC();
#endif
		}
		return *m_pInstance;
	}
	//销毁实例
	static void DestroyInstance()
	{
		if (nullptr != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
	//用新的实例将旧的实例换出来
	static T* SwapInstance(T* new_one)
	{
		T* old_one = m_pInstance;
		m_pInstance = new_one;
		return old_one;
	}
protected:
	THREAD_LOCAL static T	*m_pInstance;
protected:
	CLocalSingleton()
	{
	}
	virtual ~CLocalSingleton()
	{
	}
private:
	// Prohibited actions
	CLocalSingleton(const CLocalSingleton &);
	CLocalSingleton& operator = (const CLocalSingleton &);
private:
	//自动析构单例
#ifndef  WIN32
	class CAutoGC
	{
	public:
		~CAutoGC()
		{
			if(nullptr != m_pInstance)
			{
				delete m_pInstance;
				m_pInstance = nullptr;
			}
		}
	};
	THREAD_LOCAL static CAutoGC m_gc;
	static CAutoGC& GetGC(){return m_gc;}
#endif
};
template<typename T>
THREAD_LOCAL T* CLocalSingleton<T>::m_pInstance = nullptr;
#ifndef  WIN32
template<typename T>
THREAD_LOCAL typename CLocalSingleton<T>::CAutoGC CLocalSingleton<T>::m_gc;
#endif
#endif
