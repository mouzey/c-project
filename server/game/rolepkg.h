#pragma once
#include<string>
#include<memory>
#include "pkgbase.h"
#include "protocol/mgrmsgbodypkg.h"
class CRole;
class CRoledataRecord;
struct CRolePkg
{
	CRolePkg(CRole* role,std::shared_ptr<CRoledataRecord> pRecord);
	std::shared_ptr<CRoledataRecord>m_record;
	CMgrmsgbodyPkg m_skill;
};

