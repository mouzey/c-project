#include"mgrmsgbodyprotocol.h"
#include"codebase.h"

SCSMsgHeadEXReq::SCSMsgHeadEXReq():CMsgBase(SCSMsgHeadEXReq::Code())
,m_MsgSize(0)
,m_MsgID(0)
{}
SCSMsgHeadEXReq::SCSMsgHeadEXReq(
		int8_t MsgSize,
		uint8_t MsgID)
	:CMsgBase(SCSMsgHeadEXReq::Code())
	,m_MsgSize(MsgSize)
	,m_MsgID(MsgID)
	{}

bool SCSMsgHeadEXReq::Encode()const
{
	CMsgBase::Encode();
	CCode::Encode(m_MsgSize);
	CCode::Encode(m_MsgID);
	return CCode::HasError();
}

bool SCSMsgHeadEXReq::Decode()
{
	CMsgBase::Decode();
	CCode::Decode(m_MsgSize);
	CCode::Decode(m_MsgID);
	return CCode::HasError();
}

SSSMsgHeadEXResp::SSSMsgHeadEXResp():CMsgBase(SSSMsgHeadEXResp::Code())
,m_roleID(0)
,m_skilltype(enmSkillType_type2)
{}
SSSMsgHeadEXResp::SSSMsgHeadEXResp(
		uint64_t roleID,
		const std::string& name,
		const SCSMsgHead& head,
		ESkillType skilltype,
		const std::vector<int32_t>& vectorvtest1,
		const std::vector<std::string>& vectorvtest2,
		const std::vector<SCSMsgHead>& vectorvtest3,
		const std::map<int32_t,SCSMsgHead>& maptest1,
		const std::map<int32_t,SCSMsgHead>& maptest2)
	:CMsgBase(SSSMsgHeadEXResp::Code())
	,m_roleID(roleID)
	,m_name(name)
	,m_head(head)
	,m_skilltype(skilltype)
	,m_vectorvtest1(vectorvtest1)
	,m_vectorvtest2(vectorvtest2)
	,m_vectorvtest3(vectorvtest3)
	,m_maptest1(maptest1)
	,m_maptest2(maptest2)
	{}

bool SSSMsgHeadEXResp::Encode()const
{
	CMsgBase::Encode();
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
	return CCode::HasError();
}

STestMsgStructEXNoti::STestMsgStructEXNoti():CMsgBase(STestMsgStructEXNoti::Code())
,m_roleID(0)
,m_skilltype(enmSkillType_type2)
{}
STestMsgStructEXNoti::STestMsgStructEXNoti(
		uint64_t roleID,
		const std::string& name,
		const SCSMsgHead& head,
		ESkillType skilltype,
		const std::vector<int32_t>& vectorvtest1,
		const std::vector<std::string>& vectorvtest2,
		const std::vector<SCSMsgHead>& vectorvtest3,
		const std::map<int32_t,SCSMsgHead>& maptest1,
		const std::map<int32_t,SCSMsgHead>& maptest2)
	:CMsgBase(STestMsgStructEXNoti::Code())
	,m_roleID(roleID)
	,m_name(name)
	,m_head(head)
	,m_skilltype(skilltype)
	,m_vectorvtest1(vectorvtest1)
	,m_vectorvtest2(vectorvtest2)
	,m_vectorvtest3(vectorvtest3)
	,m_maptest1(maptest1)
	,m_maptest2(maptest2)
	{}

bool STestMsgStructEXNoti::Encode()const
{
	CMsgBase::Encode();
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
	return CCode::HasError();
}

