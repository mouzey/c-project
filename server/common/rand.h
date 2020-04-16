#pragma once
#include <stdint.h>
//Ëæ»úº¯Êý
class CRandom
{
public:
	CRandom();
	~CRandom();
	static uint8_t Rand8(uint8_t max=UINT8_MAX, uint8_t min=0);
	static uint16_t Rand16(uint16_t max=UINT16_MAX, uint16_t min=0);
	static uint32_t Rand32(uint32_t max=UINT32_MAX, uint32_t min=0);
	static uint64_t Rand64(uint64_t max=UINT64_MAX, uint64_t min=0);
	static void Seed();
	template<T>
	static void Rand(std::vector<T>&in,std::vector<T>&out){}
};
