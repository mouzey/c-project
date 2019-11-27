#ifndef _RED_CFG_H_
#define _RED_CFG_H_
#include<cstdint>
#include<cstring>
#include "readcsv.h"
struct SRedConfig
{
	static uint16_t GetCol(){return 13;}
	const std::string& GetId()const{return m_val1;}
	void ReadData(CReadCsv& csv)
	{
		csv.ReadValue(0,m_val1);
		csv.ReadValue(1,m_val2);
		csv.ReadValue(2,m_val3);
		csv.ReadValue(3,m_val4);
		csv.ReadValue(4,m_val5);
		csv.ReadValue(5,m_val6);
		csv.ReadValue(6,m_val7);
		csv.ReadValue(7,m_val8);
		csv.ReadValue(8,m_val9);
		csv.ReadValue(9,m_val10);
		csv.ReadValue(10,m_val11);
		csv.ReadValue(11,m_val12);
		csv.ReadValue(12,m_val13);
	}
	std::string m_val1;/*字段*/
	bool m_val2 = 0;/*字段*/
	uint8_t m_val3 = 0;/*字段*/
	int16_t m_val4 = 0;/*字段*/
	uint16_t m_val5 = 0;/*字段*/
	int32_t m_val6 = 0;/*字段*/
	uint32_t m_val7 = 0;/*字段*/
	int64_t m_val8 = 0;/*字段*/
	uint64_t m_val9 = 0;/*字段*/
	std::string m_val10;/*字段*/
	std::string m_val11;/*字段*/
	double m_val12 = 0.0;
	float m_val13 = 0.0;
};
#endif