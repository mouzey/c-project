#ifndef _H_TCPCLIENTSOCKET_H
#define _H_TCPCLIENTSOCKET_H
#include <string>
#include "socket.h"
#include "newqueue.h"
class CSSLManager;
class CClientSocket:public CSocket
{
public:
	friend CSSLManager;
	CClientSocket();
	~CClientSocket();
	CClientSocket(uint32_t recvSize,uint32_t sendSize);
	CClientSocket(const char *pHost,const uint32_t nPort,const bool bBlocking=true);
	static CSocket* CreateClient(){return new CClientSocket;}
public:
	virtual void Reinit();
	int32_t OpenAsClient(const char *pHost=NULL,const uint32_t nPort=0,const bool bNonBlocking=true);
	int32_t SendMessage(const char *pBuf,const uint32_t nSize);
	int32_t RecvMessage();
	int32_t Select();
	int32_t AttachOneMsg(char *pBuf,uint32_t &nSize,const uint32_t nMaxBufSize);
	int32_t SendMsg();
	int32_t RecvAllMsg();
	int32_t SendOneMsg();
	uint64_t GetKey()const { return m_key; }
	void SetKey(uint64_t key) { m_key = key; }
	bool IsConnected()const {return m_status >= enmSocketStatus_Connected;}
	virtual int32_t EpollIn()override;
	virtual int32_t EpollOut()override;
	//连接成功
	virtual int32_t ConnectSucc()override;
	virtual void ConnectSucc(bool) {}//主线程调用
	//拆包
	virtual bool SpiltMsg()override;
	virtual bool SendMsg(NetBuffer arrBuf[2])override;
	void SendListMsg();
	int32_t GetLength(const char* szData, uint32_t nDataLen, uint32_t& nContentPos, uint32_t& nContentLen);
	virtual bool SendMsg(const char* pMsg, uint32_t size);
	virtual bool OnAccept()override;
	virtual void OnAccept(bool) {}//主线程调用
	virtual int32_t EpollErr()override;
	virtual void DispatchMsg(const char* pMsg, uint32_t size) {}
	virtual void SetStatus(SocketStatus status) { m_status = status; }
	virtual bool IsSSLConencted()const { return m_status >= enmSocketStatus_SSLConnected; }
	virtual bool IsWSConencted()const { return m_status >= enmSocketStatus_WSConnected; }
	virtual void SetSSL(SSL* ssl) {}
	virtual SSL* GetSSL() { return nullptr; }
	virtual uint32_t GetLastSendSize()const { return 0; }
	virtual void SetLastSendSize(uint32_t val) { }
	virtual void Error() {}
	void SendMsgToBuff(const char* pMsg, uint32_t size);
	bool SSLHandshake();
	int32_t HttpsOut();
	int32_t HttpsIn();
private:
	//是否正在等待连接
	virtual bool IsWaitConnect()const {return  m_status == enmSocketStatus_Connecting;}
	virtual bool IsLogin()const override{ return enmSocketStatus_Login == m_status; }
	void SendMsgToBuff(const NetBuffer arrBuf[2]);
protected:
	CNewQueue<char> *m_pRecvBuf;
	CNewQueue<char> *m_pSendBuf;
private:
	std::string m_sendMsgList;//m_pSendBuf满了的时候存在这个里面
	uint64_t m_key;
	SocketStatus m_status;
};
#endif
