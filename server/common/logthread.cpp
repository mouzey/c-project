#include "pch.h"
#include "logthread.h"
#include "log.h"
#include "common_api.h"
CLogThread::CLogThread()
{
}
CLogThread::~CLogThread()
{

}
void CLogThread::Run()
{
	g_log->LogThreadRun();
	while (IsRun())
	{
		if(g_log->Flush())
			MySleep(1);
	}
	g_log->Flush();
	g_log->Close();
}
