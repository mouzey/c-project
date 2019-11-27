#ifndef _RED_LODER_H_
#define _RED_LODER_H_
#include "loadcfgbase.h"
#include "redcfg.h"
class CRedCfgLoader:public CBaseLoader<SRedConfig,std::unordered_map<std::string, const SRedConfig*>>,public CSingleton<CRedCfgLoader>
{
public:
	CRedCfgLoader(){}
	virtual ~CRedCfgLoader(){}
	bool LoadCfg();
};
#endif