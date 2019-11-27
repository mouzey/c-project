#ifndef _ROLE_H_
#define _ROLE_H_
#include <string>
#include<array>
#include "define.h"
//定义的角色对象
class CPkgBase;
struct CRolePkg;
class CRole
{
public:
	CRole();
	~CRole(){}
	RoleID GetRoleID()const{return m_roleID;}
	const std::string& GetName()const{return m_name;}
	const std::string& GetACcount()const{ return m_account;}
	ServerID GetServerID()const{return m_serverID;}
	uint16_t GetLvel()const{return m_level;}
	uint8_t GetVip()const{return m_vip;}
	void RegPkg(CPkgBase* pkg, std::string* name);
	void CalcScore();
	void CalcProp();
	void SyncData()const;
	void SaveData()const;
	void LoadData();
	void Init();
	void Offline();
	void NeWDay();
	template<class T>
	T* GetPkg() { return (T*)m_vecPkg[PkgIndexVal<T>::index].first; }
private:
	std::array<std::pair<CPkgBase*,std::string*>, (uint8_t)PkgIndex::enmPkgIndex_Max>m_vecPkg;
	CRolePkg* m_rolePkg;
	RoleID m_roleID;
	ServerID m_serverID;
	uint16_t m_level;
	uint8_t  m_vip;
	std::string m_name;
	std::string m_account;
};
#endif