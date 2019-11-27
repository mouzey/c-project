#include "pch.h"
#include "thread.h"
#include "log.h"
THREAD_LOCAL ThreadType CThread::m_localType = enmThreadType_Main;
CThread::CThread()
{
	m_Status = enmThreadStatus_Ready;
	m_Type = enmThreadType_Main;
	m_TID = 0;
#ifdef WIN32
	 m_hThread =NULL;
#endif
}
CThread::CThread(ThreadType type)
{
	m_Status = enmThreadStatus_Ready;
	m_Type = type;
	m_TID = 0;
#ifdef WIN32
	m_hThread =NULL;
#endif
}
CThread::~CThread()
{

}
void CThread::Start()
{
#ifdef WIN32
	m_hThread = ::CreateThread( NULL, 0, MyThreadProcess , this, 0, &m_TID ) ;
#else
	pthread_create( &m_TID, NULL , MyThreadProcess , this );
#endif
}
void CThread::Stop()
{
	m_Status = enmThreadStatus_Exiting;
}
void CThread::Run()
{

}
void CThread::Exit(void *pResult/*=(void *)0*/)
{
#ifndef WIN32
	pthread_exit( pResult );
#else
	::CloseHandle( m_hThread );
#endif
}
void CThread::WaitExit()
{
#ifndef WIN32
	pthread_join(m_TID,nullptr);
#else
	WaitForSingleObject(m_hThread, INFINITE);  
#endif
}
#ifdef WIN32
DWORD WINAPI MyThreadProcess(  void* p )
#else
void* MyThreadProcess(void* p)
#endif
{
	CThread * pThread = (CThread *)p;
	pThread->SetStatus(enmThreadStatus_Runing);
	CThread::SetLocalThreadType(pThread->GetThreadType());
	CLog::SetQueueIndex(pThread->GetThreadType());
	pThread->Run();
	pThread->SetStatus(enmThreadStatus_Exit);
	pThread->Exit();
#ifdef WIN32
	return 0;
#else
	return nullptr;
#endif
}