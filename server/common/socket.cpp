#include "pch.h"
#ifdef WIN32
#include <WS2tcpip.h>
 #pragma comment(lib,"ws2_32.lib")
#else
#include <string.h>
#endif
#include"socket.h"
#include "errorcode.h"
CSocket::CSocket()
{
	m_szHost[0] = '\0';
	m_nPort = 0;
	m_Socket = INVALID_SOCKET;
	memset(&m_Addr,0,sizeof(m_Addr));
	m_bNonBlocking = false;
	m_nSeesionIndex = 0;
}
CSocket::CSocket(const char *pHost, uint16_t nPort, bool bBlocking/*=true*/)
{
	m_nSeesionIndex = 0;
	Init(pHost,nPort,bBlocking);
}
void CSocket::Init(const char *pHost, uint16_t nPort, bool bBocking)
{
	m_bNonBlocking = bBocking;
	m_nPort = nPort;
	m_Socket = INVALID_SOCKET;
	memset(&m_Addr, 0, sizeof(m_Addr));
	if (pHost != nullptr)
	{
		STRCPY(m_szHost, pHost);
		inet_pton(AF_INET, m_szHost, &(m_Addr.sin_addr.s_addr));
	}
	else
		m_Addr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_Addr.sin_family = AF_INET;
	m_Addr.sin_port = htons(nPort);
}
CSocket::~CSocket()
{
	Close();
}
void CSocket::SetHost(const char *pHost)
{
	//if(NULL == pHost) return;
	STRCPY(m_szHost,pHost);
	m_Addr.sin_family = AF_INET;
	inet_pton(AF_INET, m_szHost, &(m_Addr.sin_addr.s_addr));
}
void CSocket::SetPort(uint16_t nPort)
{
	m_nPort = nPort;
	m_Addr.sin_port = htons(m_nPort);
}
int32_t CSocket::SetOption(int32_t nLevel,int32_t nOptName,const char *pOptVal,int32_t nOptLen)const
{
	//if(NULL == pOptVal) return E_NULLPOINTER;
#ifdef WIN32
	return setsockopt(m_Socket,nLevel,nOptName,pOptVal,nOptLen);
#else
	return setsockopt(m_Socket,nLevel,nOptName,(const void *)pOptVal,nOptLen);
#endif
}
int32_t CSocket::GetOption(int32_t nLevel,int32_t nOptName, char *pOptVal,int32_t *nOptLen)const
{
	//if(NULL == pOptVal || NULL == nOptLen) return E_NULLPOINTER;
#ifdef WIN32
	return getsockopt(m_Socket,nLevel,nOptName,pOptVal,nOptLen);
#else
	return getsockopt(m_Socket,nLevel,nOptName,(void *)pOptVal,(socklen_t *)nOptLen);
#endif
}
int32_t CSocket::CreateSocket()
{
	m_Socket = socket(AF_INET,SOCK_STREAM,0);
	return INVALID_SOCKET == m_Socket ? E_UNKNOW : S_OK;
}
int32_t CSocket::Listen(int32_t nBacklog/*=1000*/)const
{
	return listen(m_Socket, nBacklog) == SOCKET_ERROR ? E_UNKNOW : S_OK;
}
int32_t CSocket::Bind()
{
	m_Addr.sin_family = AF_INET;
	if (m_szHost[0] != '\0')
		inet_pton(AF_INET, m_szHost, &(m_Addr.sin_addr.s_addr));
	else
		m_Addr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_Addr.sin_port = htons(m_nPort);
	return bind(m_Socket, (const sockaddr *)&m_Addr, sizeof(m_Addr)) == SOCKET_ERROR ? E_UNKNOW : S_OK;
}
int32_t CSocket::Bind(uint16_t nPort)
{
	m_nPort = nPort;
	return Bind();
}
int32_t CSocket::Connect()const
{
	if( connect(m_Socket,(const sockaddr *)&m_Addr,sizeof(m_Addr)) == SOCKET_ERROR)
	{
#ifdef WIN32
		uint32_t _ESIZE = 32;
		char Error[32] = {0};
		INT32 iErr = WSAGetLastError() ;
		switch ( iErr ) 
		{
		case WSANOTINITIALISED : 
			STRNCPY( Error, "WSANOTINITIALISED", _ESIZE ) ;
			break ;
		case WSAENETDOWN : 
			STRNCPY( Error, "WSAENETDOWN", _ESIZE ) ;
			break ;
		case WSAEADDRINUSE : 
			STRNCPY( Error, "WSAEADDRINUSE", _ESIZE ) ;
			break ;
		case WSAEINTR : 
			STRNCPY( Error, "WSAEINTR", _ESIZE ) ;
			break ;
		case WSAEINPROGRESS : 
			STRNCPY( Error, "WSAEINPROGRESS", _ESIZE ) ;
			break ;
		case WSAEALREADY : 
			STRNCPY( Error, "WSAEALREADY", _ESIZE ) ;
			break ;
		case WSAEADDRNOTAVAIL : 
			STRNCPY( Error, "WSAEADDRNOTAVAIL", _ESIZE ) ;
			break ;
		case WSAEAFNOSUPPORT : 
			STRNCPY( Error, "WSAEAFNOSUPPORT", _ESIZE ) ;
			break ;
		case WSAECONNREFUSED : 
			STRNCPY( Error, "WSAECONNREFUSED", _ESIZE ) ;
			break ;
		case WSAEFAULT : 
			STRNCPY( Error, "WSAEFAULT", _ESIZE ) ;
			break ;
		case WSAEINVAL : 
			STRNCPY( Error, "WSAEINVAL", _ESIZE ) ;
			break ;
		case WSAEISCONN : 
			STRNCPY( Error, "WSAEISCONN", _ESIZE ) ;
			break ;
		case WSAENETUNREACH : 
			STRNCPY( Error, "WSAENETUNREACH", _ESIZE ) ;
			break ;
		case WSAENOBUFS : 
			STRNCPY( Error, "WSAENOBUFS", _ESIZE ) ;
			break ;
		case WSAENOTSOCK : 
			STRNCPY( Error, "WSAENOTSOCK", _ESIZE ) ;
			break ;
		case WSAETIMEDOUT : 
			STRNCPY( Error, "WSAETIMEDOUT", _ESIZE ) ;
			break ;
		case WSAEWOULDBLOCK  : 
			STRNCPY( Error, "WSAEWOULDBLOCK", _ESIZE ) ;
			break ;
		default :
			{
				STRNCPY( Error, "UNKNOWN", _ESIZE ) ;
				break ;
			};
		};
		return iErr;
#else
		return errno;
#endif
	}
	return S_OK;
}
int32_t CSocket::Reconnect(const char *pHost/*=NULL*/, uint16_t nPort/*=0*/)
{
	if (pHost) 
		SetHost(pHost);
	if (nPort > 0)
		SetPort(nPort);
	Close();
	if (S_OK != CreateSocket())
		return E_UNKNOW;
	if(!SetNonBlocking(m_bNonBlocking))
		return E_UNKNOW;
	int32_t nRet = Connect();
	if (nRet == S_OK)
		ConnectSucc();
	else if (EINPROGRESS != nRet && WSAEWOULDBLOCK != nRet)
		return nRet;
	return S_OK;
}
SOCKET CSocket::Accept( struct sockaddr_in* addr)const
{
	//if(NULL == addr || NULL == addrlen) return E_NULLPOINTER;
	uint32_t addrlen = sizeof(sockaddr_in);
#ifdef WIN32
	return accept(m_Socket,(sockaddr *)addr,(int*)&addrlen);
#else 
	return accept(m_Socket,(sockaddr *)addr,(socklen_t *)&addrlen);
#endif
}
void CSocket::Close()
{
	if (INVALID_SOCKET != m_Socket)
	{
#ifdef WIN32
		closesocket(m_Socket);
#else
		close(m_Socket);
#endif
		m_Socket = INVALID_SOCKET;
	}
}
int32_t CSocket::Send(const char *pBuf, uint32_t nCount, int32_t nFlag/*=0*/)const
{
	/*if(0 == nCount) return S_OK;
	if(NULL == pBuf) return E_NULLPOINTER;
	if(m_Socket == INVALID_SOCKET) return E_UNKNOW;*/
#ifdef WIN32
	return send(m_Socket,pBuf,nCount,nFlag);
#else
	return send(m_Socket,(const void*)pBuf,nCount,nFlag);
#endif
}
int32_t CSocket::Send(const void *pBuf, uint32_t nCount, int32_t nFlag/*=0*/)const
{
	/*if(0 == nCount) return S_OK;
	if(NULL == pBuf) return E_NULLPOINTER;
	if(m_Socket == INVALID_SOCKET) return E_UNKNOW;*/
#ifdef WIN32
	return send(m_Socket,(const char*)pBuf,nCount,nFlag);
#else
	return send(m_Socket,pBuf,nCount,nFlag);
#endif
}
int32_t CSocket::Recv(char *pBuf,uint32_t nCount,int32_t nFlag /*0*/)const
{
	/*if(NULL == pBuf)return E_NULLPOINTER;
	if(m_Socket == INVALID_SOCKET) return E_UNKNOW;*/
#ifdef WIN32
	return recv(m_Socket,pBuf,nCount,nFlag);
#else
	return recv(m_Socket,(void *)pBuf,nCount,nFlag);
#endif
}
int32_t CSocket::Recv(void *pBuf,uint32_t nCount,int32_t nFlag /*0*/)const
{
	/*if(NULL == pBuf)return E_NULLPOINTER;
	if(m_Socket == INVALID_SOCKET) return E_UNKNOW;*/
#ifdef WIN32
	return recv(m_Socket,(char *)pBuf,nCount,nFlag);
#else
	return recv(m_Socket,pBuf,nCount,nFlag);
#endif
}
int32_t CSocket::SetSendBufSize(const uint32_t nSize)const
{
	return SetOption(SOL_SOCKET,SO_SNDBUF,(const char *)&nSize,sizeof(uint32_t));
}
int32_t CSocket::GetSendBufSize()const
{
	int32_t nOptVal = 0;
	uint32_t nSize = sizeof(int32_t);
	if( GetOption(SOL_SOCKET,SO_SNDBUF,(char *)&nOptVal,(int32_t *)&nSize) == SOCKET_ERROR)
		nOptVal = -1;
	return nOptVal;
}

int32_t CSocket::SetRecvBufSize(const uint32_t nSize)const
{
	return SetOption(SOL_SOCKET,SO_RCVBUF,(const char*)&nSize,sizeof(nSize));
}
int32_t CSocket::GetRecvBufSize()const
{
	int32_t nOptVal = 0;
	uint32_t nSize = sizeof(int32_t);
	if (GetOption(SOL_SOCKET,SO_RCVBUF,(char *)&nOptVal,(int32_t*)&nSize) == SOCKET_ERROR)
		nOptVal = -1;
	return nOptVal;
}
bool CSocket::IsNonBlocking ()const 
{
#ifndef	WIN32
	int32_t nRet= fcntl(m_Socket,F_GETFL,0);
	if(nRet < 0) return false;
	return nRet & O_NONBLOCK;
#endif
	return true;
}
bool CSocket::SetNonBlocking (bool on /*= TRUE*/) 
{
	m_bNonBlocking = on;
#ifdef WIN32
	uint32_t nArg = (on == true?1:0);
	return ioctlsocket(m_Socket, FIONBIO, (unsigned long*)&nArg) == 0;
#else
	int32_t nRet = fcntl(m_Socket,F_GETFL,0);
	if(nRet < 0) 
		return false;
	if (on) 
		nRet |= O_NONBLOCK;
	else 
		nRet &= ~O_NONBLOCK;
	return fcntl(m_Socket, F_SETFL, nRet) >= 0;
#endif
}
uint32_t CSocket::Available ()const
{
	uint32_t nArg = 0;
#ifdef WIN32
	ioctlsocket(m_Socket,FIONREAD,(unsigned long*)&nArg);
#else
	ioctl(m_Socket,FIONREAD,(void *)&nArg);
#endif
	return nArg;
}
uint32_t CSocket::GetLinger ()const 
{
	linger ling = {0};
	uint32_t nLen = sizeof(ling);
	GetOption(SOL_SOCKET,SO_LINGER,(char *)&ling,(int32_t *)&nLen);
	return ling.l_linger;
}
bool CSocket::SetLinger (uint32_t lingerTime) const
{
	linger ling = {0};
	ling.l_onoff = (lingerTime > 0 ? 1:0);
	ling.l_linger = lingerTime;//关闭连接的时候等待发送数据的时间
	return SetOption(SOL_SOCKET, SO_LINGER, (char *)&ling, sizeof(ling)) != SOCKET_ERROR;
}
bool CSocket::IsReuseAddr ()const 
{
	int32_t Result = 0;
	uint32_t nLen = sizeof(int32_t);
	GetOption(SOL_SOCKET,SO_REUSEADDR,(char *)&Result,(int32_t *)&nLen);
	return (Result == 1);
}
bool CSocket::SetReuseAddr (bool on /*= TRUE*/) const
{
#ifndef WIN32//window设置了会重复绑定端口 Linux中不会
	int32_t nOptVal = (on ? 1 : 0);
	return S_OK == SetOption(SOL_SOCKET, SO_REUSEADDR, (const char *)&nOptVal, sizeof(nOptVal));
#endif
	return true;
}
bool CSocket::IsSockError()const 
{
	int32_t nError = 0;
	uint32_t nLen = sizeof(int32_t);
	if(GetOption(SOL_SOCKET,SO_ERROR,(char *)&nError,(int32_t *)&nLen) != 0)
		return true;
	return 0 != nError;
}
void CSocket::SetAddAndSocket(sockaddr_in &add,const SOCKET socket)
{
	m_Socket = socket;
	memcpy(&m_Addr,&add,sizeof(sockaddr_in));
	m_nPort = add.sin_port;
	inet_ntop( AF_INET,static_cast<void*>(&add),m_szHost,MaxHostName);
}
int32_t CSocket::GetSocketError()const
{
	int32_t nError = 0;
	uint32_t nLen = sizeof(int32_t);
	GetOption(SOL_SOCKET,SO_ERROR,(char *)&nError,(int32_t *)&nLen);
	return nError;
}
bool CSocket::SetNagle(bool on /*= true*/)
{
	int32_t nOpt = on ? 1 : 0;  
	return SetOption(IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&nOpt), sizeof(nOpt)) != SOCKET_ERROR;
}
bool CSocket::HasError()const
{
#ifdef WIN32
	return WSAGetLastError() != WSAEWOULDBLOCK;
#else
	return errno != EAGAIN;
#endif
}