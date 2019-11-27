#include"testmsgbodymgr.h"
#include"traitsmsghandle.h"
REGMGHHANDLE(CTestmsgbodyMgr)
bool  CTestmsgbodyMgr::Register()
{
	REGFUN(&CTestmsgbodyMgr::DopkCSMsgHeadReq);
	return false;
}
void CTestmsgbodyMgr::DopkCSMsgHeadReq(CRole* role,SpkCSMsgHeadReq&& stReq)
{
}
