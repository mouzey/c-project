#include"mgrmsgbodypkg.h"
#include"traitsmsghandle.h"
REGMGHHANDLE(CMgrmsgbodyPkg)
bool  CMgrmsgbodyPkg::Register()
{
	REGPKGINDEX(CMgrmsgbodyPkg);
	REGFUN(&CMgrmsgbodyPkg::DoCSMsgHeadEXReq);
	return false;
}
 uint32_t CMgrmsgbodyPkg::GetScore()const
 {
	 return 0; 
 }
 void CMgrmsgbodyPkg::CalcProp(AttributeMgr* mgr)const
 {
 }
 void CMgrmsgbodyPkg::SyncData()const
 {
 }
 void CMgrmsgbodyPkg::ToString(std::string& str)const
 {
 }
 void CMgrmsgbodyPkg::FromString(const std::string& str)
 {
 }
 void CMgrmsgbodyPkg::Init()
 {
 }
 void CMgrmsgbodyPkg::Offline()
 {
 }
 void CMgrmsgbodyPkg::NeWDay()
 {
 }
void CMgrmsgbodyPkg::DoCSMsgHeadEXReq(CRole* role,SCSMsgHeadEXReq&& stReq)
{
}
