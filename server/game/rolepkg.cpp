#include "rolepkg.h"
#include "sql/roledatarecord.h"
 CRolePkg::CRolePkg(CRole* role, std::shared_ptr<CRoledataRecord> pRecord)
	 :m_record(pRecord),m_skill(role,&pRecord->skill)
 {

 }
