#ifndef _PROTOCOL_STRUCT_H_
#define _PROTOCOL_STRUCT_H_
#include<map>
#include<unordered_map>
#include<set>
#include<unordered_set>
#include<vector>
#include<list>
#include<forward_list>
#include<deque>
#include<string>
#include "protocolenum.h"
/*客户端和服务器通信协议头*/
struct SCSMsgHead
{
	int8_t m_MsgSize;		/*消息长度*/
	uint8_t m_MsgID;		
	SCSMsgHead();
	SCSMsgHead(
		int8_t MsgSize,
		uint8_t MsgID);
	void Encode()const;
	void Decode();
};

struct STestMsgStruct
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
	STestMsgStruct();
	STestMsgStruct(
		uint64_t roleID,
		const std::string& name,
		const SCSMsgHead& head,
		ESkillType skilltype,
		const std::vector<int32_t>& vectorvtest1,
		const std::vector<std::string>& vectorvtest2,
		const std::vector<SCSMsgHead>& vectorvtest3,
		const std::map<int32_t,SCSMsgHead>& maptest1,
		const std::map<int32_t,SCSMsgHead>& maptest2);
	void Encode()const;
};

#endif//_PROTOCOL_STRUCT_H_