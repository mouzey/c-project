#ifndef _H_SOCKET_H
#define _H_SOCKET_H
#include <errno.h> 
#ifdef WIN32
#include <WinSock.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <netinet/tcp.h>
#endif
#include "define.h"
#include "errorcode.h"
#undef SetPort
class CSocket
{
public:
	CSocket();
	CSocket(const char *pHost, uint16_t nPort,bool bBlocking=true);
	virtual ~CSocket();
public:
	virtual void Init(const char *pHost, uint16_t nPort, bool bBocking);
	void SetHost(const char *pHost);
	void SetPort(uint16_t nPort);
	int32_t SetOption(int32_t nLevel,int32_t nOptName,const char *pOptVal,int32_t nOptLen)const;
	int32_t GetOption(int32_t nLevel,int32_t nOptName, char *pOptVal,int32_t *nOptLen)const;
	int32_t CreateSocket();
	int32_t Listen(int32_t nBacklog=1000)const;
	int32_t Bind();
	int32_t Bind(uint16_t nPort);
	int32_t Connect() const;
	int32_t Reconnect(const char *pHost=NULL, uint16_t nPort=0);
	SOCKET Accept( struct sockaddr_in* addr)const;
	void Close();
	int32_t Send(const char *pBuf, uint32_t nCount, int32_t nFlag=0)const;
	int32_t Send(const void *pBuf, uint32_t nCount, int32_t nFlag=0)const;
	int32_t Recv(char *pBuf,uint32_t nCount,int32_t nFlag=0)const;
	int32_t Recv(void *pBuf,uint32_t nCount,int32_t nFlag=0)const;
	int32_t SetSendBufSize( uint32_t nSize)const;
	int32_t GetSendBufSize()const;
	int32_t SetRecvBufSize( uint32_t nSize)const;
	int32_t GetRecvBufSize()const;
	bool IsNonBlocking ()const ;
	bool SetNonBlocking (bool on = true) ;
	//得到缓冲区里有多少字节要被读取
	uint32_t Available ()const;
	uint32_t GetLinger ()const ;
	bool SetLinger (uint32_t lingerTime)const;
	bool IsReuseAddr ()const ;
	bool SetReuseAddr (bool on = true)const;
	bool IsSockError()const ;
	void SetAddAndSocket(sockaddr_in& add,const SOCKET socket);
	int32_t GetSocketError()const;
	//设置Nag算法小数据延迟发送
	bool SetNagle(bool on = true);
public:
	SOCKET GetSocket()const {return m_Socket;}
	uint32_t GetHostIP()const{return (uint32_t)m_Addr.sin_addr.s_addr;}
	uint16_t GetPort()const{return m_nPort;}
	char *GetHost()const {return (char *)m_szHost;}
	bool IsBlocking()const{return m_bNonBlocking;}
	bool HasError()const;
	SessionID GetSessionIndex()const{return m_nSeesionIndex;}
	virtual void SetSessionIndex(SessionID nIndex) { m_nSeesionIndex = nIndex; }
	//监听套接字
	virtual bool IsListenSocket()const{return false;}
	//被动连接套接字
	virtual bool IsAcceptSocket()const {return false;}
	virtual int32_t EpollIn(){return S_OK;}
	virtual int32_t EpollErr() { Close(); return E_UNKNOW; }
	virtual int32_t EpollOut(){return S_OK;}
	virtual bool Active(uint32_t nowSec)const { return false; }
	virtual int32_t ConnectSucc() { return S_OK; }
	virtual bool AddHeartTimer()const { return false; }
	virtual bool SpiltMsg() { return true; }
	virtual bool SendMsg(NetBuffer arrBuf[2]) { return false; }
	virtual bool IsLogin()const { return false; }
	virtual bool PushErrEvent()const { return false; }
	virtual bool OnAccept() { return true; }//接受连接
	virtual bool CanSendMsg()const { return GetSocket() != INVALID_SOCKET; }
	virtual void ConnectClose() {}
	virtual void SendHeart() {}
protected:
	sockaddr_in m_Addr;
	SOCKET m_Socket;
	uint16_t m_nPort;
	bool m_bNonBlocking;
	char m_szHost[MaxHostName];
	SessionID m_nSeesionIndex;
};
#endif
