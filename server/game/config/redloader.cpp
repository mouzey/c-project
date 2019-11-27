#include"redloader.h"
#include "configmanager.h"
REGHOLDER(CRedCfgLoader)
bool CRedCfgLoader::LoadCfg()
{	
	if(CBaseLoader::Load("data/red.csv"))return true;
	WRITE_ERROR_LOG("load red.csv has error");
	return false;
}