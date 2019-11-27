#pragma once
#include <list>
#include "tcpclientsocket.h"
class CHttpReqSocket :public CClientSocket
{
public:
	CHttpReqSocket();
	CHttpReqSocket(uint32_t recvSize, uint32_t sendSize,uint16_t port);
	virtual ~CHttpReqSocket();
	virtual bool SpiltMsg() override;
	virtual int32_t EpollOut()override;
	virtual int32_t EpollErr()override;
	virtual int32_t ConnectSucc()override;
	bool ParaseIp(const std::string& ip);
	bool SetData(bool post, bool json, const std::string& ip, const std::string& page, const std::string& data);
	int32_t GetRespCode(const char* pMsg);
	virtual void Error()override {}
private:
	std::list<std::string> m_listIp;
};

