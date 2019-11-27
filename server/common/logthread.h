#ifndef _H_LOGTHREAD_H
#define _H_LOGTHREAD_H
#include "thread.h"
#include "singleton.h"
//日志线程
class CLogThread:public CThread,public CSingleton<CLogThread>
{
public:
	CLogThread();
	~CLogThread();
	void virtual Run()override;
};
#endif