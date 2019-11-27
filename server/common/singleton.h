#ifndef _COMMON_SINGLETON_H_
#define _COMMON_SINGLETON_H_
#include <assert.h>
//单例
template<typename T>
class CSingleton
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
			assert(m_pInstance != nullptr);
			CSingleton<T>::GetGC();
		}
		return *m_pInstance;
	}
	static T& Instance(){return GetInstance();}
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
	static T	*m_pInstance;
protected:
	CSingleton()
	{
	}
	virtual ~CSingleton()
	{
	}
private:
	// Prohibited actions
	CSingleton(const CSingleton &);
	CSingleton& operator = (const CSingleton &);
private:
	//自动析构单例
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
	static CAutoGC m_gc;
	static CAutoGC& GetGC(){return m_gc;}
};
template<typename T>
T* CSingleton<T>::m_pInstance = nullptr;

template<typename T>
typename CSingleton<T>::CAutoGC CSingleton<T>::m_gc;
#endif
