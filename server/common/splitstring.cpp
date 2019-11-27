#include "pch.h"
#include "splitstring.h"
#include "singleton.h"
CFastSplitStr* g_pFastSplitStr=&CSingleton<CFastSplitStr>::GetInstance();
template<>
void CSplitStr<std::string>::Split(std::string&& str,char element)
{
	std::vector<std::string> tmp;
	std::string::size_type begin = 0;
	std::string::size_type end = str.find_first_of(element);
	while (end != std::string::npos)
	{
		tmp.emplace_back(str.substr(begin, end - begin));
		begin = end + 1;
		end = str.find_first_of(element,begin);
	}
	tmp.emplace_back(str.substr(begin));
	m_arrValue.emplace_back(std::move(tmp));
}
void CFastSplitStr::Split(std::string& str, char second /*= ';'*/, char element/*='-'*/)
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
			if(m_size >= 10)
			{
				m_error = true;
				return;
			}
			begin = end + 1;
			end = str.find_first_of(second, begin);
		} while (end != std::string::npos);
		Split(str.substr(begin), element);
	}
}
void CFastSplitStr::Split(std::string&& str,char element/*='-'*/)
{
	std::string::size_type begin = 0;
	std::string::size_type end = str.find_first_of(element);
	m_rowSize[m_size] = 0;
	while (end != std::string::npos)
	{
		m_arrValue[m_size][m_rowSize[m_size]++]=ToInt<uint64_t>(str.substr(begin,end-begin));
		if(m_rowSize[m_size] >= 10) 
		{
			++m_size;
			m_error = true;
			return;
		}
		begin = end + 1;
		end = str.find_first_of(element,begin);
	}
	m_arrValue[m_size][m_rowSize[m_size]++] = ToInt<uint64_t>(str.substr(begin));
	++m_size;
}
bool SplitAward(const std::string& str)
{
	return true;
}
bool SplitCost(const std::string& str)
{
	return true;
}
bool SplitProp(const std::string& str)
{
	return true;
}