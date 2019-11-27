#include "pch.h"
#include "filterword.h"
//utf 最新标准最多4字节
CFilterWord::CFilterWord()
{
	m_arrLen[0]=0;
	memset(m_arrBadWord,0,sizeof(m_arrBadWord));
}
int8_t CFilterWord::GetLen(const char* pStr, const char*pEnd, uint8_t count/*=0*/)
{
	while (pStr < pEnd)
	{
		if(++count == 5)
			return 5;
		if ((*pStr&0xF0) == 0xE0)//1110xxxx 10xxxxxx 10xxxxxx
		{
			pStr += 3;
			m_arrLen[count] = m_arrLen[count-1]+3;
		}
		else if ((*pStr & 0x80) == 0)//0xxxxxxx
		{
			++pStr;
			m_arrLen[count] = m_arrLen[count-1]+1;
		}
		else if ((*pStr&0xF8) == 0xF0)//11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		{
			pStr += 4;
			m_arrLen[count] = m_arrLen[count-1]+4;
		}
		else //110xxxxx 10xxxxxx
		{
			pStr += 2;
			m_arrLen[count] = m_arrLen[count-1]+2;
		}
	}
	return pStr > pEnd ? -1 : count;
}
int8_t CFilterWord::GetLen(const char* pStr, const char*pEnd, uint8_t oldLen,uint8_t replaceLen)
{
	if(oldLen == replaceLen)
		return 0;
	uint8_t count = 0;
	for (uint8_t i = replaceLen+1;i <= oldLen;++i)
		m_arrLen[++count] = m_arrLen[i]-m_arrLen[replaceLen];
	pStr += m_arrLen[count];
	return GetLen(pStr,pEnd,count);
}
uint64_t CFilterWord::ToValue(const char* pStr,uint8_t len)
{
	uint64_t value = 0;
	memcpy(&value,pStr,m_arrLen[1]);
	if(len == 1)
		return value;
	uint64_t tmp = 0;
	memcpy(&tmp,&pStr[m_arrLen[1]],m_arrLen[2]-m_arrLen[1]);
	return (value<<32)|tmp;
}
uint32_t CFilterWord::ToU32(const char* pStr)
{
	switch (m_arrLen[1])
	{
	case 1:
		return uint32_t(pStr[0]);
	case 2:
		return uint32_t((pStr[0]&0x1F)<<6)|(pStr[1]&0x3F);
	case 3:
		return (uint32_t(pStr[0]&0x0F)<<12)|(uint32_t(pStr[1]&0x3F)<<6)|(pStr[2]&0x3F);
	default://4
		return (uint32_t(pStr[0]&0x07)<<18)|(uint32_t(pStr[1]&0x3F)<<12)|(uint32_t(pStr[2]&0x3F)<<6)|(pStr[3]&0x3F);
	}
	return 0;
}
void CFilterWord::ReplaceBadWord(const std::string& str)
{
	char*pStr = const_cast<char*>(str.data());
	char*pEnd = pStr+str.size();
	int8_t len = GetLen(pStr,pEnd);
	uint32_t size;
	std::string key;
	while (*pStr != '\0')
	{
		if (-1 == len)
		{
			const_cast<std::string&>(str).clear();
			return;
		}
		if (!m_arrBadWord[ToU32(pStr)])
		{
			pStr += m_arrLen[1];
			len = GetLen(pStr,pEnd,len > 4 ? 4 : len,1);
			continue;
		}
		switch (len)
		{
		case 5:
		{
			auto mapIter = m_longStr.find(key.assign(pStr,m_arrLen[4]));
			if (mapIter != m_longStr.end())
			{
				size = uint32_t(pEnd - pStr);
				for (auto iter=mapIter->second.rbegin(); iter != mapIter->second.rend();++iter)
				{
					if(size < iter->first || memcmp(pStr,iter->second.data(),iter->first) != 0)
						continue;
					memset(pStr,'*',iter->first);
					pStr+= iter->first;
					break;
				}
				len = GetLen(pStr,pEnd);
				break;
			}
			len = 4;
			//没有找到则继续执行case4 以下case类似
		}
		case 4:
			if (m_shortStr.find(key.assign(pStr,m_arrLen[4])) != m_shortStr.end())
			{
				memset(pStr,'*',m_arrLen[4]);
				pStr += m_arrLen[4];
				len = GetLen(pStr,pEnd);
				break;
			}
		case 3:
			if (m_shortStr.find(key.assign(pStr,m_arrLen[3])) != m_shortStr.end())
			{
				memset(pStr,'*',m_arrLen[3]);
				pStr += m_arrLen[3];
				len = GetLen(pStr, pEnd,len,3);
				break;
			}
		case 2:
			if (m_shortWord.find(ToValue(pStr,2)) != m_shortWord.end())
			{
				memset(pStr,'*',m_arrLen[2]);
				pStr += m_arrLen[2];
				len = GetLen(pStr, pEnd,len,2);
				break;
			}
		case 1:
			if (m_shortWord.find(ToValue(pStr,1)) != m_shortWord.end())
			{
				memset(pStr,'*',m_arrLen[1]);
				pStr += m_arrLen[1];
				len = GetLen(pStr, pEnd,len,1);
				break;
			}
		default://没有屏蔽字
			pStr += m_arrLen[1];
			len = GetLen(pStr, pEnd,len,1);
			break;
		}
	}
}
void CFilterWord::InsertStr(std::string&& str)
{
	uint8_t len = GetLen(str.data(),str.data()+str.size());
	m_arrBadWord[ToU32(str.data())]=true;
	if (3 == len || 4 == len)
		m_shortStr.emplace(str);
	else if( 1 == len || 2 == len)
		m_shortWord.emplace(ToValue(str.data(),len));
	else
	{
		std::string key(str.data(),m_arrLen[4]);
		auto iter = m_longStr.find(key);
		if (iter != m_longStr.end())
			iter->second.emplace((uint16_t)str.size(),str);
		else
		{
			std::multimap<uint16_t,std::string> tmp;
			tmp.emplace((uint16_t)str.size(),str);
			m_longStr.emplace(std::move(key),std::move(tmp));
		}
	}
}
bool CFilterWord::HasBadWord(const std::string& str)
{
	const char* pStr = str.data();
	const char*pEnd = pStr + str.size();
	int8_t len = GetLen(pStr,pEnd);
	std::string key;
	while (*pStr != '\0')
	{
		if (-1 == len)
			return true;
		if (!m_arrBadWord[ToU32(pStr)])
		{
			pStr += m_arrLen[1];
			len = GetLen(pStr,pEnd,len > 4 ? 4 : len,1);
			continue;
		}
		switch (len)
		{
		case 5:
			if (m_longStr.find(key.assign(pStr,m_arrLen[4])) != m_longStr.end())
				return true;
			len = 4;
		case 4:
			if (m_shortStr.find(key.assign(pStr,m_arrLen[4])) != m_shortStr.end())
				return true;
		case 3:
			if (m_shortStr.find(key.assign(pStr,m_arrLen[3])) != m_shortStr.end())
				return true;
		case 2:
			if (m_shortWord.find(ToValue(pStr,2)) != m_shortWord.end())
				return true;
		case 1:
			if (m_shortWord.find(ToValue(pStr,1)) != m_shortWord.end())
				return true;
		default://没有屏蔽字
			pStr += m_arrLen[1];
			len = GetLen(pStr,pEnd,len,1);
			break;
		}
	}
	return false;
}

CWordNode::CWordNode()
{
	m_pNode = new WordNodeBase[256];
}
CWordNode::~CWordNode()
{
	delete[] m_pNode;
}
void CWordNode::InsertStr(const std::string& str, uint32_t index)
{
	uint8_t val = (uint8_t)str[index];
	if (m_pNode[val].val == 0xffff)
	{
		m_pNode[val].val = val;
		++m_pNode[0].size;
	}
	if (index == str.size() - 1)
		m_pNode[val].end = true;
	else
	{
		if (m_pNode->next == nullptr)
			m_pNode->next = new CWordNode;
		m_pNode->next->InsertStr(str, ++index);
	}
}

void CWordNode::Shit()
{
	auto tmp = new WordNodeBase[m_pNode[0].size];
	for (int i = 0,j = 0; i < 256; ++i)
		if (m_pNode[i].val != 0xffff)
			tmp[j++] = m_pNode[i];
	tmp[0].size = m_pNode[0].size;
	delete[] m_pNode;
	m_pNode = tmp;
	for (int i = 0; i < tmp[0].size; ++i)
		if (tmp[i].next != nullptr)
			tmp[i].next->Shit();
	
}
int CWordNode::FindIndex(WordNodeBase* node, uint8_t val)
{
	int left = 0;
	int right = node[0].size;
	while (left <= right)
	{
		int mid = (left + right) >> 1;
		if (node[mid].val == val)return mid;
		else if (node[mid].val < val)
			left = mid + 1;
		else
			right = mid - 1;
	}
	return -1;
}
int CWordNode::Replace(std::string& str, int begin, int index)
{
	int end = m_pNode[index].end ? begin  : -1;
	auto node = m_pNode[index].next->m_pNode;
	for (uint32_t i = begin + 1; i < str.size(); ++i)
	{
		index = FindIndex(node, (uint8_t)str[i]);
		if (-1 == index)break;
		if(node[index].end)end = i;
		if (nullptr == node[index].next)break;
		node = node[index].next->m_pNode;
	}
	if (-1 == end)return begin+1;
	for (int j = begin; j <= end; ++j)
		str[j] = '*';
	return end + 1;
}
void CWordNode::Replace(std::string& str)
{
	int index = -1;
	for (uint32_t i = 0; i < str.size();)
	{
		index = FindIndex(m_pNode, (uint8_t)str[i]);
		if (-1 != index)
			i = Replace(str, i, index);
		else
			++i;
	}
}
