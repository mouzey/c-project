#include "pch.h"
#ifdef WIN32
#include <io.h>
#else
#include <sys/types.h>  
#include <sys/stat.h>  
#include <stdarg.h>
#include <unistd.h>
#endif
#include "log.h"
#include "datetime.h"
#include "snprintf.h"
#include "common_api.h"
#include "singleton.h"
THREAD_LOCAL ThreadType CLog::m_queueIndex=enmThreadType_Main;
CLog* g_log =  &CSingleton<CLog>::Instance();
const static char* const arrLogLevel[]={
"[DEBUG] ",
"[INFO] ",
"[WARING] ",
"[ERROR] ",
"[CRISIS] "
};
const static int8_t arrLogSize[]={8,7,9,8,9};
 CLog::CLog(LogLevel level/*=enmLogLevel_Debug*/,const char *pszFileName/*="frame"*/)
{
	m_logLevel = level;
	m_nNowDate = CDateTime::CurrDateTime().CurrentDate();
	SetFileName(pszFileName);
	m_pFile = nullptr;
	m_nLastSaveTime = 0;
	m_bLogThreadRun = false;
}
 CLog::~CLog()
{
	 Close();
}
 void CLog::SetFileName(const char* pFilename)
 {
	 SNPRINTF(m_szFileName, MaxFileName, "%s_%s_%d.txt", DEFUALTLOGNAME, pFilename, m_nNowDate);
	 SNPRINTF(m_szTempName, MaxFileName, "%s", pFilename);
 }
 void CLog::WriteLog(LogLevel level,const char * pFile,uint32_t nLine,const char *pBuf,...)
 {
	 if(level < m_logLevel || level > enmLogLevel_Crisis || nullptr == pBuf) return;
	 char szBuf[MaxOneLogSize];
	 uint32_t nSize = arrLogSize[level];
	 memcpy(szBuf,arrLogLevel[level],nSize);
	 CDateTime dt = CDateTime::CurrDateTime();
	 nSize += SNPRINTF(szBuf+nSize,MaxOneLogSize-nSize,"%04u-%02u-%02u %02u:%02u:%02u [%s:%u] ",
		 dt.GetYear(),dt.GetMonth(),dt.GetDay(),dt.GetHour(),dt.GetMinute(),dt.GetSecond(),pFile,nLine);
	 va_list list;
	 va_start(list,pBuf);
	 int32_t nRet = VSNPRINTF(szBuf+nSize,MaxOneLogSize-nSize,pBuf,list);
	  va_end(list);
	  if (uint32_t(nRet) >= MaxOneLogSize - nSize)
	  {
		  WriteLog(enmLogLevel_Waring,pFile,nLine,"log is to long");
		  return;
	  }
	 nSize += nRet;
	 szBuf[nSize]='\n';
	 nSize += 1;
	 if (m_bLogThreadRun)
	 {
		 m_mutex.lock();
		 auto free = m_nBuf.GetFreeSize();
		 if (!m_strLogBuff.empty())
		 {
			 if (free >= m_strLogBuff.size())
			 {
				 m_nBuf.AddOnlyBuf(m_strLogBuff.data(), (uint32_t)m_strLogBuff.size());
				 free -= (uint32_t)m_strLogBuff.size();
				 m_strLogBuff.clear();
			 }
			 else
			 {
				 m_nBuf.AddOnlyBuf(m_strLogBuff.data(), free);
				 m_strLogBuff.erase(0, free);
				 free = 0;
			 }
		 }
		 if (free >= nSize)
			m_nBuf.AddOnlyBuf(szBuf,nSize);
		 else
		 {
			 m_nBuf.AddOnlyBuf(szBuf, free);
			 m_strLogBuff.append(szBuf + free, nSize - free);
			 m_strLogBuff.append("------log is full--------\n");
		 }
		 m_mutex.unlock();
	 }
	 else
	 {
		 WriteToFile(szBuf,nSize);
	 }
 }
 bool CLog::Flush()
 {
	 if(m_nBuf.IsEmpty()) 
		 return true;
	 if(nullptr == m_pFile)
	 {
		 m_pFile = Fopen(m_szFileName,"a+");
		 if(nullptr == m_pFile) 
			 return true;
	 }
	 NetBuffer arrBuf[2];
	 m_nBuf.GetReadEnable(arrBuf);
	auto nSize = fwrite(arrBuf[0].buf,1,arrBuf[0].len,m_pFile);
	if(arrBuf[1].len > 0 && nSize == arrBuf[0].len)
		nSize += fwrite(arrBuf[1].buf,1,arrBuf[1].len,m_pFile);
	m_nBuf.Remove((uint32_t)nSize);
	Check();
	return m_nBuf.IsEmpty();
 }
 void CLog::Check()
 {
	 CDateTime dt = CDateTime::CurrDateTime();
	 if(m_pFile &&(dt.GetMilliTime() >= m_nLastSaveTime+FULSHLOGTIME))
	 {
		 fflush(m_pFile);
		 m_nLastSaveTime = dt.GetMilliTime();
	 }
	 if (dt.CurrentDate() != m_nNowDate)
	 {
		 m_nNowDate = dt.CurrentDate();
		 Close();
		 SNPRINTF(m_szFileName,MaxFileName,"%s_%s_%d.txt",DEFUALTLOGNAME,m_szTempName,m_nNowDate);
		 return;
	 }
	 return;//去掉文件大小检查
#ifdef WIN32
	 if(NULL == m_pFile)
		  m_pFile = Fopen(m_szFileName,"r");
	 if(NULL == m_pFile) return;
	 uint32_t nSize =  _filelength(_fileno(m_pFile));
#else
	 struct stat buf;  
	 if (stat (m_szFileName, &buf) != 0) return;
	 uint32_t nSize = buf.st_size;
#endif
	  if(nSize < MaxLogSize) return;
	  char szNewName[MaxFileName] = {0};
	  int32_t n=1;
	  while (true)
	  {
		  SNPRINTF(szNewName,MaxFileName,"%s_%d",m_szFileName,n);
		  int32_t nRet = access(szNewName,F_OK);
		  if(S_OK != nRet) break;//文件不存在
		  ++n;//文件存在继续找
	  }
	  if(m_pFile)
	  {
		  fclose(m_pFile);
		  m_pFile = NULL;
	  }
	  if(S_OK !=  rename(m_szFileName,szNewName) )
	  {
		  printf("change name error");
	  }
 }
 void CLog::WriteToFile(const char *pBuf,uint32_t nSize)
 {
	 Check();
	 if(NULL == m_pFile)
	 {
		 m_pFile = Fopen(m_szFileName,"a+");
		 if(NULL == m_pFile) return;
	 }
	 fwrite(pBuf, 1, nSize, m_pFile);
 }
 void CLog::Close()
 {
	 if(m_pFile)
	 {
		 fclose(m_pFile);
		 m_pFile = NULL;
	 }
 }