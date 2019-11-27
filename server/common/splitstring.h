#ifndef _SPLIT_STRING_H_
#define _SPLIT_STRING_H_
#include <string>
#include <vector>
#include "common_api.h"
#include "log.h"
//字符分割
template<class T>
class CSplitStr
{
public:
	typedef typename std::vector<std::vector<T>>::const_iterator Iterator;
	CSplitStr(){}
	CSplitStr(std::string& str, char second = ';', char element='-') {Split(str,second,element);}
	void Split(std::string& str, char second = ';', char element='-')
	{
		Clear();
		Trim(str);
		if (str.empty())return;
		if (str.back() == second)
			str.pop_back();
		auto end = str.find_first_of(second);
		if(end == std::string::npos)
			Split(std::string(str), element);
		else
		{
			std::string::size_type begin = 0;
			do
			{
				Split(str.substr(begin, end - begin),element);
				begin = end + 1;
				end = str.find_first_of(second, begin);
			} while (end != std::string::npos);
			Split(str.substr(begin), element);
		}
	}
	Iterator begin()const { return m_arrValue.cbegin();}
	Iterator end()const { return m_arrValue.cend(); }
	size_t Size()const { return m_arrValue.size();}
	size_t EleSize(uint32_t index)const{ return index >= m_arrValue.size ? 0 : m_arrValue[index].size();}
	T GetData(uint32_t index, uint32_t pos)
	{  
		if(index >= m_arrValue.size() || pos >= m_arrValue[index].size())
			return -1;
		return m_arrValue[index][pos];
	}
	void GetData(uint32_t index, uint32_t pos, std::string& val)
	{ 
		if (index < m_arrValue.size() && pos < m_arrValue[index].size())
			val = std::move(m_arrValue[index][pos]); 
	}
private:
	void Split(std::string&& str,char element='-')
	{
		std::vector<T> tmp;
		std::string::size_type begin = 0;
		std::string::size_type end = str.find_first_of(element);
		while (end != std::string::npos)
		{
			tmp.emplace_back(ToInt<T>(str.substr(begin, end - begin)));
			begin = end + 1;
			end = str.find_first_of(element,begin);
		}
		tmp.emplace_back(ToInt<T>(str.substr(begin)));
		m_arrValue.emplace_back(std::move(tmp));
	}
	void Clear(){m_arrValue.clear();}
private:
	std::vector<std::vector<T>> m_arrValue;
};
template<>
void CSplitStr<std::string>::Split(std::string&& str,char element);
typedef CSplitStr<uint32_t> SplitStrToInt;
typedef CSplitStr<std::string> SplitStrToStr;

class CFastSplitStr
{
public:
	CFastSplitStr() { memset(m_arrValue, 0, sizeof m_arrValue); memset(m_rowSize, 0, sizeof m_rowSize); }
	CFastSplitStr(std::string& str,char second = ';',char element='-'){Split(str,second,element);}
	void Split(std::string& str, char second = ';', char element='-');
	uint8_t Size()const { return m_size;}
	uint8_t EleSize(uint32_t index)const{ return index >= m_size ? 0 : m_rowSize[index];}
	uint64_t GetData(uint32_t index, uint32_t pos)
	{  
		if(index >= 10 || pos >= 10)
			return -1;
		return m_arrValue[index][pos];
	}
	bool HasError(){return m_error;}
private:
	void Split(std::string&& str,char element='-');
	void Clear(){m_size=0; m_error = false;}
private:
	uint64_t m_arrValue[10][10];
	uint8_t m_rowSize[10];
	uint8_t m_size=0;
	bool m_error=false;
};
//全局对象快速版不是线程安全 线程安全可以用线程本地存储的单例
extern CFastSplitStr* g_pFastSplitStr;
bool SplitAward(const std::string& str);
bool SplitCost(const std::string& str);
bool SplitProp(const std::string& str);
template<class T,class V>
bool SplitTwo(const std::string& str, std::vector<std::pair<T, V>>& vec)
{
	CSplitStr split(const_cast<std::string&>(str));
	for (auto& iter : split)
	{
		if (iter.Size() != 2)
		{
			WRITE_ERROR_LOG("split str error not two element str=%s",str.data());
			return false;
		}
		vec.emplace_back(std::pair<T, V>((T)iter[0], (V)iter[1]));
	}
	return true;
}
#endif