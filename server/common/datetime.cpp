#include "pch.h"
#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <string.h>
#endif
#include "datetime.h"
#include "common_api.h"
#include "snprintf.h"
#include "define.h"
#ifdef USETIMEZONE
int32_t CDateTime::m_nTimeZone = GetTimeZone()*3600;
int32_t CSecondTime::m_nTimeZone = GetTimeZone()*3600;
#endif
CDateTime::CDateTime()
{
	m_nCurrTime = 0;
	memset(&m_tm,0,sizeof(m_tm));
}
CDateTime::~CDateTime()
{
}
CDateTime::CDateTime(uint64_t nTime)
{
	SetTime(nTime);
}
CDateTime::CDateTime(uint32_t nTime)
{
	SetTime(uint64_t(nTime)* 1000000);
}
CDateTime::CDateTime(time_t nTime)
{
	SetTime(nTime*1000000);
}
CDateTime::CDateTime(uint32_t nYear,uint32_t nMon,uint32_t nDay,uint32_t nHour/*=0*/,
					 uint32_t nMin/*=0*/,uint32_t nSec/*=0*/)
{
	m_tm.tm_year = nYear-1900;
	m_tm.tm_mon = nMon-1;
	m_tm.tm_mday = nDay;
	m_tm.tm_hour = nHour;
	m_tm.tm_min = nMin;
	m_tm.tm_sec = nSec;
	m_tm.tm_isdst = -1;
	m_nCurrTime = mktime(&m_tm)*1000000;
}
CDateTime::CDateTime(const string& szTime)
{
	Init(szTime.c_str());
}
CDateTime::CDateTime(const char *pTime)
{
	Init(pTime);
}
CDateTime::CDateTime(const tm& stTm)
{
	m_tm = stTm;
	m_nCurrTime = mktime(&m_tm)*1000000;
}
void CDateTime::Init(const char *pTime)
{
	SSCANF(pTime,"%4d-%02d-%02d %02d:%02d:%02d",&m_tm.tm_year,&m_tm.tm_mon,&m_tm.tm_mday,
		&m_tm.tm_hour,&m_tm.tm_min,&m_tm.tm_sec);
	m_tm.tm_year -= 1900;
	m_tm.tm_mon -= 1;
	m_tm.tm_isdst = -1;
	m_nCurrTime = mktime(&m_tm)*1000000;
}
 CDateTime CDateTime::CurrDateTime()    
 {
#ifdef WIN32
	 SYSTEMTIME stSys = {0};
	 GetLocalTime(&stSys) ;
	 uint64_t nNow =time(NULL)*1000000+stSys.wMilliseconds* uint64_t(1000);
#else
	 timeval stTimeValue = {0};
	 gettimeofday(&stTimeValue,NULL);
	 uint64_t nNow = stTimeValue.tv_sec*1000000+stTimeValue.tv_usec;
#endif
	 return CDateTime(nNow);
 }
 void CDateTime::Init()
 {
	 time_t nNow;
#ifdef WIN32
	 SYSTEMTIME stSys = {0};
	 GetLocalTime(&stSys) ;
	 nNow = time(NULL);
	 m_nCurrTime =nNow*1000000+stSys.wMilliseconds* uint64_t(1000);
#else
	 timeval stTimeValue = {0};
	 gettimeofday(&stTimeValue,NULL);
	 m_nCurrTime = stTimeValue.tv_sec*1000000+stTimeValue.tv_usec;
	 nNow = stTimeValue.tv_sec;
#endif
#ifdef WIN32
	 localtime_s(&m_tm,&nNow);
#else
	 localtime_r(&nNow,&m_tm);
#endif
 }
 void CDateTime::SetTime(int64_t nTime)
 {
	 m_nCurrTime = nTime;
	 time_t nNow = nTime/1000000;
#ifdef WIN32
	 localtime_s(&m_tm,&nNow);
#else
	 localtime_r(&nNow,&m_tm);
#endif
 }
 uint32_t	CDateTime::CurrTime(bool bFlag/*=false*/)
 {
	 if(bFlag) Init();
	 return GetSeconds();
 }
 bool CDateTime::IsSameDay(time_t nFirstTime,time_t nSecondTime)
 {
#ifdef USETIMEZONE
	 return (((nFirstTime+m_nTimeZone)/86400) == ((nSecondTime+m_nTimeZone)/86400));
#endif
	 tm stFirst ={0};
	 tm stSecond = {0};
#ifdef WIN32
	 localtime_s(&stFirst,&nFirstTime);
	 localtime_s(&stSecond,&nSecondTime);
#else
	 localtime_r(&nFirstTime,&stFirst);
	 localtime_r(&nSecondTime,&stSecond);
#endif
	 return ((stFirst.tm_year == stSecond.tm_year)&& (stFirst.tm_yday == stSecond.tm_yday));
 }
 bool CDateTime::IsSameWeek( time_t nFirstTime, time_t nSecondTime)
 {
	 tm stFirst ={0};
	 tm stSecond = {0};
#ifdef WIN32
	 localtime_s(&stFirst,&nFirstTime);
	 localtime_s(&stSecond,&nSecondTime);
#else
	 localtime_r(&nFirstTime,&stFirst);
	 localtime_r(&nSecondTime,&stSecond);
#endif
	 time_t nFirst = nFirstTime-stFirst.tm_wday* uint64_t(86400)-stFirst.tm_hour* uint64_t(3600)-stFirst.tm_min* uint64_t(60)-stFirst.tm_sec;
	 time_t nSecond = nSecondTime-stSecond.tm_wday* uint64_t(86400)-stSecond.tm_hour* uint64_t(3600)-stSecond.tm_min* uint64_t(60)-stSecond.tm_sec;
	 return (nFirst == nSecond);
 }
 bool CDateTime::IsSameMonth( time_t nFirstTime, time_t nSecondTime)
 {
	 tm stFirst ={0};
	 tm stSecond = {0};
#ifdef WIN32
	 localtime_s(&stFirst,&nFirstTime);
	 localtime_s(&stSecond,&nSecondTime);
#else
	 localtime_r(&nFirstTime,&stFirst);
	 localtime_r(&nSecondTime,&stSecond);
#endif
	 return ((stFirst.tm_year == stSecond.tm_year)&& (stFirst.tm_mon == stSecond.tm_mon));
 }
 bool CDateTime::IsSameYear( time_t nFirstTime, time_t nSecondTime)
 {
	 tm stFirst ={0};
	 tm stSecond = {0};
#ifdef WIN32
	 localtime_s(&stFirst,&nFirstTime);
	 localtime_s(&stSecond,&nSecondTime);
#else
	 localtime_r(&nFirstTime,&stFirst);
	 localtime_r(&nSecondTime,&stSecond);
#endif
	 return (stFirst.tm_year == stSecond.tm_year);
 }
 uint32_t CDateTime::DiffDay( time_t nFirstTime, time_t nSecondTime)
 {
#ifdef USETIMEZONE
	 return static_cast<uint32_t>(abs((nFirstTime+m_nTimeZone)/86400-(nSecondTime+m_nTimeZone)/86400));
#endif
	 tm stFirst ={0};
	 tm stSecond = {0};
#ifdef WIN32
	 localtime_s(&stFirst,&nFirstTime);
	 localtime_s(&stSecond,&nSecondTime);
#else
	 localtime_r(&nFirstTime,&stFirst);
	 localtime_r(&nSecondTime,&stSecond);
#endif
	 if(stFirst.tm_year == stSecond.tm_year)
		 return static_cast<uint32_t>(abs(stFirst.tm_yday - stSecond.tm_yday));
	 time_t nFirst = nFirstTime - stFirst.tm_hour* uint64_t(3600) - stFirst.tm_min* uint64_t(60) - stFirst.tm_sec;
	 time_t nSecond = nSecondTime - stSecond.tm_hour* uint64_t(3600) - stSecond.tm_min* uint64_t(60) - stSecond.tm_sec;
	 return static_cast<uint32_t>(abs(nFirst/86400-nSecond/86400));
 }
 uint32_t CDateTime::TotalDay(time_t nTime)
 {
#ifdef USETIMEZONE
	 return static_cast<uint32_t>((nTime+m_nTimeZone)/86400+1);
#endif
	 tm stFirst ={0};
#ifdef WIN32
	 localtime_s(&stFirst,&nTime);
#else
	 localtime_r(&nTime,&stFirst);
#endif
	 time_t nDayBegin = nTime-stFirst.tm_hour* uint64_t(3600)-stFirst.tm_min* uint64_t(60)-stFirst.tm_sec;
	 return static_cast<uint32_t>(nDayBegin/86400+1);
 }
 int32_t CDateTime::GetTimeStr(char *pTime,const uint32_t nMaxSize)const
 {
	return SNPRINTF(pTime,nMaxSize,"%4d-%02d-%02d %02d:%02d:%02d",m_tm.tm_year+1900,m_tm.tm_mon+1,m_tm.tm_mday,
		 m_tm.tm_hour, m_tm.tm_min,m_tm.tm_sec);
 }
 void CDateTime::GetTimeStr(string &szTime)const
 {
	 char szTemp[20];
	 SNPRINTF(szTemp,20,"%4d-%02d-%02d %02d:%02d:%02d",m_tm.tm_year+1900,m_tm.tm_mon+1,m_tm.tm_mday,
		 m_tm.tm_hour, m_tm.tm_min,m_tm.tm_sec);
	 szTime.append(szTemp);
 }
 int32_t CDateTime::GetNowTimeStr(char *pTime,uint32_t nMaxSize)
 {
	 time_t nNow = time(NULL);
	 tm stTemp ={0};
#ifdef WIN32
	 localtime_s(&stTemp,&nNow);
#else
	 localtime_r(&nNow,&stTemp);
#endif
	 return SNPRINTF(pTime,nMaxSize,"%4d-%02d-%02d %02d:%02d:%02d",stTemp.tm_year+1900,stTemp.tm_mon+1,
		 stTemp.tm_mday,stTemp.tm_hour,stTemp.tm_min,stTemp.tm_sec);
 }
 void CDateTime::GetNowTimeStr(string &szTime)
 {
	 char szTemp[20];
	 GetNowTimeStr(szTemp,20);
	 szTime.append(szTemp);
 }
 bool CDateTime::IsSameDay( time_t nTime)const
 {
	 tm stTemp ={0};
#ifdef WIN32
	 localtime_s(&stTemp,&nTime);
#else
	 localtime_r(&nTime,&stTemp);
#endif
	 return ((stTemp.tm_year == m_tm.tm_year)&& (stTemp.tm_yday == m_tm.tm_yday));
 }
 bool CDateTime::IsSameWeek( time_t nTime) const
 {
	 tm stFirst ={0};
#ifdef WIN32
	 localtime_s(&stFirst,&nTime);
#else
	 localtime_r(&nTime,&stFirst);
#endif
	 time_t nFirst = nTime-stFirst.tm_wday* uint64_t(86400)-stFirst.tm_hour* uint64_t(3600)-stFirst.tm_min* uint64_t(60)-stFirst.tm_sec;
	 return (nFirst == GetWeekBegine());
 }
 bool CDateTime::IsSameMonth( time_t nTime)const
 {
	 tm stFirst ={0};
#ifdef WIN32
	 localtime_s(&stFirst,&nTime);
#else
	 localtime_r(&nTime,&stFirst);
#endif
	 return ((stFirst.tm_year == m_tm.tm_year)&& (stFirst.tm_mon == m_tm.tm_mon));
 }
 bool CDateTime::IsSameYear( time_t nTime) const
 {
	 tm stFirst ={0};
#ifdef WIN32
	 localtime_s(&stFirst,&nTime);
#else
	 localtime_r(&nTime,&stFirst);
#endif
	 return (stFirst.tm_year == m_tm.tm_year);
 }
 uint32_t CDateTime::DiffDay(time_t nTime)const
 {
#ifdef USETIMEZONE
	 return static_cast<uint32_t>(abs((nTime+m_nTimeZone)/86400-(GetSeconds()+m_nTimeZone)/86400));
#endif
	 tm stFirst ={0};
#ifdef WIN32
	 localtime_s(&stFirst,&nTime);
#else
	 localtime_r(&nTime,&stFirst);
#endif
	 if(stFirst.tm_year == m_tm.tm_year)
		 return static_cast<uint32_t>(abs(stFirst.tm_yday - m_tm.tm_yday));
	 time_t nFirst = nTime - stFirst.tm_hour* uint64_t(3600) - stFirst.tm_min* uint64_t(60) - stFirst.tm_sec;
	 return static_cast<uint32_t>(abs(nFirst/86400-int32_t(GetDayBegine()/86400)));
 }
 bool CDateTime::IsSameDay(const  CDateTime& dateTime)const
 {
	 return ((dateTime.GetYear() == (uint32_t)m_tm.tm_year)&& (dateTime.GetYearDay() == (uint32_t)m_tm.tm_yday));
 }
 bool CDateTime::IsSameWeek(const  CDateTime& dateTime)const
 {
	  return (dateTime.GetWeekBegine() == GetWeekBegine());
 }
 bool CDateTime::IsSameMonth(const  CDateTime& dateTime)const
 {
	 return ((dateTime.GetYear() == (uint32_t)m_tm.tm_year)&& (dateTime.GetMonth() == (uint32_t)m_tm.tm_mon));
 }
 bool CDateTime::IsSameYear( const  CDateTime& dateTime)const
 {
	 return (dateTime.GetYear() == (uint32_t)m_tm.tm_year);
 }
 uint32_t CDateTime::DiffDay(const  CDateTime& dateTime)const
 {
	 if(dateTime.GetYear()== (uint32_t)m_tm.tm_year)
		 return static_cast<uint32_t>(abs(int32_t(dateTime.GetYearDay()) - m_tm.tm_yday));
#ifdef USETIMEZONE
	 return static_cast<uint32_t>(abs(int32_t((dateTime.GetSeconds()+m_nTimeZone)/86400)-int32_t((GetSeconds()+m_nTimeZone)/86400)));
#endif
	  return static_cast<uint32_t>(abs(int32_t(dateTime.GetDayBegine()/86400)-int32_t(GetDayBegine()/86400)));
 }
 bool CDateTime::IsSameWeekEx( time_t nFirstTime, time_t nSecondTime)
 {
	 tm stFirst ={0};
	 tm stSecond = {0};
#ifdef WIN32
	 localtime_s(&stFirst,&nFirstTime);
	 localtime_s(&stSecond,&nSecondTime);
#else
	 localtime_r(&nFirstTime,&stFirst);
	 localtime_r(&nSecondTime,&stSecond);
#endif
	 if( 0 == stFirst.tm_wday)
		 stFirst.tm_wday = 6;
	 else
		  --stFirst.tm_wday;
	 if( 0 == stSecond.tm_wday)
		 stSecond.tm_wday = 6;
	 else 
		 --stSecond.tm_wday;
	 time_t nFirst = nFirstTime-stFirst.tm_wday* uint64_t(86400)-stFirst.tm_hour* uint64_t(3600)-stFirst.tm_min* uint64_t(60)-stFirst.tm_sec;
	 time_t nSecond = nSecondTime-stSecond.tm_wday* uint64_t(86400)-stSecond.tm_hour* uint64_t(3600)-stSecond.tm_min* uint64_t(60)-stSecond.tm_sec;
	 return (nFirst == nSecond);
 }
 bool CDateTime::IsSameWeekEx( time_t nTime) const
 {
	 tm stFirst ={0};
#ifdef WIN32
	 localtime_s(&stFirst,&nTime);
#else
	 localtime_r(&nTime,&stFirst);
#endif
	 if( 0 == stFirst.tm_wday)
		 stFirst.tm_wday = 6;
	 else
		--stFirst.tm_wday;
	 uint32_t nWeekDay = m_tm.tm_wday;
	 if(0 == nWeekDay)
		 nWeekDay = 6;
	 else
		 --nWeekDay;
	 time_t nFirst = nTime-stFirst.tm_wday* uint64_t(86400)-stFirst.tm_hour* uint64_t(3600)-stFirst.tm_min* uint64_t(60)-stFirst.tm_sec;
	 time_t nSecond = GetDayBegine()-nWeekDay*24*3600;
	 return (nFirst == nSecond);
 }
 bool CDateTime::IsSameWeekEx(const  CDateTime& dateTime)const
 {
	 uint32_t nFirstWeekDay = dateTime.GetWeek();
	 uint32_t nSecondWeekDay = GetWeek();
	 if(0 == nFirstWeekDay)
		 nFirstWeekDay = 6;
	 else
		 --nFirstWeekDay;
	 if(0 == nSecondWeekDay)
		 nSecondWeekDay = 6;
	 else
		 --nSecondWeekDay;
	  time_t nFirst =dateTime.GetDayBegine()-nFirstWeekDay*24*3600;
	  time_t nSecond = GetDayBegine()-nSecondWeekDay*24*3600;
	  return (nFirst == nSecond);
 }
 uint32_t CDateTime::GetYearWeek()const
 {
	 uint32_t nWeekDay = m_tm.tm_wday;
	 if( 0 == nWeekDay)
		 nWeekDay = 7;
	 uint32_t nTemp= m_tm.tm_yday +1;
	 if(nTemp <= nWeekDay)
		 return 1;
	 else
		nTemp -=nWeekDay;//先减去本周的日期
	 uint32_t nYearWeek  = nTemp/7+1;//加上本周
	 if(nTemp %7 != 0)//一月一号可能不是周一所以可能没法整除
		 ++nYearWeek;
	 return nYearWeek;
 }
 string CDateTime::GetTimeStr()const
 {
	 char szTemp[20];
	 SNPRINTF(szTemp,20,"%4d-%02d-%02d %02d:%02d:%02d",m_tm.tm_year+1900,m_tm.tm_mon+1,m_tm.tm_mday,
		 m_tm.tm_hour, m_tm.tm_min,m_tm.tm_sec);
	 return std::string(szTemp);
 }
string CDateTime::GetNowTimeStr()
{
	char szTemp[20];
	GetNowTimeStr(szTemp,20);
	return std::string(szTemp);
}

CDateTime CDateTime::operator+( time_t nTime)const
{
	return CDateTime(GetMicroseconds()+nTime*1000000);
}
CDateTime& CDateTime::operator+=( time_t nTime )
{
	SetTime(m_nCurrTime+nTime*1000000);
	return *this;
}
CDateTime CDateTime::operator -(time_t nTime)const
{
	return CDateTime(GetMicroseconds()-nTime*1000000);
}
CDateTime& CDateTime::operator -=(time_t nTime)
{
	SetTime(m_nCurrTime-nTime*1000000);
	return *this;
}
CDateTime  CDateTime::Add(int32_t nDay,int32_t nHour/*=0*/,int32_t nMin/*=0*/,int32_t nSec/*=0*/)const
{
	return CDateTime(GetMicroseconds()+nDay* uint64_t(86400000000)+nHour* uint64_t(3600000000)+nMin* uint64_t(60000000)+nSec* uint64_t(1000000));
}
void CDateTime::AddTime(int32_t nDay,int32_t nHour/*=0*/,int32_t nMin/*=0*/,int32_t nSec/*=0*/)
{
	SetTime(m_nCurrTime+nDay* uint64_t(86400000000)+nHour* uint64_t(3600000000)+nMin* uint64_t(60000000)+nSec* uint64_t(1000000));
}
//
CSecondTime::CSecondTime()
{
	m_nCurrTime = 0;
	memset(&m_tm,0,sizeof(m_tm));
}
CSecondTime::~CSecondTime()
{
}
CSecondTime::CSecondTime(time_t nTime)
{
	SetTime(nTime);
}
CSecondTime::CSecondTime(uint32_t nYear,uint32_t nMon,uint32_t nDay,uint32_t nHour/*=0*/,
					 uint32_t nMin/*=0*/,uint32_t nSec/*=0*/)
{
	m_tm.tm_year = nYear-1900;
	m_tm.tm_mon = nMon-1;
	m_tm.tm_mday = nDay;
	m_tm.tm_hour = nHour;
	m_tm.tm_min = nMin;
	m_tm.tm_sec = nSec;
	m_tm.tm_isdst = -1;
	m_nCurrTime = mktime(&m_tm);
}
CSecondTime::CSecondTime(const string& szTime)
{
	Init(szTime.c_str());
}
CSecondTime::CSecondTime(const char *pTime)
{
	Init(pTime);
}
CSecondTime::CSecondTime(const tm& stTm)
{
	m_tm = stTm;
	m_nCurrTime = mktime(&m_tm);
}
void CSecondTime::Init(const char *pTime)
{
	SSCANF(pTime,"%4d-%02d-%02d %02d:%02d:%02d",&m_tm.tm_year,&m_tm.tm_mon,&m_tm.tm_mday,
		&m_tm.tm_hour,&m_tm.tm_min,&m_tm.tm_sec);
	m_tm.tm_year -= 1900;
	m_tm.tm_mon -= 1;
	m_tm.tm_isdst = -1;
	m_nCurrTime = mktime(&m_tm);
}
CSecondTime CSecondTime::CurrDateTime()    
{
	return CSecondTime(time(nullptr));
}
void CSecondTime::Init()
{
	m_nCurrTime = time(NULL);
#ifdef WIN32
	localtime_s(&m_tm,&m_nCurrTime);
#else
	localtime_r(&m_nCurrTime,&m_tm);
#endif
}
void CSecondTime::SetTime(time_t nTime)
{
	m_nCurrTime = nTime;
#ifdef WIN32
	localtime_s(&m_tm,&m_nCurrTime);
#else
	localtime_r(&m_nCurrTime,&m_tm);
#endif
}
uint32_t CSecondTime::CurrTime(bool bFlag/*=false*/)
{
	if(bFlag) Init();
	return GetSeconds();
}

int32_t CSecondTime::GetTimeStr(char *pTime,const uint32_t nMaxSize)const
{
	return SNPRINTF(pTime,nMaxSize,"%4d-%02d-%02d %02d:%02d:%02d",m_tm.tm_year+1900,m_tm.tm_mon+1,m_tm.tm_mday,
		m_tm.tm_hour, m_tm.tm_min,m_tm.tm_sec);
}
void CSecondTime::GetTimeStr(string &szTime)const
{
	char szTemp[20];
	SNPRINTF(szTemp,20,"%4d-%02d-%02d %02d:%02d:%02d",m_tm.tm_year+1900,m_tm.tm_mon+1,m_tm.tm_mday,
		m_tm.tm_hour, m_tm.tm_min,m_tm.tm_sec);
	szTime.append(szTemp);
}
bool CSecondTime::IsSameDay( time_t nTime)const
{
	tm stTemp ={0};
#ifdef WIN32
	localtime_s(&stTemp,&nTime);
#else
	localtime_r(&nTime,&stTemp);
#endif
	return ((stTemp.tm_year == m_tm.tm_year)&& (stTemp.tm_yday == m_tm.tm_yday));
}
bool CSecondTime::IsSameWeek( time_t nTime) const
{
	tm stFirst ={0};
#ifdef WIN32
	localtime_s(&stFirst,&nTime);
#else
	localtime_r(&nTime,&stFirst);
#endif
	time_t nFirst = nTime-stFirst.tm_wday* uint64_t(86400)-stFirst.tm_hour* uint64_t(3600)-stFirst.tm_min* uint64_t(60)-stFirst.tm_sec;
	return (nFirst == GetWeekBegine());
}
bool CSecondTime::IsSameMonth( time_t nTime)const
{
	tm stFirst ={0};
#ifdef WIN32
	localtime_s(&stFirst,&nTime);
#else
	localtime_r(&nTime,&stFirst);
#endif
	return ((stFirst.tm_year == m_tm.tm_year)&& (stFirst.tm_mon == m_tm.tm_mon));
}
bool CSecondTime::IsSameYear( time_t nTime) const
{
	tm stFirst ={0};
#ifdef WIN32
	localtime_s(&stFirst,&nTime);
#else
	localtime_r(&nTime,&stFirst);
#endif
	return (stFirst.tm_year == m_tm.tm_year);
}
uint32_t CSecondTime::DiffDay(time_t nTime)const
{
#ifdef USETIMEZONE
	return static_cast<uint32_t>(abs((nTime+m_nTimeZone)/86400-(GetSeconds()+m_nTimeZone)/86400));
#endif
	tm stFirst ={0};
#ifdef WIN32
	localtime_s(&stFirst,&nTime);
#else
	localtime_r(&nTime,&stFirst);
#endif
	if(stFirst.tm_year == m_tm.tm_year)
		return static_cast<uint32_t>(abs(stFirst.tm_yday - m_tm.tm_yday));
	time_t nFirst = nTime - stFirst.tm_hour* uint64_t(3600) - stFirst.tm_min* uint64_t(60) - stFirst.tm_sec;
	return static_cast<uint32_t>(abs(nFirst/86400-int32_t(GetDayBegine()/86400)));
}
bool CSecondTime::IsSameDay(const  CSecondTime& dateTime)const
{
	return ((dateTime.GetYear() == (uint32_t)m_tm.tm_year)&& (dateTime.GetYearDay() == (uint32_t)m_tm.tm_yday));
}
bool CSecondTime::IsSameWeek(const  CSecondTime& dateTime)const
{
	return (dateTime.GetWeekBegine() == GetWeekBegine());
}
bool CSecondTime::IsSameMonth(const  CSecondTime& dateTime)const
{
	return ((dateTime.GetYear() == (uint32_t)m_tm.tm_year)&& (dateTime.GetMonth() == (uint32_t)m_tm.tm_mon));
}
bool CSecondTime::IsSameYear( const  CSecondTime& dateTime)const
{
	return (dateTime.GetYear() == (uint32_t)m_tm.tm_year);
}
uint32_t CSecondTime::DiffDay(const  CSecondTime& dateTime)const
{
	if(dateTime.GetYear()== (uint32_t)m_tm.tm_year)
		return static_cast<uint32_t>(abs(int32_t(dateTime.GetYearDay()) - m_tm.tm_yday));
#ifdef USETIMEZONE
	return static_cast<uint32_t>(abs(int32_t((dateTime.GetSeconds()+m_nTimeZone)/86400)-int32_t((GetSeconds()+m_nTimeZone)/86400)));
#endif
	return static_cast<uint32_t>(abs(int32_t(dateTime.GetDayBegine()/86400)-int32_t(GetDayBegine()/86400)));
}
bool CSecondTime::IsSameWeekEx( time_t nTime) const
{
	tm stFirst ={0};
#ifdef WIN32
	localtime_s(&stFirst,&nTime);
#else
	localtime_r(&nTime,&stFirst);
#endif
	if( 0 == stFirst.tm_wday)
		stFirst.tm_wday = 6;
	else
		--stFirst.tm_wday;
	uint32_t nWeekDay = m_tm.tm_wday;
	if(0 == nWeekDay)
		nWeekDay = 6;
	else
		--nWeekDay;
	time_t nFirst = nTime-stFirst.tm_wday* uint64_t(86400)-stFirst.tm_hour* uint64_t(3600)-stFirst.tm_min* uint64_t(60)-stFirst.tm_sec;
	time_t nSecond = GetDayBegine()-nWeekDay*24*3600;
	return (nFirst == nSecond);
}
bool CSecondTime::IsSameWeekEx(const  CSecondTime& dateTime)const
{
	uint32_t nFirstWeekDay = dateTime.GetWeek();
	uint32_t nSecondWeekDay = GetWeek();
	if(0 == nFirstWeekDay)
		nFirstWeekDay = 6;
	else
		--nFirstWeekDay;
	if(0 == nSecondWeekDay)
		nSecondWeekDay = 6;
	else
		--nSecondWeekDay;
	time_t nFirst =dateTime.GetDayBegine()-nFirstWeekDay*24*3600;
	time_t nSecond = GetDayBegine()-nSecondWeekDay*24*3600;
	return (nFirst == nSecond);
}
uint32_t CSecondTime::GetYearWeek()const
{
	uint32_t nWeekDay = m_tm.tm_wday;
	if( 0 == nWeekDay)
		nWeekDay = 7;
	uint32_t nTemp= m_tm.tm_yday+1;
	if(nTemp <= nWeekDay)
		return 1;
	else
		nTemp -=nWeekDay;//先减去本周的日期
	uint32_t nYearWeek  = nTemp/7+1;//加上本周
	if(nTemp %7 != 0)//一月一号可能不是周一所以可能没法整除
		++nYearWeek;
	return nYearWeek;
}
string CSecondTime::GetTimeStr()const
{
	char szTemp[20];
	SNPRINTF(szTemp,20,"%4d-%02d-%02d %02d:%02d:%02d",m_tm.tm_year+1900,m_tm.tm_mon+1,m_tm.tm_mday,
		m_tm.tm_hour, m_tm.tm_min,m_tm.tm_sec);
	return std::string(szTemp);
}
CSecondTime CSecondTime::operator+( time_t nTime)const
{
	return CSecondTime(GetSeconds()+nTime);
}
CSecondTime& CSecondTime::operator+=( time_t nTime )
{
	SetTime(m_nCurrTime+nTime);
	return *this;
}
CSecondTime CSecondTime::operator -(time_t nTime)const
{
	return CSecondTime(GetSeconds()-nTime);
}
CSecondTime& CSecondTime::operator -=(time_t nTime)
{
	SetTime(m_nCurrTime-nTime);
	return *this;
}
CSecondTime  CSecondTime::Add(int32_t nDay,int32_t nHour/*=0*/,int32_t nMin/*=0*/,int32_t nSec/*=0*/)const
{
	return CSecondTime(GetSeconds()+nDay* uint64_t(86400)+nHour* uint64_t(3600)+nMin* uint64_t(60)+nSec);
}
void CSecondTime::AddTime(int32_t nDay,int32_t nHour/*=0*/,int32_t nMin/*=0*/,int32_t nSec/*=0*/)
{
	SetTime(m_nCurrTime+nDay*uint64_t(86400)+nHour* uint64_t(3600)+nMin* uint64_t(60)+nSec);
}