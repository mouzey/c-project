#include "role.h"
#include "rolepkg.h"
#include "pkgbase.h"
#include "sql/roledatarecord.h"//表示角色存档表
CRole::CRole()
	: m_rolePkg(new CRolePkg(this, std::make_shared<CRoledataRecord>()))
{
	m_roleID = 0;
	m_serverID = 0;
	m_level = 0;
	m_vip = 0;
}
void CRole::RegPkg(CPkgBase* pkg, std::string* name)
{ 
	m_vecPkg[pkg->GetIndex()] = (std::pair<CPkgBase*, std::string*>(pkg, name));
}
void CRole::CalcScore()
{
	uint32_t score = 0;
	for (auto& iter : m_vecPkg)
		score += iter.first->GetScore();
}
void CRole::CalcProp()
{
	for (auto& iter : m_vecPkg)
		iter.first->CalcProp(nullptr);
}
void CRole::SyncData()const
{
	for (auto& iter : m_vecPkg)
		iter.first->SyncData();
}
void CRole::SaveData()const
{
	for (auto& iter : m_vecPkg)
		iter.first->ToString(*iter.second);
}
void CRole::LoadData()
{
	for (auto& iter : m_vecPkg)
		iter.first->FromString(*iter.second);
}
void CRole::Init()
{
	for (auto& iter : m_vecPkg)
		iter.first->Init();
}
void CRole::Offline()
{
	for (auto& iter : m_vecPkg)
		iter.first->Offline();
}
void CRole::NeWDay()
{
	for (auto& iter : m_vecPkg)
		iter.first->NeWDay();
}