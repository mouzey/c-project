#ifndef _FILTER_WORD_H_
#define _FILTER_WORD_H_
#include <cstdint>
#include <unordered_set>
#include <string>
#include <unordered_map>
#include <map>
#include <string.h>
#include "singleton.h"
//屏蔽字过滤可以用类似拉链法的形式来实现但是内存会需要更多不过速度快
class CFilterWord:public CSingleton<CFilterWord>
{
public:
	CFilterWord();
	void InsertStr(std::string&& str);
	void InsertStr(const std::string& str){InsertStr(std::string(str));}
	void InsertStr(const char* pStr,uint32_t len){InsertStr(std::string(pStr,len));}
	void InsertStr(const char* pStr){ InsertStr(pStr,(uint32_t)strlen(pStr));}
	void ReplaceBadWord(const std::string& str);
	bool HasBadWord(const std::string& str);
private:
	int8_t GetLen(const char* pStr, const char*pEnd,uint8_t count=0);
	int8_t GetLen(const char* pStr, const char*pEnd, uint8_t oldLen,uint8_t replaceLen);
	uint64_t ToValue(const char* pStr,uint8_t len);
	uint32_t ToU32(const char* pStr);
private:
	std::unordered_set<uint64_t> m_shortWord;
	std::unordered_set<std::string> m_shortStr;
	std::unordered_map<std::string,std::multimap<uint16_t,std::string>> m_longStr;
	uint8_t m_arrLen[5];
	bool m_arrBadWord[0x1fffff+1];
};
class CWordNode
{
	struct WordNodeBase
	{
		CWordNode* next = nullptr;
		uint16_t val = 0xffff;
		uint16_t size = 0;
		bool end = false;
	};
public:
	CWordNode();
	~CWordNode();
	void InsertStr(const std::string& str, uint32_t index);
	void Shit();
	void Replace(std::string& str);
private:
	int FindIndex(WordNodeBase* node, uint8_t val);
	int Replace(std::string& str, int begin, int index);
private:
	WordNodeBase* m_pNode = nullptr;
};
#endif