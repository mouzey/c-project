#pragma once
#include<cstdint>
#include<string>
#include"define.h"
class CRole;
class AttributeMgr {};//测试使用表示计算属性的类
class CPkgBase
{
public:
	CPkgBase(CRole* role,std::string* pkg);
	virtual ~CPkgBase() {}
	virtual uint32_t GetScore()const { return 0;}
	virtual void CalcProp(AttributeMgr* mgr)const {}
	virtual void SyncData()const {}
	virtual void ToString(std::string& str)const {}
	virtual void FromString(const std::string& str) {}
	virtual void Init() {}
	virtual void Offline(){}
	virtual void NeWDay() {}
	virtual uint8_t GetIndex() = 0;
	CRole* GetRole() { return m_role; }
private:
	CRole* m_role;
};