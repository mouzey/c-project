#ifndef _READ_CFG_H_
#define _READ_CFG_H_
#include <cstdint>
#include <string>
#include <cstdio>
#include "common_api.h"
//读取csv文件
class CReadCsv
{
public:
	CReadCsv();
	~CReadCsv();
	bool OpenFile(const std::string &strPath);
	bool ParseHead(uint16_t nCol);
	bool FetchRow(uint16_t nCol);
	bool FetchRow();
	uint32_t GetRow()const {return m_nRow;}
	//index 从0开始
	void ReadValue(uint16_t index,std::string& val)
	{
		uint16_t tmp = index << 1;
		val.assign(&m_buffer[m_arrVal[tmp]],m_arrVal[tmp+1]-m_arrVal[tmp]);
	}
	void ReadValue(uint16_t index,float& val){val=(float)strtod(&m_buffer[m_arrVal[index << 1]],nullptr);}
	void ReadValue(uint16_t index,double& val){val=strtod(&m_buffer[m_arrVal[index << 1]],nullptr);}
	void ReadValue(uint16_t index,bool& val){val = m_buffer[m_arrVal[index << 1]]=='1';}
	void ReadValue(uint16_t index,uint8_t& val){val=ToInt<uint8_t>(&m_buffer[m_arrVal[index<<1]]);}
	void ReadValue(uint16_t index,int8_t& val){val=ToInt<int8_t>(&m_buffer[m_arrVal[index<<1]]);}
	void ReadValue(uint16_t index,uint16_t& val){val=ToInt<uint16_t>(&m_buffer[m_arrVal[index<<1]]);}
	void ReadValue(uint16_t index,int16_t& val){val=ToInt<int16_t>(&m_buffer[m_arrVal[index<<1]]);}
	void ReadValue(uint16_t index,uint32_t& val){val=ToInt<uint32_t>(&m_buffer[m_arrVal[index<<1]]);}
	void ReadValue(uint16_t index,int32_t& val){val=ToInt<int32_t>(&m_buffer[m_arrVal[index<<1]]);}
	void ReadValue(uint16_t index,uint64_t& val){val=ToInt<uint64_t>(&m_buffer[m_arrVal[index<<1]]);}
	void ReadValue(uint16_t index,int64_t& val){val=ToInt<int64_t>(&m_buffer[m_arrVal[index<<1]]);}
private:
	void Parse(uint16_t begin = 0);
	void Close();
private:
	enum { MAXBUFFSIZE = 10240,MAXINDEX=500, };
	uint16_t m_arrVal[MAXINDEX];
	uint8_t m_nIndex;
	uint32_t m_nRow = 0;
	char m_buffer[MAXBUFFSIZE];
	FILE* m_fp;
};
#endif