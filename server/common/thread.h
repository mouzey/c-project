#ifndef _H_THREAD_H
#define _H_THREAD_H
#ifndef WIN32
#include <pthread.h>
typedef pthread_t	TID ;
#else
#include<windows.h> 
typedef DWORD		TID ;
#endif
#include "define.h"
//线程基类
class CThread
{
public:
	CThread();
	CThread(ThreadType type);
	virtual ~CThread();
public:
	void Start();
	void virtual Stop();
	void virtual Run();
	void virtual Exit(void *pResult=(void *)0);
	TID GetTID()const{return m_TID;}
	ThreadStatus GetStatus()const{return m_Status;}
	void SetStatus(ThreadStatus status){m_Status = status;}
	bool IsRun()const{return (enmThreadStatus_Runing == m_Status);}
	bool IsExiting()const{return (enmThreadStatus_Exiting == m_Status);}
	ThreadType GetThreadType()const{return m_Type;}
	void SetThreadType(ThreadType type){m_Type = type;}
	static ThreadType GetLocalThreadType(){return m_localType;}
	static void SetLocalThreadType(ThreadType type){m_localType = type;}
	//一般主线程调用
	void WaitExit();
private:
	ThreadStatus m_Status;
	ThreadType m_Type;
	TID m_TID;
	static THREAD_LOCAL ThreadType m_localType;//线程本地存储的一个线程类型变量
#ifdef WIN32
	HANDLE m_hThread ;
#endif
};
#ifdef WIN32
DWORD WINAPI MyThreadProcess(  void* p ) ;
#else
void * MyThreadProcess ( void * p ) ;
#endif
#endif