#ifndef _H_DEFINE_H
#define _H_DEFINE_H
#include <cstdint>
#include <memory>
#define SHORTSQLSIZE 4*1024
#define SQLMAXSIZE 128*1024
#define MaxFileName 128
#define MaxOneLogSize 4096
#define MaxLogSize 1024*1024*50 //50M
#define DEFUALTLOGNAME "./log/log"
#define MaxHostName 24
#define MaxOneMsg 128*1024
#define MaxEpollSocket 10000
#define MaxOnceAccept 50
#define MaxClient 1
#define FULSHLOGTIME	1000
#define MAXREDISCOMMAND	256
#define MAXREDISRESULT	1024
#define MAXSOCKETCLOSETIME 300
#define MAXSOCKEHEART 60
#define TIMERBASE 1000// 1000表示定时器精度为毫秒 1表示定时器精度为微妙
#define PACKHEADSIZE 6
#define PROCESS_LOCK_NAME "frame"
#define SNPRINTF  CSnprintf::Snprintf//snprintf for linux and sprintf_s for windows
#define VSNPRINTF CSnprintf::Snprintf//vsnprintf
#ifndef NULL
#define NULL 0
#endif
#if defined(_M_X64) || defined(__x86_64__)
#define _64_PLATFORM_ //64位平台
#ifndef PRIu64
#define PRIu64 "lu"
#define PRIi64 "ld"
#define PRIX64 "lx"
#define PRIx64 "lX"
#endif
#else
#ifndef PRIu64
#define PRIu64 "llu"
#define PRIi64 "lld"
#define PRIX64 "llx"
#define PRIx64 "llX"
#endif
#endif
#define	ASSERT_FRAME assert
#ifdef WIN32
struct SSL {};
struct SSL_CTX {};
#else
#include <openssl/ossl_typ.h>
#endif
enum  ThreadStatus : uint8_t
{
	enmThreadStatus_Ready ,		// 当前线程处于准备状态
	enmThreadStatus_Runing ,	// 处于运行状态
	enmThreadStatus_Exiting ,	// 线程等待退出
	enmThreadStatus_Exit		// 已经退出 
};
enum RedisRetType : uint8_t
{
	enmRedisRetType_Non,
	enmRedisRetType_Error,
	enmRedisRetType_Status,
	enmRedisRetType_Int,
	enmRedisRetType_Str,
	enmRedisRetType_Arr,
};
enum ShareMemoryStatus:uint8_t
{
	enmShareMemoryStatus_Free = 0x00,//共享内存空闲
	enmShareMemoryStatus_C_READ=0x01,//共享内存自己读取
	enmShareMemoryStatus_C_WRITE=0x02,//共享内存自己写
	enmShareMemoryStatus_S_READ=0x03,//Server读
	enmShareMemoryStatus_S_WRITE=0x04,//Server写
};
enum 
{
	enmMaxRoleNameLength = 32,
};
enum LogLevel : uint8_t
{
	enmLogLevel_Debug,
	enmLogLevel_Info,
	enmLogLevel_Waring,
	enmLogLevel_Error,
	enmLogLevel_Crisis,
};
enum ClientType : uint8_t
{
	enmClientType_User		= 0x00,//普通客户端
	enmClientType_Http		= 0x01,//http客户端
	enmClientType_Server	= 0x02,//服务器客户端
	enmClientType_Proxy		= 0x03,//代理客户端
	enmClientType_Cross		= 0x04,//跨服客户端
};
enum ThreadType:uint8_t
{
	enmThreadType_Main = 0,
	enmThreadType_Sql = 1,
	enmThreadType_Log = 2,
	enmThreadType_Client = 3,
	enmThreadType_Http = 4,
	enmThreadType_Recharge=5,
	enmThreadType_Server = 6,
	enmThreadType_Net = 7,
	enmThreadType_Bill = 8,
};
enum SocketStatus:uint8_t
{
	enmSocketStatus_None,
	enmSocketStatus_Accept,//已经连接
	enmSocketStatus_Register,//服务器之间连接收到了注册包wensocket收到了第一个包
	enmSocketStatus_Connecting,//正在等待连接
	enmSocketStatus_Connected,//连接成功
	enmSocketStatus_SSLConnected,//ssl握手成功
	enmSocketStatus_WSConnected,//ws握手成功
	enmSocketStatus_Login,//客服端连接登陆了游戏
};
enum EventType :uint8_t
{
	enmEventType_Accept,
	enmEventType_Data,
	enmEventType_Remove,
	enmEventType_Connect,
	enmEventType_HttpErr,
};
struct EventData
{
	EventType type;
	void* data;
};
typedef uint32_t RoleID;//角色id
typedef uint16_t ServerID;//服务器id
typedef uint32_t ZoneID;//分区id
typedef uint64_t SessionID;
typedef uint64_t TimerID;
struct NetBuffer
{
	uint32_t len;
	char* buf;
};
enum class TimerType:uint32_t
{
	enmTimerType_None,
	enmTimerType_SqlEvent,
	enmTimerType_SocketAct,
	enmTimerType_SocketHeart,
	enmTimerType_AddSocket,
	enmTimerType_KickSocket,
	enmTimerType_SplitMsg,
	enmTimerType_FunEvent,
	enmTimerType_AddEvent,
	enmTimerType_SendMsg,
	enmTimerType_SocketClose,
};
union FuncData
{
	bool   bflag;
	uint8_t u8;
	int8_t i8;
	uint16_t u16;
	int16_t i16;
	uint32_t u32;
	int32_t i32;
	uint64_t u64;
	int64_t i64;
	uint8_t arrU8[8];
	int8_t  arrI8[8];
	bool  arrBool[8];
	char szStr[8];
	uint16_t arrU16[4];
	int16_t arrI16[4];
	uint32_t arrU32[2];
	int32_t arrI32[2];
	void *ptr;
	FuncData():u64(0){}
	FuncData(uint64_t value):u64(value){}
	FuncData(int64_t value):i64(value){}
	FuncData(void* value):ptr(value){}
};
typedef FuncData TimerData;
class CTimerBase;
struct TimerDataInfo
{
	TimerType type = TimerType::enmTimerType_None;
	TimerID   id = 0;
	uint64_t nCycleTime = 0;
	TimerData data;
	CTimerBase* pHandle = nullptr;
};
class CFuncBase
{
	CFuncBase(){}
	virtual ~CFuncBase(){}
};
struct SharedPtrTag{};
struct WeakPtrTag{};
struct FuncPtrData
{
	FuncData data;
	std::shared_ptr<CFuncBase> pShare;
	std::weak_ptr<CFuncBase> pWeak;
	FuncPtrData(){}
	FuncPtrData(uint64_t value){data.u64 = value;}
	FuncPtrData(void* value){data.ptr = value;}
	FuncPtrData(std::shared_ptr<CFuncBase> ptr,SharedPtrTag){pShare = ptr;}
	FuncPtrData(std::shared_ptr<CFuncBase> ptr,WeakPtrTag){pWeak = ptr;}
};
enum class PkgIndex :uint8_t//加载初始化顺序等需要
{
	enmPkgIndex_CSkill,
	enmPkgIndex_CMgrmsgbodyPkg,
	enmPkgIndex_Max//所有模块枚举定义在这个之前
};
class CPkgBase;
template<class T>
struct PkgIndexVal
{
	static uint8_t index;
	static_assert(std::is_base_of<CPkgBase, T>::value, "T mast base CPkgBase");
};
template<class T>
uint8_t PkgIndexVal<T>::index = 0;
#define REGPKGINDEX(TYPE) PkgIndexVal<TYPE>::index = (uint8_t)PkgIndex::enmPkgIndex_##TYPE;
#ifdef WIN32//for windows
#define REGMGHHANDLE(TYPE) static bool __##TYPE__ = TYPE::Register();
class CSelectThread;
typedef CSelectThread CSocketThread;
#define access  _access_s
#define stricmp _stricmp
#define F_OK 0
typedef union epoll_data {
	void *ptr;
	int fd;
	uint32_t u32;
	uint64_t u64;
} epoll_data_t;//保存触发事件的某个文件描述符相关的数据

struct epoll_event {
	uint32_t events;      /* epoll event */
	epoll_data_t data;      /* User data variable */
};
#define SIGUSR1 0;
#define SIGUSR2 1;
#define EPOLL_CTL_ADD 0
#define EPOLL_CTL_MOD 0
#define EPOLL_CTL_DEL 0
#define EPOLLIN 0
#define EPOLLOUT 0
#define EPOLLRDHUP 0
#define EPOLLERR 0
#define EPOLLHUP 0
#define EPOLLET 0
#define  FARMHASH_NO_BUILTIN_EXPECT
#define CHECK_FORMAT(i, j)
#if _MSC_VER >= 1900 //vs2015
#define THREAD_LOCAL  thread_local
#else
#define THREAD_LOCAL  __declspec(thread)
#endif
#define FORCE_INLINE inline
#define STRCPY strcpy_s
#define STRNCPY strncpy_s
#ifndef likely
#define likely(x) x
#define unlikely(x) x
#endif
#define  SSCANF sscanf_s
#define STRTOLL _strtoi64
#define STRTOULL _strtoui64
#define VSPRINTF vsprintf_s
#define localtime_r(x,y) localtime_s(y,x)
#define  gmtime_r(x,y) gmtime_s(y,x)
#else //for linux
#define REGMGHHANDLE(TYPE) static bool __##TYPE__  __attribute__((unused))= TYPE::Register();
class CEpollThread;
typedef CEpollThread CSocketThread;
typedef int32_t		SOCKET;
#define TRUE true
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define stricmp strcasecmp
#define CHECK_FORMAT(i, j) __attribute__((format(printf, i, j)))
#if defined(__GNUC__) && ((__GNUC__ >= 5) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
#define THREAD_LOCAL thread_local
#else
#define THREAD_LOCAL __thread
#endif
#define FORCE_INLINE __attribute__((always_inline)) inline
#define STRCPY strcpy
#define STRNCPY strncpy
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#define SSCANF sscanf
#define STRTOLL strtoll
#define STRTOULL strtoull
#define VSPRINTF vsprintf
#define  localtime_s(x,y) localtime_r(y,x)
#define  gmtime_s(x,y) gmtime_r(y,x)
#endif //end if
#endif
