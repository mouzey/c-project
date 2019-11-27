#ifndef _TEST_LODER_H_
#define _TEST_LODER_H_
#include "loadcfgbase.h"
#include "testcfg.h"
class CTestCfgLoader:public CBaseLoader<STestConfig,std::unordered_map<int8_t, const STestConfig*>>,public CSingleton<CTestCfgLoader>
{
public:
	CTestCfgLoader(){}
	virtual ~CTestCfgLoader(){}
	bool LoadCfg();
};
#endif