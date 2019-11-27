#include "pch.h"
#include <random>
#include <time.h>
#include "rand.h"
static std::mt19937 mt32(static_cast<uint32_t>(time(nullptr)));
static std::mt19937_64 mt64(time(nullptr));
CRandom::CRandom()
{
}
CRandom::~CRandom()
{
}
 uint8_t CRandom::Rand8(uint8_t max, uint8_t min)
{
	std::uniform_int_distribution<uint32_t>tmp(min, max);
	return static_cast<uint8_t>(tmp(mt32));
}
 uint16_t CRandom::Rand16(uint16_t max, uint16_t min)
{
	std::uniform_int_distribution<uint16_t>tmp(min, max);
	return tmp(mt32);
}
 uint32_t CRandom::Rand32(uint32_t max, uint32_t min)
{
	std::uniform_int_distribution<uint32_t>tmp(min, max);
	return tmp(mt32);
}
uint64_t CRandom::Rand64(uint64_t max, uint64_t min)
{
	std::uniform_int_distribution<uint64_t>tmp(min, max);
	return tmp(mt64);
}
void CRandom::Seed()
{
	mt32.seed(static_cast<uint32_t>(time(nullptr)));
	mt64.seed(static_cast<unsigned long>(time(nullptr)));
}