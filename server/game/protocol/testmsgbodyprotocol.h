#ifndef _TESTMSGBODY_PROTOCOL_H_
#define _TESTMSGBODY_PROTOCOL_H_
#include "msgbase.h"
#include "protocolstruct.h"
/*客户端和服务器通信协议头*/
struct SpkCSMsgHeadReq:public CMsgBase
{
	int8_t m_MsgSize;		/*消息长度*/
	uint8_t m_MsgID;		
	static uint16_t Code(){return 0X0101;}
	SpkCSMsgHeadReq();
	SpkCSMsgHeadReq(
		int8_t MsgSize,
		uint8_t MsgID);
	bool Encode()const;
	bool Decode();
};

/*服务器内部通信协议头*/
struct SpkSSMsgHeadResp:public CMsgBase
{
	uint64_t m_roleID;		/*基础类型*/
	std::string m_name;		/*字符类型*/
	SCSMsgHead m_head;		/*结构体类型*/
	ESkillType m_skilltype;		/*枚举类型*/
	std::vector<int32_t> m_vectorvtest1;		/*arr字段取值为vector表示数组 为map表示键值对 arrtype表示后端用的数据类型 如果没有那直接使用arr作为后端数据类型*/
	std::vector<std::string> m_vectorvtest2;		/*arr字段取值为vector表示数组 为map表示键值对 arrtype表示后端用的数据类型 如果没有那直接使用arr作为后端数据类型*/
	std::vector<SCSMsgHead> m_vectorvtest3;		/*arr字段取值为vector表示数组 为map表示键值对 arrtype表示后端用的数据类型 如果没有那直接使用arr作为后端数据类型*/
	std::map<int32_t,SCSMsgHead> m_maptest1;		/*arr字段取值为vector表示数组 为map表示键值对 arrtype表示后端用的数据类型 如果没有那直接使用arr作为后端数据类型*/
	std::map<int32_t,SCSMsgHead> m_maptest2;		/*arr字段取值为vector表示数组 为map表示键值对 arrtype表示后端用的数据类型 如果没有那直接使用arr作为后端数据类型*/
	static uint16_t Code(){return 0X0102;}
	SpkSSMsgHeadResp();
	SpkSSMsgHeadResp(
		uint64_t roleID,
		const std::string& name,
		const SCSMsgHead& head,
		ESkillType skilltype,
		const std::vector<int32_t>& vectorvtest1,
		const std::vector<std::string>& vectorvtest2,
		const std::vector<SCSMsgHead>& vectorvtest3,
		const std::map<int32_t,SCSMsgHead>& maptest1,
		const std::map<int32_t,SCSMsgHead>& maptest2);
	bool Encode()const;
};

struct SpkTestMsgStructNoti:public CMsgBase
{
	uint64_t m_roleID;		/*基础类型*/
	std::string m_name;		/*字符类型*/
	SCSMsgHead m_head;		/*结构体类型*/
	ESkillType m_skilltype;		/*枚举类型*/
	std::vector<int32_t> m_vectorvtest1;		/*arr字段取值为vector表示数组 为map表示键值对 arrtype表示后端用的数据类型 如果没有那直接使用arr作为后端数据类型*/
	std::vector<std::string> m_vectorvtest2;		/*arr字段取值为vector表示数组 为map表示键值对 arrtype表示后端用的数据类型 如果没有那直接使用arr作为后端数据类型*/
	std::vector<SCSMsgHead> m_vectorvtest3;		/*arr字段取值为vector表示数组 为map表示键值对 arrtype表示后端用的数据类型 如果没有那直接使用arr作为后端数据类型*/
	std::map<int32_t,SCSMsgHead> m_maptest1;		/*arr字段取值为vector表示数组 为map表示键值对 arrtype表示后端用的数据类型 如果没有那直接使用arr作为后端数据类型*/
	std::map<int32_t,SCSMsgHead> m_maptest2;		/*arr字段取值为vector表示数组 为map表示键值对 arrtype表示后端用的数据类型 如果没有那直接使用arr作为后端数据类型*/
	static uint16_t Code(){return 0X0103;}
	SpkTestMsgStructNoti();
	SpkTestMsgStructNoti(
		uint64_t roleID,
		const std::string& name,
		const SCSMsgHead& head,
		ESkillType skilltype,
		const std::vector<int32_t>& vectorvtest1,
		const std::vector<std::string>& vectorvtest2,
		const std::vector<SCSMsgHead>& vectorvtest3,
		const std::map<int32_t,SCSMsgHead>& maptest1,
		const std::map<int32_t,SCSMsgHead>& maptest2);
	bool Encode()const;
};

#endif//_TESTMSGBODY_PROTOCOL_H_