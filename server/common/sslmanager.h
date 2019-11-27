#pragma once
#include "define.h"
#include <string>
class CClientSocket;
class CSSLManager
{
public:
	CSSLManager();
	~CSSLManager();
	bool InitAsServer(const char* cert, const char* key);
	bool InitAsClient();
	bool AcceptSocket(CClientSocket* pSocket);
	bool ConnectSocket(CClientSocket* pSocket);
	bool SSLHandshake(CClientSocket* pSocket);
	bool EpollIn(CClientSocket* pSocket);
	bool EpollOut(CClientSocket* pSocket);
	bool SendWSMsg(CClientSocket* pSocket,char* msg,uint32_t size);
	bool RecvWSMsg(CClientSocket* pSocket);
	bool WSHandshake(CClientSocket* pSocket);
	void RecvData(CClientSocket* pSocket){}
	void WsClientConnectSucc(CClientSocket* pSocket);
	void WsClientSend(CClientSocket* pSocket,char* msg, uint32_t len);
	void WsClientSplitMsg(CClientSocket* pSocket, char* msg, uint32_t len);
	void CloseSocket(CClientSocket* pSocket);
private:
	SSL_CTX* m_ctx = nullptr;
};

