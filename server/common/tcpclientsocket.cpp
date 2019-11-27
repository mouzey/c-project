#include "pch.h"
#ifndef WIN32
#include <stdlib.h>
#endif
#include <assert.h>
#include "tcpclientsocket.h"
#include "log.h"
#include "selectthread.h"
#include "epollthread.h"
#include "serverbase.h" 
#include "socketthread.h"
#include "common_api.h"
#ifndef WIN32
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif
CClientSocket::CClientSocket()
{
	m_pRecvBuf = new CNewQueue<char>;
	m_pSendBuf = new CNewQueue<char>;
	m_key = 0;
	m_status = enmSocketStatus_Accept;
}
CClientSocket::CClientSocket(uint32_t recvSize, uint32_t sendSize)
{
	m_pRecvBuf = new CNewQueue<char>(recvSize);
	m_pSendBuf = new CNewQueue<char>(sendSize);
	m_key = 0;
	m_status = enmSocketStatus_Accept;
}
CClientSocket::CClientSocket(const char *pHost,const uint32_t nPort,const bool bBlocking/*=true*/): CSocket(pHost,nPort,bBlocking)
{
	m_pSendBuf = new CNewQueue<char>;
	m_pRecvBuf = new CNewQueue<char>;
	m_key = 0;
	m_status = enmSocketStatus_Accept;
}
CClientSocket::~CClientSocket()
{
	WRITE_INFO_LOG("delete client socket=%d host=%s port=%u",GetSocket(),GetHost(),GetPort());
	if(m_pRecvBuf)
	{
		delete m_pRecvBuf;
		m_pRecvBuf = nullptr;
	}
	if (m_pSendBuf)
	{
		delete m_pSendBuf;
		m_pSendBuf = nullptr;
	}
}
void CClientSocket::Reinit()
{
	m_pRecvBuf->Init();
	m_pSendBuf->Init();
	m_sendMsgList.clear();
	m_status = enmSocketStatus_Accept;
}
int32_t CClientSocket::OpenAsClient(const char *pHost/*=NULL*/,const uint32_t nPort/*=0*/,const bool bNonBlocking/*=true*/)
{
	Close();
	if(pHost) SetHost(pHost);
	if(nPort > 0) SetPort(nPort);
	if(S_OK != CreateSocket())
	{
		WRITE_ERROR_LOG("create socket has error port=%u host=%s",GetPort(),GetHost());
		return E_UNKNOW;
	}
	if(!SetNonBlocking(true))
	{
		WRITE_ERROR_LOG("set nonblocking has error port=%u host=%s", GetPort(), GetHost());
		return E_UNKNOW;
	}
	int32_t nRet = Connect();
	if (nRet == S_OK)
		return	ConnectSucc();
	else if (EINPROGRESS != nRet && WSAEWOULDBLOCK != nRet)
	{
		WRITE_ERROR_LOG("connect has errorn ret=%d port=%u host=%s", nRet, GetPort(), GetHost());
		return nRet;
	}
	else
		SetStatus(enmSocketStatus_Connecting);
	return S_OK;
}
int32_t CClientSocket::SendMessage(const char *pBuf,const uint32_t nSize)
{
	if (NULL == pBuf || 0 == nSize) return S_OK;
	int32_t nRet = m_pSendBuf->AddBuf(pBuf,nSize);
	if (S_OK != nRet)
	{
		WRITE_ERROR_LOG("send msg has error ret=%d",nRet);
	}
	WRITE_DEBUG_LOG("send msg size=%d",nSize);
	return nRet;
}
int32_t CClientSocket::RecvMessage()
{
	static char szBuf[MaxOneMsg] = {0};
	uint32_t nSize = MaxOneMsg;
	int32_t nRet = Recv(szBuf,nSize);
	if (0 > nRet)
	{
		WRITE_ERROR_LOG("recv msg has error ret=%d",nRet);
		return nRet;
	}
	nSize = nRet;
	nRet = m_pRecvBuf->AddOnlyBuf(szBuf,nSize);
	if (S_OK != nRet)
	{
		WRITE_ERROR_LOG("add msg to queue has error ret=%d",nRet);
	}
	if(nSize == MaxOneMsg) szBuf[nSize-1] = '\0';
	WRITE_DEBUG_LOG("recv msg  msg=%s size=%d",szBuf,nSize);
	return nRet;
}
int32_t CClientSocket::RecvAllMsg()
{
	uint32_t nSize = m_pRecvBuf->GetFreeSize();
	if(0 == nSize) return E_LESSMEMORY;
	char *pBuf = new char[nSize];
	if(NULL == pBuf)
		return E_UNKNOW;
	if(NULL == pBuf) return E_NULLPOINTER;
	int32_t nRet = Recv(pBuf,nSize);
	if (0 >= nRet)
	{
		WRITE_ERROR_LOG("recv msg has error ret=%d errno=%d lasterror=%d",nRet,errno,GetSocketError());
		delete [] pBuf;
		pBuf = NULL;
		return nRet;
	}
	if (S_OK != m_pRecvBuf->AddOnlyBuf(pBuf,nRet))
	{
		WRITE_ERROR_LOG("add msg to queue has error ret=%d",nRet);
	}
	
	WRITE_DEBUG_LOG("client real msg=(%.*s)recv  size=%d", nRet,pBuf,nRet);
	//printf("client real recv  size=%d\n",nRet);
	//if(0 == m_RoleID)
	//printf("%d %d %d %d %d\n",pBuf[0],pBuf[1],pBuf[2],pBuf[3],m_RoleID);
	delete [] pBuf;
	pBuf = NULL;
	return nSize == static_cast<uint32_t>(nRet) ? E_LESSMEMORY: nRet;
}
int32_t CClientSocket::Select()
{
	SOCKET fd = GetSocket();
	fd_set fRead;
	FD_ZERO(&fRead);
	FD_SET(fd,&fRead);
	fd_set fWrite;
	FD_ZERO(&fWrite);
	FD_SET(fd,&fWrite);
	fd_set fError;
	FD_ZERO(&fError);
	FD_SET(fd,&fError);
	timeval outtime = {0,0};
	int32_t nRet = select((int)fd+1,&fRead,&fWrite,&fError,&outtime);//in win32 first arg  has no use
	if (0 > nRet)
	{
		WRITE_ERROR_LOG("socket has error");
		return Reconnect();
	}
	if(0 == nRet) return S_OK;
	if (FD_ISSET(fd,&fError)/*||(FD_ISSET(fd,&fRead)&&FD_ISSET(fd,&fWrite))*/)
	{
		WRITE_ERROR_LOG("socket has error fd=%d error=%d errorno=%d nret=%d",fd,GetSocketError(),errno,nRet);
		return Reconnect();
	}
	if (FD_ISSET(fd,&fRead))
	{
		//printf("can read\n");
		WRITE_INFO_LOG("client recv msg");
		RecvAllMsg();
		return S_OK;
	}
	if (FD_ISSET(fd,&fWrite))
	{
		//SendOneMsg();
		//printf("can send\n");
		WRITE_INFO_LOG("client send msg");
		return SendMsg();	
	}
	return S_OK;
}
int32_t CClientSocket::SendMsg()
{
	if(m_pSendBuf->IsEmpty())
	{
		//printf("empty");
		return S_OK;
	}
	char *pBuf = NULL;
	//char szBuf[1024] = {0};
	uint32_t nHead = 0;
	uint32_t nTail = 0;
	m_pSendBuf->GetBuf(&pBuf,nHead,nTail);
	if(NULL == pBuf) return E_NULLPOINTER;
	int32_t n = 0;
	if(nTail > nHead)
	{
		n = Send(pBuf,nTail-nHead);
		//memcpy(szBuf,pBuf,n);
	}
	else
	{
		uint32_t nMax = m_pSendBuf->GetMaxSize();
		n = Send(pBuf,nMax-nHead);
		//memcpy(szBuf,pBuf,n);
		if (static_cast<uint32_t>(n) == (nMax-nHead))
		{
			pBuf = pBuf-nHead;
			int32_t nTemp = Send(pBuf,nTail);
			if (nTemp > 0) 
			{
				n += nTemp;
				//memcpy(&szBuf[n],pBuf,nTemp);
			}
			else WRITE_ERROR_LOG("send msg has error ret=%d",nTemp);
		}
	}
	if(n > 0)
	{
		WRITE_DEBUG_LOG("real send size=%d ",n);
		//printf("real send msg=%s size=%d\n",szBuf,n);
		m_pSendBuf->Remove(n);
	}
	else 
	{
		//printf("error");
		WRITE_ERROR_LOG("send msg has errors ret=%d errno=%d lasterror=%d",n,errno,GetSocketError());
	}
	pBuf = NULL;
	return n;
}
int32_t CClientSocket::AttachOneMsg(char *pBuf,uint32_t &nSize,const uint32_t nMaxBufSize)
{
	if(NULL == pBuf) return E_NULLPOINTER;
	return m_pRecvBuf->GetBuf(pBuf,nSize,nMaxBufSize);
	//WRITE_DEBUG_LOG("attach msg=%s size=%d",pBuf,nSize);
}
int32_t CClientSocket::SendOneMsg()
{
	if(m_pSendBuf->IsEmpty()) return S_OK;
	static char szBuf[MaxOneMsg] = {0};
	uint32_t nSize = 0;
	m_pSendBuf->GetBuf(szBuf,nSize,MaxOneMsg);
	int32_t n = Send(szBuf,nSize);
	if(MaxOneMsg == nSize) szBuf[MaxOneMsg-1] = '\0';
	WRITE_DEBUG_LOG("real sen msg to other msg=%s size=%d",szBuf,n);
	return  S_OK;
}
int32_t CClientSocket::EpollOut()
{
	if(unlikely(IsWaitConnect()))
		ConnectSucc();
	if(m_pSendBuf->IsEmpty()) return S_OK;
	NetBuffer arrBuf[2];
	m_pSendBuf->GetReadEnable(arrBuf);
	int32_t nRet = Send(arrBuf[0].buf,arrBuf[0].len);
	if(nRet <= 0)
	{
		if(HasError())
		{
			WRITE_ERROR_LOG("send msg has errors ret=%d errno=%d lasterror=%d host=%s port=%u key=%" PRIu64 " session=%" PRIu64,
				nRet,errno,GetSocketError(),GetHost(),GetPort(),GetKey(),GetSessionIndex());
			return E_UNKNOW;
		}
		return S_OK;
	}
	if(arrBuf[1].len > 0 && static_cast<uint32_t>(nRet) == arrBuf[0].len)
	{
		int32_t nTemp = Send(arrBuf[1].buf,arrBuf[1].len);
		if(nTemp > 0)
			nRet += nTemp;
		else if (HasError())
		{
			WRITE_ERROR_LOG("send msg has errors ret=%d errno=%d lasterror=%d host=%s port=%u key=%" PRIu64 " session=%" PRIu64,
				nRet, errno, GetSocketError(), GetHost(), GetPort(), GetKey(),GetSessionIndex());
			return E_UNKNOW;
		}
	}
	m_pSendBuf->Remove(nRet);
	WRITE_DEBUG_LOG("real send size=%d host=%s port=%u key=%" PRIu64 " session=%" PRIu64,
		nRet,GetHost(),GetPort(),GetKey(),GetSessionIndex());
	return nRet;
}
int32_t CClientSocket::EpollIn()
{
	if(m_pRecvBuf->IsFull())
	{
		g_pSocketThread->EpollMod(this);
		return S_OK;
	}
	NetBuffer arrBuf[2];
	m_pRecvBuf->GetWriteEnable(arrBuf);
	int32_t nRet = Recv(arrBuf[0].buf,arrBuf[0].len);
	if(nRet <= 0)
	{
		if(HasError())
		{
			WRITE_ERROR_LOG("recv msg has errors ret=%d errno=%d lasterror=%d host=%s port=%u key=%" PRIu64 " session=%" PRIu64,
				nRet, errno, GetSocketError(), GetHost(), GetPort(), GetKey(), GetSessionIndex());
			return E_UNKNOW;
		}
		return S_OK;
	}
	if(static_cast<uint32_t>(nRet) == arrBuf[0].len && arrBuf[1].len > 0)
	{
		int32_t nTemp = Recv(arrBuf[1].buf,arrBuf[1].len);
		if (nTemp > 0)
			nRet += nTemp;
		else if (HasError())
		{
			WRITE_ERROR_LOG("recv msg has errors ret=%d errno=%d lasterror=%d host=%s port=%u key=%" PRIu64 " session=%" PRIu64,
				nRet, errno, GetSocketError(), GetHost(), GetPort(), GetKey(), GetSessionIndex());
			return E_UNKNOW;
		}	
	}
	m_pRecvBuf->Add(nRet);
	WRITE_DEBUG_LOG("real recv size=%d host=%s port=%u key=%" PRIu64 " session=%" PRIu64,
		nRet, GetHost(), GetPort(), GetKey(), GetSessionIndex());
	if(m_pRecvBuf->IsFull())
		g_pSocketThread->EpollMod(this);
	g_pServerBase->PushSocketEvent(enmEventType_Data, this);
	return nRet;
}
int32_t CClientSocket::ConnectSucc()
{
	WRITE_INFO_LOG("connect succ host=%s port=%u key=%" PRIu64 " sessionid=%" PRIu64,
		GetHost(),GetPort(),GetKey(),GetSessionIndex());
	m_status = enmSocketStatus_Connected;
	g_pServerBase->PushSocketEvent(enmEventType_Connect, this);
	return S_OK;
}
bool CClientSocket::SpiltMsg()
{
	static char buff[MaxOneMsg];
	NetBuffer arrBuf[2];
	uint32_t nSize = 0;
	uint32_t nCount = 0;
	while (++nCount < 50)
	{
		if (m_pRecvBuf->GetSize() < PACKHEADSIZE)return false;
		m_pRecvBuf->GetReadEnable(arrBuf);
		if (arrBuf[0].len >= sizeof nSize)
			nSize = *(uint32_t*)arrBuf[0].buf;
		else
		{
			char *p = (char*)&nSize;
			memcpy(p, arrBuf[0].buf, arrBuf[0].len);
			memcpy(p + arrBuf[0].len, arrBuf[1].buf, sizeof(nSize) - arrBuf[0].len);
		}
		if (arrBuf[0].len + arrBuf[1].len < nSize)return false;
		if (nSize > MaxOneMsg || 0 == nSize)
		{
			g_pSocketThread->AddKickSession(GetSessionIndex());
			return false;
		}
		if (arrBuf[0].len >=nSize)
			DispatchMsg(arrBuf[0].buf,nSize);
		else
		{
			memcpy(buff, arrBuf[0].buf, arrBuf[0].len);
			memcpy(buff+arrBuf[0].len, arrBuf[1].buf,nSize - arrBuf[0].len);
			DispatchMsg(buff, nSize);
		}
		m_pRecvBuf->Remove(nSize);
	}
	return true;
}
void CClientSocket::SendListMsg()
{
	//这个函数在主线程调用
	if (m_sendMsgList.empty() || !CanSendMsg())return;
	auto freeSize = m_pSendBuf->GetFreeSize();
	if (m_sendMsgList.size() <= freeSize)
	{
		m_pSendBuf->AddOnlyBuf(m_sendMsgList.data(), (uint32_t)m_sendMsgList.size());
		m_sendMsgList.clear();
	}
	else if(freeSize > 0)
	{
		m_pSendBuf->AddOnlyBuf(m_sendMsgList.data(), freeSize);
		m_sendMsgList.erase(0, freeSize);
	}
	if(freeSize > 0)
		g_pSocketThread->EpollMod(this);
	if (!m_sendMsgList.empty())
		g_pServerBase->AddIntervalTimer(TimerType::enmTimerType_SendMsg,100);
}
void CClientSocket::SendMsgToBuff(const NetBuffer arrBuf[2])
{
	auto freeSize = m_pSendBuf->GetFreeSize();
	if (freeSize >= arrBuf[0].len)
	{
		m_pSendBuf->AddOnlyBuf(arrBuf[0].buf, arrBuf[0].len);
		freeSize -= arrBuf[0].len;
	}
	else
	{
		m_pSendBuf->AddOnlyBuf(arrBuf[0].buf, freeSize);
		m_sendMsgList.append(arrBuf[0].buf + freeSize, arrBuf[0].len - freeSize);
		freeSize = 0;
	}
	if (arrBuf[1].len > 0)
	{
		if (freeSize >= arrBuf[1].len)
			m_pSendBuf->AddOnlyBuf(arrBuf[1].buf, arrBuf[1].len);
		else
		{
			m_pSendBuf->AddOnlyBuf(arrBuf[1].buf, freeSize);
			m_sendMsgList.append(arrBuf[1].buf + freeSize, arrBuf[1].len - freeSize);
		}
	}
	g_pSocketThread->EpollMod(this);
	if (!m_sendMsgList.empty())
		g_pServerBase->AddIntervalTimer(TimerType::enmTimerType_SendMsg, 100, GetSessionIndex());
}
bool CClientSocket::SendMsg(NetBuffer arrBuf[2])
{
	if (!CanSendMsg())
	{
		WRITE_WARING_LOG("can not send msg socketid=%u sessionid=%" PRIu64 " msgid=%04x size=%u",
			GetSocket(), GetSessionIndex(), *(uint16_t*)(arrBuf[0].buf + 4), arrBuf[0].len+ arrBuf[1].len);//假设协议头为4字节长度加2字节协议id
		return false;
	}
	if (!m_sendMsgList.empty())
	{
		m_sendMsgList.append(arrBuf[0].buf, arrBuf[0].len);
		if (arrBuf[1].len > 0)
			m_sendMsgList.append(arrBuf[1].buf, arrBuf[1].len);
		return true;
	}
	if (!m_pSendBuf->IsEmpty())
	{
		SendMsgToBuff(arrBuf);
		return true;
	}
	auto nRet = Send(arrBuf[0].buf, arrBuf[0].len);
	if (nRet <= 0 && HasError())
	{
		WRITE_ERROR_LOG("send msg has error host=%s port=%u session=%" PRIu64, GetHost(), GetPort(), GetSessionIndex());
		return false;
	}
	if ((uint32_t)nRet == arrBuf[0].len)
	{
		if (arrBuf[1].len == 0)return true;
		nRet = Send(arrBuf[1].buf, arrBuf[1].len);
		if (nRet <= 0 && HasError())
		{
			WRITE_ERROR_LOG("send msg has error host=%s port=%u session=%" PRIu64, GetHost(), GetPort(), GetSessionIndex());
			return false;
		}
		if (nRet >0 &&(uint32_t)nRet == arrBuf[1].len)return true;
		if (nRet < 0)nRet = 0;
		SendMsgToBuff(arrBuf[1].buf + nRet, arrBuf[1].len - nRet);
		return true;
	}
	if (nRet < 0)nRet = 0;
	arrBuf[0].buf += nRet;
	arrBuf[0].len = -nRet;
	SendMsgToBuff(arrBuf);
	return true;
}
void CClientSocket::SendMsgToBuff(const char* pMsg, uint32_t size)
{
	auto freeSize = m_pSendBuf->GetFreeSize();
	if (freeSize >= size)
		m_pSendBuf->AddOnlyBuf(pMsg, size);
	else
	{
		m_pSendBuf->AddOnlyBuf(pMsg, freeSize);
		m_sendMsgList.append(pMsg + freeSize, size - freeSize);
		g_pServerBase->AddIntervalTimer(TimerType::enmTimerType_SendMsg, 100, GetSessionIndex());
	}
	if(m_pSendBuf->GetSize() == size)
		g_pSocketThread->EpollMod(this);
}
bool CClientSocket::SendMsg(const char* pMsg, uint32_t size)
{
	//这个函数在主线程调用
	if (!CanSendMsg())
	{
		WRITE_WARING_LOG("can not send msg socketid=%u sessionid=%" PRIu64 " msgid=%04x size=%u",
			GetSocket(),GetSessionIndex(),*(uint16_t*)(pMsg+4),size);//假设协议头为4字节长度加2字节协议id
		return false;
	}
	if (!m_sendMsgList.empty())
	{
		m_sendMsgList.append(pMsg,size);
		return true;
	}
	if (!m_pSendBuf->IsEmpty())
	{
		SendMsgToBuff(pMsg, size);
		return true;
	}
	auto nRet = Send(pMsg, size);
	if (nRet <= 0 && HasError())
	{
		WRITE_ERROR_LOG("send msg has error host=%s port=%u session=%" PRIu64, GetHost(), GetPort(), GetSessionIndex());
		return false;
	}
	if ((uint32_t)nRet == size)return true;
	if (nRet < 0)nRet = 0;
	SendMsgToBuff(pMsg+nRet, size-nRet);
	return true;
}
bool CClientSocket::OnAccept()
{
	 g_pSocketThread->AddTimerSec(TimerType::enmTimerType_SocketAct, MAXSOCKETCLOSETIME, MAXSOCKETCLOSETIME, GetSessionIndex());
	 g_pServerBase->PushSocketEvent(enmEventType_Accept, this);
	 return true;
}
 int32_t CClientSocket::EpollErr()
 {
	 g_pServerBase->PushSocketEvent(enmEventType_Remove, this);
	 return CSocket::EpollErr();
 }
 int32_t CClientSocket::GetLength(const char* szData, uint32_t nDataLen, uint32_t& nContentPos, uint32_t& nContentLen)
 {
	 //根据http头结束符判断
	 char* pHeadEnd = strstr((char*)szData, "\r\n\r\n");
	 if (pHeadEnd == nullptr)return 0;
	 nContentPos = 0;
	 nContentLen = 0;
	 uint32_t nSize = uint32_t(pHeadEnd + 4 - szData);
	 if (memcmp(szData, "GET ", 4) == 0)
	 {
		 pHeadEnd = strstr((char*)szData, "?");
		 if (pHeadEnd != nullptr)
		 {
			 char* pContentLen = strstr((char*)szData, "\r\n");//GET url?data HTTP/1.0\r\n GET url?data HTTP/1/1\r\n
			 if (pContentLen != nullptr && pContentLen - pHeadEnd > 10)
			 {
				 nContentPos = uint32_t(pHeadEnd + 1 - szData);
				 nContentLen = uint32_t(pContentLen - pHeadEnd - 10);
			 }
		 }
	 }
	 else if ((memcmp(szData, "HTTP/", 5) == 0) || (memcmp(szData, "POST ", 5) == 0))
	 {
		 char* pContentLen = strstr((char*)szData, "Content-Length: ");
		 if (nullptr == pContentLen)//数据有问题
			 return -1;
		 pContentLen += 16;//strlen("Content-Length: ");
		 nContentLen = atoi(pContentLen);
		 nContentPos = nSize;
		 nSize += nContentLen;
	 }
	 else
		 return -1;
	 return nSize <= nDataLen ? nSize : 0;
 }
 bool CClientSocket::SSLHandshake()
 {
	 auto nRet = SSL_do_handshake(GetSSL());
	 if (1 == nRet)
	 {
		 SetStatus(enmSocketStatus_SSLConnected);
		 g_pSocketThread->EpollMod(this);
		 return true;
	 }
	 auto nErr = SSL_get_error(GetSSL(), nRet);
	 if (nErr == SSL_ERROR_WANT_READ || nErr == SSL_ERROR_WANT_WRITE)
		 return true;
	 WRITE_ERROR_LOG("SSL_do_handshake error port=%u ip=%s ret=%d err=%d", GetPort(), GetHost(), nRet, nErr);
	 return false;
 }
 int32_t CClientSocket::HttpsIn()
 {
	 if (!IsSSLConencted())
		 return SSLHandshake() ? S_OK : E_UNKNOW;
	 int nCount = 0;
	 int nRecvSize = 0;
	 while (++nCount < 10)
	 {
		 if (m_pRecvBuf->IsFull())//满了
		 {
			 g_pSocketThread->EpollMod(this);
			 g_pServerBase->PushSocketEvent(enmEventType_Data, this);
			 return S_OK;
		 }
		 NetBuffer arrBuf[2];
		 m_pRecvBuf->GetWriteEnable(arrBuf);
		 int nRet = SSL_read(GetSSL(), arrBuf[0].buf, arrBuf[0].len);
		 if (nRet > 0)
		 {
			 m_pRecvBuf->Add(nRet);
			 nRecvSize += nRet;
		 }
		 else
		 {
			 auto nErr = SSL_get_error(GetSSL(), nRet);
			 if (nErr != SSL_ERROR_WANT_READ)
				 return E_UNKNOW;//socket 已经发生了错误
			 break;
		 }
	 }
	 if (nCount >= 10)//达到了循环次数
		 g_pSocketThread->EpollMod(this);
	 if (nRecvSize > 0)//接收到了数据处理
		 g_pServerBase->PushSocketEvent(enmEventType_Data, this);
	 return S_OK;
 }

 int32_t CClientSocket::HttpsOut()
 {
	 if (!IsSSLConencted())
		 return SSLHandshake() ? S_OK : E_UNKNOW;
	 int nCount = 0;
	 while (++nCount < 10)
	 {
		 if (m_pSendBuf->IsEmpty())return S_OK;
		 NetBuffer arrBuf[2];
		 m_pSendBuf->GetReadEnable(arrBuf);
		 if (GetLastSendSize() > 0)//上次数据没有发送成功需要用相同的参数发送
		 {
			 int nRet = SSL_write(GetSSL(), arrBuf[0].buf, GetLastSendSize());
			 if (nRet >0 && (uint32_t)nRet == GetLastSendSize())
			 {
				 m_pSendBuf->Remove(GetLastSendSize());
				 SetLastSendSize(0);
				 continue;
			 }
			 auto nErr = SSL_get_error(GetSSL(), nRet);
			 if (nErr != SSL_ERROR_WANT_WRITE)
				 return E_UNKNOW;//socket 已经发生了错误
			 return S_OK;
		 }
		 int nRet = SSL_write(GetSSL(), arrBuf[0].buf, arrBuf[0].len);
		 if (nRet > 0 && (uint32_t)nRet != arrBuf[0].len)
		 {
			 auto nErr = SSL_get_error(GetSSL(), nRet);
			 if (nErr != SSL_ERROR_WANT_WRITE)
				 return E_UNKNOW;//socket 已经发生了错误
			 SetLastSendSize(arrBuf[0].len);//保存参数
			 return S_OK;
		 }
		 m_pSendBuf->Remove(arrBuf[0].len);
	 }
	 if (nCount >= 10)//达到了循环次数
		 g_pSocketThread->EpollMod(this);
	 return S_OK;
 }