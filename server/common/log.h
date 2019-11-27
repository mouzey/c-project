#ifndef _H_LOG_H
#define _H_LOG_H
#include <mutex>
#include "define.h"
#include "newqueue.h"
//文件日志
class CLog
{
public:
	CLog(LogLevel level=enmLogLevel_Debug,const char *pszFileName="frame");
	~CLog();
public:
	void WriteLog(LogLevel level,const char * pFile,uint32_t nLine,const char *pBuf,...)CHECK_FORMAT(5,6);
	void SetLogLevel(LogLevel level){m_logLevel = level;}
	void SetFileName(const char* pFilename);
	bool Flush();
	void Check();
	void LogThreadRun(){m_bLogThreadRun=true;}
	void WriteToFile(const char *pBuf,uint32_t nSize);
	void Close();
	static void SetQueueIndex(ThreadType index){m_queueIndex=index;}
private:
	bool m_bLogThreadRun;
	LogLevel m_logLevel;
	CNewQueue<char> m_nBuf;//日志都是字符串
	std::string m_strLogBuff;//m_nBuf满的时候使用 可能存在的
	char m_szFileName[MaxFileName];
	char m_szTempName[MaxFileName];
	FILE *m_pFile;
	uint64_t m_nLastSaveTime;
	uint32_t m_nNowDate;
	std::mutex m_mutex;
	static THREAD_LOCAL ThreadType m_queueIndex;//多线程中避免锁则每个线程一个日志队列
};
extern CLog *g_log;
#define  WRITE_DEBUG_LOG( pattern,...)   g_log->WriteLog(enmLogLevel_Debug, __FILE__,__LINE__,pattern,##__VA_ARGS__)
#define  WRITE_INFO_LOG( pattern,...)    g_log->WriteLog(enmLogLevel_Info, __FILE__,__LINE__,pattern,##__VA_ARGS__)
#define  WRITE_WARING_LOG( pattern,...)  g_log->WriteLog(enmLogLevel_Waring, __FILE__,__LINE__,pattern,##__VA_ARGS__)
#define  WRITE_ERROR_LOG( pattern,...)   g_log->WriteLog(enmLogLevel_Error, __FILE__,__LINE__,pattern,##__VA_ARGS__)
#define  WRITE_CRISIS_LOG( pattern,...)  g_log->WriteLog(enmLogLevel_Crisis, __FILE__,__LINE__,pattern,##__VA_ARGS__)
#endif