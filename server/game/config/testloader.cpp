#include"testloader.h"
#include "configmanager.h"
REGHOLDER(CTestCfgLoader)
bool CTestCfgLoader::LoadCfg()
{	
	if(CBaseLoader::Load("data/test.csv"))return true;
	WRITE_ERROR_LOG("load test.csv has error");
	return false;
}