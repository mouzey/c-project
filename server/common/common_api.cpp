#include "pch.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#ifdef WIN32
#include<direct.h>
#include <io.h>
#include<windows.h> 
#else
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <sys/syscall.h>
#endif
#include "common_api.h"
#include "errorcode.h"
#include "snprintf.h"
#include "define.h"
int32_t CreateDir(const char *pszDir)
{
	if(unlikely(NULL == pszDir))
		return E_NULLPOINTER;
	int32_t nRet = access(pszDir,F_OK);
	if(S_OK == nRet) return S_OK;
#ifdef WIN32
	nRet = _mkdir(pszDir);
#else
	nRet = mkdir(pszDir,0777);
#endif
	return  nRet;
}
void MySleep(uint32_t nMilliseconds)
{
#ifdef WIN32
	Sleep(nMilliseconds);
#else
	usleep(nMilliseconds * 1000);
#endif // WIN32
}
void MicroSleep(uint32_t nMicrosecond)
{
#ifdef WIN32
	timeval tm;
	tm.tv_sec = 0;
	tm.tv_usec = nMicrosecond;
	fd_set fds;
	FD_ZERO(&fds);
	auto fd = socket(AF_INET, SOCK_DGRAM, 0);
	FD_SET(fd, &fds);
	select(0, nullptr, nullptr, &fds, &tm);//windows中fds不能去掉并且不能为空 Linux中可以去掉该参数
	closesocket(fd);
#else
	usleep(nMicrosecond);
#endif
	
}
void InitWinSocket()
{
#ifdef WIN32
	WORD wVersionRequested = MAKEWORD( 2, 2 );
	WSADATA wsaData = {0};
	WSAStartup( wVersionRequested, &wsaData );
#endif
}
int32_t GetTimeZone()
{
	time_t now = time(NULL);
	tm p1 ={0};
	tm p2 = {0};
#ifdef WIN32
	localtime_s(&p1,&now);
	gmtime_s(&p2,&now);
#else
	localtime_r(&now,&p1);
	gmtime_r(&now,&p2);
#endif
	int32_t val = p1.tm_hour-p2.tm_hour;
	if(val > 12)
		return val-24;
	else if(val < -12)
		return val+24;
	return val;
}
void PrintVersion()
{
	const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	char temp[] = __DATE__;
	int32_t year = atoi(temp + 7);
	*(temp + 6) = 0;
	int32_t day = atoi(temp + 4);
	*(temp + 3) = 0;
	int32_t  month = 0;
	for (int32_t i = 0; i < 12; i++)
	{
		if (!strcmp(temp, months[i]))
		{
			month = i + 1;
			break;
		}
	}
	printf("Version build Date: %04d-%02d-%02d %s",year, month, day, __TIME__);
}
void InitDaemon()
{
#ifndef WIN32
	pid_t pid = 0;
	if ((pid = fork()) != 0)
		exit(0);
	setsid();
	if ((pid = fork()) != 0)
		exit(0);
	umask(0);
#endif
}
int32_t CreateServerDir(const char *pszSeverName)
{
	int32_t nRet = CreateDir("../log");
	if(S_OK !=nRet)
		return nRet;
	nRet = CreateDir("../data");
	if(S_OK != nRet)
		return nRet;
	char logpath[MaxFileName];
	SNPRINTF(logpath, MaxFileName,"../log/%s", pszSeverName);
	return CreateDir(logpath);
}
/*
uint32_t Lock(ShareMemoryStatus& flag,ShareMemoryStatus type)
{
	uint32_t nSleepTimes = 0;
	while (true)
	{
		++nSleepTimes;
		if (nSleepTimes > 5)
			return E_UNKNOW;
		if(flag==enmShareMemoryStatus_Free)
		{
			flag=type;
			if(flag!=type)
			{
				MySleep(20);
				continue;
			}
			return S_OK;
		}
		else
			MySleep(20);
	}
	return S_OK;
}
int32_t Unlock(ShareMemoryStatus& flag,ShareMemoryStatus type)
{
	uint32_t nSleepTimes = 0;
	while (true)
	{
		if(flag==enmShareMemoryStatus_Free)
			return S_OK;
		++nSleepTimes;
		if (nSleepTimes >5)
			return E_UNKNOW;
		flag=enmShareMemoryStatus_Free;
		if(flag!=enmShareMemoryStatus_Free)
		{
			MySleep(20);
			continue;
		}
		return S_OK;
	}
	return S_OK;
}*/
int64_t	atoi64(const char* pszNum)
{
	int64_t I = 0;
	int64_t signal = 1;
	if ('-' == *pszNum)
	{
		signal = -1;
		++pszNum;
	}
	while(nullptr != pszNum)
	{
		if ('0' <= *pszNum && '9' >= *pszNum)
		{
			I *= 10;
			I = I + *pszNum - '0';
			++pszNum;
		}
		else
			break;
	}
	return I * signal; 
}
int32_t Random(int32_t nRange)
{
	if (0 == nRange)
		return 0;
	return (static_cast<int32_t>(static_cast<double>(rand()) / RAND_MAX * nRange)) % nRange;
}
//获取一个随机数
int32_t Random2(int32_t nRange)
{
	if (0 == nRange)
		return 0;
	int32_t rand1 = Random(256);
	int32_t rand2 = Random(256);
	int32_t rand3 = Random(256);
	int32_t rand4 = Random(128);
	int32_t number = (rand4 << 24) | (rand3 << 16) | (rand2 << 8) | rand1;
	return (static_cast<int32_t>(static_cast<double>(number) / 0x7FFFFFFF * nRange)) % nRange;
}
uint32_t randomNumberSeed() 
{  	
#ifdef WIN32
	uint32_t pid = GetCurrentThreadId();
	SYSTEMTIME sys = {0};
	GetLocalTime(&sys) ;
	uint32_t sec = static_cast<uint32_t>(time(NULL));
	uint32_t millsec = sys.wMilliseconds*1000;
#else
	timeval tm = {0};
	gettimeofday(&tm,NULL);
	uint32_t sec = tm.tv_sec;
	uint32_t millsec = tm.tv_usec;
	uint32_t pid = getpid();
#endif
	const uint32_t kPrime1 = 61631;  
	const uint32_t kPrime2 = 64997;  
	const uint32_t kPrime3 = 111857;  
	return kPrime1 * pid + kPrime2 * sec + kPrime3 * millsec;  
}  
FILE *Fopen( const char * szFilename,const char * szMode)
{
#ifdef WIN32
	FILE *temp;
	fopen_s(&temp,szFilename,szMode);
	return temp;
#else
	return fopen(szFilename,szMode);
#endif
}
std::string ToStr(uint8_t nValue)
{
	char pStr[4] = {0};
	SNPRINTF(pStr,4,"%hhu",nValue);
	return std::string(pStr);
}
std::string ToStr(uint16_t nValue)
{
	char pStr[6] = {0};
	SNPRINTF(pStr,6,"%hu",nValue);
	return std::string(pStr);
}
std::string ToStr(uint32_t nValue)
{
	char pStr[11] = {0};
	SNPRINTF(pStr,11,"%u",nValue);
	return std::string(pStr);
}
std::string ToStr(uint64_t nValue)
{
	char pStr[21] = {0};
#ifdef _64_PLATFORM_
	SNPRINTF(pStr,21,"%lu",nValue);
#else
	SNPRINTF(pStr,21,"%llu",nValue);
#endif
	return std::string(pStr);
}
std::string ToStr(int8_t nValue)
{
	char pStr[5] = {0};
	SNPRINTF(pStr,5,"%hhd",nValue);
	return std::string(pStr);
}
std::string ToStr(int16_t nValue)
{
	char pStr[7] = {0};
	SNPRINTF(pStr,7,"%hd",nValue);
	return std::string(pStr);
}
std::string ToStr(int32_t nValue)
{
	char pStr[12] = {0};
	SNPRINTF(pStr,12,"%d",nValue);
	return std::string(pStr);
}
std::string ToStr(int64_t nValue)
{
	char pStr[22] = {0};
#ifdef _64_PLATFORM_
	SNPRINTF(pStr,22,"%ld",nValue);
#else
	SNPRINTF(pStr,22,"%lld",nValue);
#endif
	return std::string(pStr);
}
std::string ToStr(const std::string& nValue)
{
	return nValue;
}
std::string ToStr(std::string&& nValue)
{
	return nValue;
}
int32_t ToStr(uint8_t nValue,char* pBuf)
{
	return SNPRINTF(pBuf,4,"%hhu",nValue);
}
int32_t ToStr(uint16_t nValue ,char* pBuf)
{
	return SNPRINTF(pBuf,6,"%hu",nValue);
}
int32_t ToStr(uint32_t nValue,char* pBuf)
{
	return SNPRINTF(pBuf,11,"%u",nValue);
}
int32_t ToStr(uint64_t nValue ,char* pBuf)
{
#ifdef _64_PLATFORM_
	return SNPRINTF(pBuf,21,"%lu",nValue);
#else
	return SNPRINTF(pBuf,21,"%llu",nValue);
#endif
}
int32_t ToStr(int8_t nValue ,char* pBuf)
{
	return SNPRINTF(pBuf,5,"%hhd",nValue);
}
int32_t ToStr(int16_t nValue ,char* pBuf)
{
	return SNPRINTF(pBuf,7,"%hd",nValue);
}
int32_t ToStr(int32_t nValue ,char* pBuf)
{
	return SNPRINTF(pBuf,12,"%d",nValue);
}
int32_t ToStr(int64_t nValue ,char* pBuf)
{
#ifdef _64_PLATFORM_
	return SNPRINTF(pBuf,22,"%ld",nValue);
#else
	return SNPRINTF(pBuf,22,"%lld",nValue);
#endif
}
template <typename T>
T ToInt(const std::string &str)
{
	return ToInt<T>(str);
}
template <>
uint8_t ToInt<uint8_t>(const std::string &str)
{
	return static_cast<uint8_t>(atoi(str.data()));
}
template <>
uint16_t ToInt<uint16_t>(const std::string &str)
{
	return static_cast<uint16_t>(atoi(str.data()));
}
template <>
uint32_t ToInt<uint32_t>(const std::string &str)
{
	return static_cast<uint32_t>(strtoul(str.data(),nullptr,0));
}
template <>
uint64_t ToInt<uint64_t>(const std::string &str)
{
#ifdef _64_PLATFORM_
	return strtoul(str.data(),nullptr,0);
#else
	return STRTOULL(str.data(),nullptr,0);
#endif // _64_PLATFORM_
}
template <>
int8_t ToInt<int8_t>(const std::string &str)
{
	return static_cast<int8_t>(atoi(str.data()));
}
template <>
int16_t ToInt<int16_t>(const std::string &str)
{
	return static_cast<int16_t>(atoi(str.data()));
}
template <>
int32_t ToInt<int32_t>(const std::string &str)
{
	return atoi(str.data());
}
template <>
int64_t ToInt<int64_t>(const std::string &str)
{
#ifdef _64_PLATFORM_
	return strtol(str.data(),nullptr,0);
#else
	return STRTOLL(str.data(),nullptr,0);
#endif // _64_PLATFORM_
}
template <typename T>
T ToInt(const char* pSt)
{
	return ToInt<T>(pSt);
}
template <>
uint8_t ToInt<uint8_t>(const char* pSt)
{
	return static_cast<uint8_t>(atoi(pSt));
}
template <>
uint16_t ToInt<uint16_t>(const char* pSt)
{
	return static_cast<uint16_t>(atoi(pSt));
}
template <>
uint32_t ToInt<uint32_t>(const char* pSt)
{
	return static_cast<uint32_t>(strtoul(pSt,nullptr,0));
}
template <>
uint64_t ToInt<uint64_t>(const char* pSt)
{
#ifdef _64_PLATFORM_
	return strtoul(pSt,nullptr,0);
#else
	return STRTOULL(pSt,nullptr,0);
#endif // _64_PLATFORM_
}
template <>
int8_t ToInt<int8_t>(const char* pSt)
{
	return static_cast<int8_t>(atoi(pSt));
}
template <>
int16_t ToInt<int16_t>(const char* pSt)
{
	return static_cast<int16_t>(atoi(pSt));
}
template <>
int32_t ToInt<int32_t>(const char* pSt)
{
	return atoi(pSt);
}
template <>
int64_t ToInt<int64_t>(const char* pSt)
{
#ifdef _64_PLATFORM_
	return strtol(pSt,nullptr,0);
#else
	return STRTOLL(pSt,nullptr,0);
#endif // _64_PLATFORM_
}
int32_t Avg(int32_t nValue1,int32_t nValue2)
{
	return(nValue1&nValue2)+((nValue1^nValue2)>>1);
}
uint32_t Avg(uint32_t nValue1,uint32_t nValue2)
{
	return(nValue1&nValue2)+((nValue1^nValue2)>>1);
}
uint32_t Ceil(uint32_t nValue1,uint32_t nValue2)
{
	if(0 == nValue1)
		return 0;
	return((nValue1-1)/nValue2+1);
}
template <typename T>
T ToInt(const std::string &&str)
{
	return ToInt<T>(str);
}
template <>
uint8_t ToInt<uint8_t>(const std::string &&str)
{
	return static_cast<uint8_t>(atoi(str.data()));
}
template <>
uint16_t ToInt<uint16_t>(const std::string &&str)
{
	return static_cast<uint16_t>(atoi(str.data()));
}
template <>
uint32_t ToInt<uint32_t>(const std::string &&str)
{
	return static_cast<uint32_t>(strtoul(str.data(),nullptr,0));
}
template <>
uint64_t ToInt<uint64_t>(const std::string &&str)
{
#ifdef _64_PLATFORM_
	return strtoul(str.data(),nullptr,0);
#else
	return STRTOULL(str.data(),nullptr,0);
#endif // _64_PLATFORM_
}
template <>
int8_t ToInt<int8_t>(const std::string &&str)
{
	return static_cast<int8_t>(atoi(str.data()));
}
template <>
int16_t ToInt<int16_t>(const std::string &&str)
{
	return static_cast<int16_t>(atoi(str.data()));
}
template <>
int32_t ToInt<int32_t>(const std::string &&str)
{
	return atoi(str.data());
}
template <>
int64_t ToInt<int64_t>(const std::string &&str)
{
#ifdef _64_PLATFORM_
	return strtol(str.data(),nullptr,0);
#else
	return STRTOLL(str.data(),nullptr,0);
#endif // _64_PLATFORM_
}
int32_t ToStr(const uint8_t nValue,char* pBuf,const uint32_t nCount)
{
	return SNPRINTF(pBuf,nCount,"%hhu",nValue);
}
int32_t ToStr(const uint16_t nValue ,char* pBuf,const uint32_t nCount)
{
	return SNPRINTF(pBuf,nCount,"%hu",nValue);
}
int32_t ToStr(const uint32_t nValue,char* pBuf,const uint32_t nCount)
{
	return SNPRINTF(pBuf,nCount,"%u",nValue);
}
int32_t ToStr(const uint64_t nValue ,char* pBuf,const uint32_t nCount)
{
#ifdef _64_PLATFORM_
	return SNPRINTF(pBuf,nCount,"%lu",nValue);
#else
	return SNPRINTF(pBuf,nCount,"%llu",nValue);
#endif
}
int32_t ToStr(const int8_t nValue ,char* pBuf,const uint32_t nCount)
{
	return SNPRINTF(pBuf,nCount,"%hhd",nValue);
}
int32_t ToStr(const int16_t nValue ,char* pBuf,const uint32_t nCount)
{
	return SNPRINTF(pBuf,nCount,"%hd",nValue);
}
int32_t ToStr(const int32_t nValue ,char* pBuf,const uint32_t nCount)
{
	return SNPRINTF(pBuf,nCount,"%d",nValue);
}
int32_t ToStr(const int64_t nValue ,char* pBuf,const uint32_t nCount)
{
#ifdef _64_PLATFORM_
	return SNPRINTF(pBuf,nCount,"%ld",nValue);
#else
	return SNPRINTF(pBuf,nCount,"%lld",nValue);
#endif
}
void ToStr(uint8_t nValue,std::string &str)
{
	char pStr[4] = {0};
	SNPRINTF(pStr,4,"%hhu",nValue);
	str.append(pStr);
}
void ToStr(uint16_t nValue,std::string &str)
{
	char pStr[6] = {0};
	SNPRINTF(pStr,6,"%hu",nValue);
	str.append(pStr);
}
void ToStr(uint32_t nValue,std::string &str)
{
	char pStr[11] = {0};
	SNPRINTF(pStr,11,"%u",nValue);
	str.append(pStr);
}
void ToStr(uint64_t nValue,std::string &str)
{
	char pStr[21] = {0};
#ifdef _64_PLATFORM_
	SNPRINTF(pStr,21,"%lu",nValue);
#else
	SNPRINTF(pStr,21,"%llu",nValue);
#endif
	str.append(pStr);
}
void ToStr(int8_t nValue,std::string &str)
{
	char pStr[5] = {0};
	SNPRINTF(pStr,5,"%hhd",nValue);
	str.append(pStr);
}
void ToStr(int16_t nValue,std::string &str)
{
	char pStr[7] = {0};
	SNPRINTF(pStr,7,"%hd",nValue);
	str.append(pStr);
}
void ToStr(int32_t nValue,std::string &str)
{
	char pStr[12] = {0};
	SNPRINTF(pStr,12,"%d",nValue);
	str.append(pStr);
}
void ToStr(int64_t nValue,std::string &str)
{
	char pStr[22] = {0};
#ifdef _64_PLATFORM_
	SNPRINTF(pStr,22,"%ld",nValue);
#else
	SNPRINTF(pStr,22,"%lld",nValue);
#endif
	str.append(pStr);
}
void Trim(std::string& str)
{
	std::string space(" \t\n\v\f\r");
	str.erase(0, str.find_first_not_of(space));
	str.erase(str.find_last_not_of(space)+1);
}
int epoll_create(int size)
{
	//创建句柄 size 大于0就可以了
	return 0;//大于0创建成功返回的句柄值 -1表示创建失败
}
int epoll_wait(int epfd, struct epoll_event *events,int maxevents, int timeout)
{
	//epfd为epoll_create创建的句柄
	//events返回的事件数组
	//maxevents 为events数组的大小
	//timeout最长等待的时间
	/************************************************************************/
	/* epoll ET模式中事件触发条件
	epollin 监听socket有客户端连接的时候，其他socket在有数据可以读取的时候
	或者对端正常关闭socket或者本端socket发生错误的时候
	epollout 缓冲区从满到不满的时候，epollin事件触发的时候，epoll_ctl注册或者修改注册中
	有epollout事件并且当前缓冲区不满则立马触发否者等到缓冲区变为不满在触发，连接成功后会触发
	epollrdhup事件在对方正常关闭连接的时候触发 epollerr和epollhup在本端socket发生错误的时候
	触发。处理错误socket只需要处理epollrdhup epollerr epollhup就可以了。
	判断连接成功通过connect函数返回0表示连接成功非阻塞socket可能不会立马连接成功则需要判断返回值
	，返回值windows中为WSAEWOULDBLOCK，linux中为 EINPROGRESS则表示正在连接，加入epollout事件检查，当
	触发epollout事件表示连接成功。直接拔掉网线不会触发任何事件，所以socket都需要心跳检查，内核底层
	也有socket保活机制但是时间比较长，所以一般都是通过心跳或者通过长时间没有接收或者发送数据就
	关闭socket来处理
	EPOLLRDHUP 可能会导致最后一点数据无法收到 比如http请求的时候connection设置为close这样http
	服务器会在处理后发送返回协议立马关闭socket这样epoll会立马检测到EPOLLRDHUP事件，如果直接
	关闭socket就无法收到http返回的数据 所以判断socket错误还是使用EPOLLIN和EPOLLOUT事件然后
	检查recv或者send的返回值来判断是否有错误
	*/
	/************************************************************************/
	return 0;//-1表示失败等于0表示超时大于0表示返回events数组的大小，就是准备好事件的数量
}
int epoll_ctl(int epfd, int op, int64_t fd, struct epoll_event *event)
{
	//epfd为epoll_create创建的句柄
	//op 操作类型 EPOLL_CTL_ADD添加事件 EPOLL_CTL_MOD修改事件 EPOLL_CTL_DEL删除事件
	//当op为EPOLL_CTL_DEL时event可以为空指针
	//fd需要操作的socket句柄
	//event为事件数据data域为自定义数据events表示需要处理的事件，常用事件枚举值有：
	//EPOLLIN输入事件 EPOLLOUT 输出事件 EPOLLRDHUP挂起事件当为连接socket时对方断开
	//连接会触发的事件 EPOLLERR本端socket发生错误事件默认监听事件不用注册，
	//EPOLLHUP本端socket挂起事件默认监听事件不用注册 EPOLLET边缘触发事件默认为水平出发
	//监听socket一般注册 EPOLLIN和EPOLLET，连接socket一般注册EPOLLIN，EPOLLOUT，EPOLLRDHUP，
	//EPOLLET.但是监听和连接socket都的处理EPOLLERR和EPOLLHUP事件
	return 0;//0表示成功-1表示失败
}
uint32_t GetThreadId()
{
#ifndef WIN32
	return syscall(SYS_gettid);
#else
	return GetCurrentThreadId();
#endif
}
uint32_t GetProcessId()
{
#ifndef WIN32
	return getpid();
#else
	return GetCurrentProcessId();
#endif
}
void KillSelf()
{
#ifdef WIN32
	abort();
#else
	kill(GetProcessId(),SIGUSR2);
#endif // WIN32

}
void IgnoreSignal()
{
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
#ifndef WIN32
	signal(SIGHUP, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGKILL, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
#endif
}
void RegistSignal(void (*handle)(int))
{
#ifndef WIN32
	signal(SIGUSR1, handle);
	signal(SIGUSR2, handle);
#endif
}
bool CpySql(char* out, uint32_t max, const char* in, uint32_t inSize, uint32_t& outSize)
{
	if (inSize > max)return true;
	memcpy(out, in, inSize);
	outSize += inSize;
	return false;
}
std::string GetDir(const char* pSrc)
{
	std::string str(pSrc);
#ifdef WIN32
	return str.substr(0, str.rfind("\\")+1);
#else
	return str.substr(0, str.rfind("/")+1);
#endif // WIN32
}
std::string GetServerName(const char* pSrc)
{
	std::string str(pSrc);
#ifdef WIN32
	auto begin = str.rfind("\\");
	auto end = str.rfind(".");
	return str.substr(begin+1, end-begin-1);
#else
	return str.substr(str.rfind("/")+1);
#endif // WIN32
}
bool CommonInit(int argc, char* argv[])
{
	if (argc > 1 && 0 == stricmp(static_cast<const char*>(argv[1]), "-v"))
	{
		PrintVersion();
		return false;
	}
#ifndef WIN32
	if (argc > 1 && 0 == stricmp(static_cast<const char*>(argv[1]), "-d"))//设置后台进程
		if (0 != daemon(1, 0))//不改变进程根目录和标准输入输出错误输出
			return false;
#endif
	InitWinSocket();
	IgnoreSignal();
	return true;
}
void Base64Encode(const char* d, uint32_t len, std::string& ret)
{
	const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	auto old = ret.size();
	ret.resize(ret.size() + (len + 2) / 3 * size_t( 4));
	char* out = &ret[old];
	int chunk = 0;
	int padlen = 0;
	uint32_t i = 0;
	while (i < len)
	{
		chunk = int(uint8_t(d[i++])) << 16;
		if (i == len)
			padlen = 2;
		else
		{
			chunk |= int(uint8_t(d[i++])) << 8;
			if (i == len)
				padlen = 1;
			else
				chunk |= int(uint8_t(d[i++]));
		}
		*out++ = alphabet[(chunk & 0x00fc0000) >> 1];
		*out++ = alphabet[(chunk & 0x0003f000) >> 12];
		*out++ = padlen > 1 ? '=' : alphabet[(chunk & 0x00000fc0) >> 6];
		*out++ = padlen > 0 ? '=' : alphabet[(chunk & 0x0000003f)];
	}
}
unsigned char ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48;
}
std::string UrlEncode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ')
			strTemp += "+";
		else
		{
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] & 0xF);
		}
	}
	return strTemp;
}
#ifdef WIN32
void SSL_library_init() {}
void SSL_load_error_strings() {}
const SSL_METHOD* SSLv23_client_method() { return nullptr; }
const SSL_METHOD* SSLv23_server_method() { return nullptr; }
const SSL_METHOD* SSLv23_method() { return nullptr; }
SSL_CTX* SSL_CTX_new(const SSL_METHOD*) { return nullptr; }
void SSL_CTX_set_verify(SSL_CTX*, int, int*) {}
int SSL_CTX_use_certificate_file(SSL_CTX*, const char*, int) { return 1; }//返回1表示成功
int SSL_CTX_use_certificate_chain_file(SSL_CTX* ctx, const char* file) { return 1; }//加载证书链
int SSL_CTX_use_PrivateKey_file(SSL_CTX*, const char*, int) { return 1; }//返回1表示成功
int SSL_CTX_check_private_key(SSL_CTX*) { return 1; }//返回1表示成功
int SSL_do_handshake(SSL*) { return 1; }//返回1表示成功
int SSL_read(SSL* ssl, void* buf, int num) { return 0; }
int SSL_write(SSL* ssl, const void* buf, int num) { return 0; }
int SSL_get_error(const SSL* ssl, int ret) { return 0; }
int SSL_set_fd(SSL*, int) { return 1; }//返回1表示成功
void SSL_set_connect_state(SSL* ssl) {}
void SSL_set_accept_state(SSL* ssl) {}
SSL* SSL_new(SSL_CTX*) { return nullptr; }
void SSL_CTX_free(SSL_CTX*) {}
void ERR_free_strings() {}
void SSL_free(SSL*) {}
void SSL_shutdown(SSL*) {}
#endif