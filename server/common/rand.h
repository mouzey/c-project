#pragma once
#include <stdint.h>
//�������
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
	template<class T>
	static bool RandAlterIn(std::vector<T>& in, std::vector<T>& out, uint32_t num)//可能会改变输入 随机不重复
	{
		if (in.size() <= num)
		{
			out.insert(out.end(), in.begin(), in.end());
			return in.size() >= num;
		}
		for (size_t i = 0, maxRand = in.size() - 1;i < num;++i, --maxRand)
		{
			auto index = Rand64(maxRand);
			out.emplace_back(in[index]);
			std::swap(in[maxRand], in[index]);
		}
		return true;
	}
};
