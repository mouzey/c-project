#pragma once
#include "singleton.h"
#include"testmsgbodyprotocol.h"
class CRole;
class CTestmsgbodyMgr :public CSingleton<CTestmsgbodyMgr>
{
public:
	bool static Register();
public:
	void DopkCSMsgHeadReq(CRole* role,SpkCSMsgHeadReq&& stReq);
};