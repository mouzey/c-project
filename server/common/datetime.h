#ifndef _H_DATETIME_H
#define _H_DATETIME_H
#include <time.h>
#include <string>
#include <cstdint>
#define USETIMEZONE
//时间类
using namespace std;
class CDateTime
{
public:
	CDateTime();
	~CDateTime();
	CDateTime(uint64_t nTime);//微妙
	CDateTime(uint32_t nTime);//秒
	CDateTime(time_t nTime);//秒
	CDateTime(uint32_t nYear,uint32_t nMon,uint32_t nDay,uint32_t nHour = 0,uint32_t nMin = 0,uint32_t nSec = 0);
	CDateTime(const string& szTime);//格式为2015-01-01 10:30:24
	CDateTime(const char *pTime);//格式为2015-01-01 10:30:24
	CDateTime(const tm& stTm);
public:
	static CDateTime CurrDateTime();
	void Init();
	void Init(const char *pTime);
	void SetTime(int64_t nTime);//微妙
	uint32_t CurrTime(bool bFlag=false);//秒
	uint32_t GetYear( )const{ return m_tm.tm_year+1900 ; } 	//[1900,????]
	uint32_t GetMonth( )const{ return m_tm.tm_mon+1 ; } 	//[1,12]
	uint32_t GetDay( )const{ return m_tm.tm_mday ; } 		//[1,31]
	uint32_t GetHour( )const{ return m_tm.tm_hour ; } 		//[0,23]
	uint32_t GetMinute( )const{ return m_tm.tm_min ; } 		//[0,59]
	uint32_t GetSecond( )const{ return m_tm.tm_sec ; } 		//[0,59]
	uint32_t GetWeek( )const{ return m_tm.tm_wday ; } //取得当前是星期几；0表示：星期天，1～6表示：星期一～星期六
	uint32_t GetWeekEx()const { return  0 == m_tm.tm_wday ? 7 : m_tm.tm_wday; } //取得当前是星期几；7表示：星期天，1～6表示：星期一～星期六
	uint32_t GetYearDay( )const{ return m_tm.tm_yday ; }//[0.365]
	uint32_t CurrentDate()const{return GetYear()*10000+GetMonth()*100+GetDay();}//20131229
	uint32_t GetSeconds() const {return static_cast<uint32_t>(m_nCurrTime/1000000);} //当前秒
	uint32_t GetDayBegine()const 
	{
		return static_cast<uint32_t>(m_nCurrTime/1000000)-m_tm.tm_hour*3600-m_tm.tm_min*60-m_tm.tm_sec;
	}
	uint32_t GetWeekBegine()const//星期日为一周的起点
	{
		return static_cast<uint32_t>(m_nCurrTime/1000000)-m_tm.tm_wday*24*3600-m_tm.tm_hour*3600
									 -m_tm.tm_min*60-m_tm.tm_sec;
	}
	uint64_t GetMilliTime()const {return m_nCurrTime/1000;}//毫秒
	uint64_t GetMicroseconds()const{return m_nCurrTime;}//微秒
	uint64_t GetDetailTime()const//20160102214523 =2016-01-02 21:45:23
	{ 
		uint64_t nTime = GetYear();
		return (nTime*100*100*100*100*100+GetMonth()*100ull*100*100*100+GetDay()*100ull*100*100
			+GetHour()*100ull*100+GetMinute()*100ull+GetSecond());
	}
	int32_t GetTimeStr(char *pTime,const uint32_t nMaxSize)const;
	void GetTimeStr(string &szTime)const;
	//参数为秒utc时间
	static bool IsSameDay( time_t nFirstTime, time_t nSecondTime);
	static bool IsSameWeek( time_t nFirstTime, time_t nSecondTime);//这个是以星期天为一周的起点
	static bool IsSameMonth( time_t nFirstTime, time_t nSecondTime);
	static bool IsSameYear( time_t nFirstTime, time_t nSecondTime);
	static uint32_t DiffDay(time_t nFirstTime,time_t nSecondTime);
	static uint32_t TotalDay(time_t nTime);
	static int32_t GetNowTimeStr(char *pTime,uint32_t nMaxSize);
	static void GetNowTimeStr(string &szTime);
	bool IsSameDay( time_t nTime)const;
	bool IsSameWeek( time_t nTime) const;//这个是以星期天为一周的起点
	bool IsSameMonth( time_t nTime)const;
	bool IsSameYear( time_t nTime) const;
	uint32_t DiffDay(time_t nTime)const;
	bool IsSameDay(const  CDateTime& dateTime)const;
	bool IsSameWeek(const  CDateTime& dateTime)const;//这个是以星期天为一周的起点
	bool IsSameMonth(const  CDateTime& dateTime)const;
	bool IsSameYear( const  CDateTime& dateTime)const;
	uint32_t DiffDay(const  CDateTime& dateTime)const;
	//下面是以周一为一周起点判断
	static bool IsSameWeekEx( time_t nFirstTime, time_t nSecondTime);
	bool IsSameWeekEx( time_t nTime) const;
	bool IsSameWeekEx(const  CDateTime& dateTime)const;
	uint32_t GetYearWeek()const;//一年中的第几周，周一为开始 一月一日为第一周，可能不是周一
	const tm& GetTm()const{return m_tm;}
	string GetTimeStr()const;
	static string GetNowTimeStr();
	CDateTime operator +(time_t nTime)const;
	CDateTime& operator +=(time_t nTime);
	CDateTime operator -(time_t nTime)const;
	CDateTime& operator -=(time_t nTime);
	CDateTime  Add(int32_t nDay,int32_t nHour=0,int32_t nMin=0,int32_t nSec=0)const;
	void AddTime(int32_t nDay,int32_t nHour=0,int32_t nMin=0,int32_t nSec=0);
private:
	uint64_t m_nCurrTime;//微妙
	tm m_tm;
#ifdef USETIMEZONE
	static int32_t m_nTimeZone;//时区偏移时间
#endif

};
//
class CSecondTime
{
public:
	CSecondTime();
	~CSecondTime();
	CSecondTime(time_t nTime);//秒
	CSecondTime(uint32_t nYear,uint32_t nMon,uint32_t nDay,uint32_t nHour = 0,uint32_t nMin = 0,uint32_t nSec = 0);
	CSecondTime(const string& szTime);//格式为2015-01-01 10:30:24
	CSecondTime(const char *pTime);//格式为2015-01-01 10:30:24
	CSecondTime(const tm& stTm);
public:
	static CSecondTime CurrDateTime();
	void Init();
	void Init(const char *pTime);
	void SetTime(time_t nTime);//秒
	uint32_t CurrTime(bool bFlag=false);//秒
	uint32_t GetYear( )const{ return m_tm.tm_year+1900 ; } 	//[1900,????]
	uint32_t GetMonth( )const{ return m_tm.tm_mon+1 ; } 	//[1,12]
	uint32_t GetDay( )const{ return m_tm.tm_mday ; } 		//[1,31]
	uint32_t GetHour( )const{ return m_tm.tm_hour ; } 		//[0,23]
	uint32_t GetMinute( )const{ return m_tm.tm_min ; } 		//[0,59]
	uint32_t GetSecond( )const{ return m_tm.tm_sec ; } 		//[0,59]
	uint32_t GetWeek( )const{ return m_tm.tm_wday ; } //取得当前是星期几；0表示：星期天，1～6表示：星期一～星期六
	uint32_t GetWeekEx()const { return  0 == m_tm.tm_wday ? 7 : m_tm.tm_wday; } //取得当前是星期几；7表示：星期天，1～6表示：星期一～星期六
	uint32_t GetYearDay( )const{ return m_tm.tm_yday ; }//[0.365]
	uint32_t CurrentDate()const{return GetYear()*10000+GetMonth()*100+GetDay();}//20131229
	uint32_t GetSeconds() const {return static_cast<uint32_t>(m_nCurrTime);} //当前秒
	uint32_t GetDayBegine()const 
	{
		return static_cast<uint32_t>(m_nCurrTime)-m_tm.tm_hour*3600-m_tm.tm_min*60-m_tm.tm_sec;
	}
	uint32_t GetWeekBegine()const//周日为一周起点
	{
		return static_cast<uint32_t>(m_nCurrTime)-m_tm.tm_wday*24*3600-m_tm.tm_hour*3600
			-m_tm.tm_min*60-m_tm.tm_sec;
	}
	uint64_t GetDetailTime()const//20160102214523 =2016-01-02 21:45:23
	{ 
		uint64_t nTime = GetYear();
		return (nTime*100*100*100*100*100+GetMonth()*100ull*100*100*100+GetDay()*100ull*100*100
			+GetHour()*100ull*100+GetMinute()*100ull+GetSecond());
	}
	int32_t GetTimeStr(char *pTime,const uint32_t nMaxSize)const;
	void GetTimeStr(string &szTime)const;
	bool IsSameDay( time_t nTime)const;
	bool IsSameWeek( time_t nTime) const;//这个是以星期天为一周的起点
	bool IsSameMonth( time_t nTime)const;
	bool IsSameYear( time_t nTime) const;
	uint32_t DiffDay(time_t nTime)const;
	bool IsSameDay(const  CSecondTime& dateTime)const;
	bool IsSameWeek(const  CSecondTime& dateTime)const;//这个是以星期天为一周的起点
	bool IsSameMonth(const  CSecondTime& dateTime)const;
	bool IsSameYear( const  CSecondTime& dateTime)const;
	uint32_t DiffDay(const  CSecondTime& dateTime)const;
	//下面是以周一为一周起点判断
	bool IsSameWeekEx( time_t nTime) const;
	bool IsSameWeekEx(const  CSecondTime& dateTime)const;
	uint32_t GetYearWeek()const;//一年中的第几周，周一为开始 一月一日为第一周，可能不是周一
	const tm& GetTm()const{return m_tm;}
	string GetTimeStr()const;
	CSecondTime operator +(time_t nTime)const;
	CSecondTime& operator +=(time_t nTime);
	CSecondTime operator -(time_t nTime)const;
	CSecondTime& operator -=(time_t nTime);
	CSecondTime  Add(int32_t nDay,int32_t nHour=0,int32_t nMin=0,int32_t nSec=0)const;
	void AddTime(int32_t nDay,int32_t nHour=0,int32_t nMin=0,int32_t nSec=0);
private:
	time_t m_nCurrTime;//秒
	tm m_tm;
#ifdef USETIMEZONE
	static int32_t m_nTimeZone;//时区偏移时间
#endif
};
#endif