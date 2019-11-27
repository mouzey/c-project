#include"protocolstruct.h"
#include"codebase.h"

SCSMsgHead::SCSMsgHead()
	:m_MsgSize(0)
	,m_MsgID(0)
{}

SCSMsgHead::SCSMsgHead(
		int8_t MsgSize,
		uint8_t MsgID):
	m_MsgSize(MsgSize),
	m_MsgID(MsgID)
	{}
	
void SCSMsgHead::Encode()const
{
	CCode::Encode(m_MsgSize);
	CCode::Encode(m_MsgID);
}

void SCSMsgHead::Decode()
{
	CCode::Decode(m_MsgSize);
	CCode::Decode(m_MsgID);
}

STestMsgStruct::STestMsgStruct()
	:m_roleID(0)
	,m_skilltype(enmSkillType_type1)
{}

STestMsgStruct::STestMsgStruct(
		uint64_t roleID,
		const std::string& name,
		const SCSMsgHead& head,
		ESkillType skilltype,
		const std::vector<int32_t>& vectorvtest1,
		const std::vector<std::string>& vectorvtest2,
		const std::vector<SCSMsgHead>& vectorvtest3,
		const std::map<int32_t,SCSMsgHead>& maptest1,
		const std::map<int32_t,SCSMsgHead>& maptest2):
	m_roleID(roleID),
	m_name(name),
	m_head(head),
	m_skilltype(skilltype),
	m_vectorvtest1(vectorvtest1),
	m_vectorvtest2(vectorvtest2),
	m_vectorvtest3(vectorvtest3),
	m_maptest1(maptest1),
	m_maptest2(maptest2)
	{}
	
void STestMsgStruct::Encode()const
{
	CCode::Encode(m_roleID);
	CCode::Encode(m_name);
	CCode::Encode(m_head);
	CCode::Encode(m_skilltype);
	CCode::Encode(m_vectorvtest1);
	CCode::Encode(m_vectorvtest2);
	CCode::Encode(m_vectorvtest3);
	CCode::Encode(m_maptest1);
	for(auto& iter : m_maptest2)
		CCode::Encode(iter.second);
}

