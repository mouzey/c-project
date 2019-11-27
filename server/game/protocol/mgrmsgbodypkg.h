#pragma once
#include<string>
#include<memory>
#include "pkgbase.h"
#include"mgrmsgbodyprotocol.h"
class CRole;
class CMgrmsgbodyPkg :public CPkgBase
{
public:
	CMgrmsgbodyPkg(CRole* role, std::string* pkg) :CPkgBase(role, pkg) {}
	bool static Register();
	virtual uint32_t GetScore()const;
	virtual void CalcProp(AttributeMgr* mgr)const;
	virtual void SyncData()const;
	virtual void ToString(std::string& str)const;
	virtual void FromString(const std::string& str);
	virtual void Init();
	virtual void Offline();
	virtual void NeWDay();
	virtual uint8_t GetIndex() { return PkgIndexVal<CMgrmsgbodyPkg>::index; };
public:
	void DoCSMsgHeadEXReq(CRole* role,SCSMsgHeadEXReq&& stReq);
};