#ifndef PROTOCOL_ENUM_H_
#define PROTOCOL_ENUM_H_
#include<cstdint>
/*技能类型枚举*/
enum ESkillType:uint8_t
{
	enmSkillType_type1 = 0,/*技能类型1 value字段可选默认会从0开始编号*/
	enmSkillType_type2 = 1,
};

enum EActType:int8_t
{
	enmActType_type1,/*活动类型1 value字段可选默认会从0开始编号*/
	enmActType_type2,
};

#endif