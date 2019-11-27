#ifndef _SNPRINTF_H_
#define _SNPRINTF_H_
#include <stdarg.h>
#include <string>
/*
//自己实现的snprintf函数
【标志位】【宽度】【.精度】【长度】类型
支持类型：
uint8_t hhu 
int8_t hhd 
hhx/hhX 8位 
uint16_t hd
int16_t hu
uint32_t u
int32_t d
uint64_t llu
int64_t lld
double f
float f
char c
string s
blob b 后面跟一个size参数"%b","test",4
hx/hX 16位
x/X 32位
llx/llX 64位
size_t z=int32_t or int64_t
time_t t=long
指针   p
标志位:(不实现)
-:左对齐右边填充空格在打印字符应该没有作用
+:强制输出正负号直接负数输出负号正数不输出
空格:输出空格获取符号同+
#:前缀0，0x,0X这个可以自己控制，小数点控制估计几乎不用
宽度：字符串的没有实现所以%m.ns达不到预期效果只能用%.ns并且n不能大于字符长度和%b相同避免编译器警告
精度：double和float都是用snprintf实现
长度：h,l,L,n,f,j,t,z没有实际的意义直接用类型控制
8进制未实现感觉用处不大
*/
class CSnprintf
{
public:
	typedef unsigned int  PrintType;
	enum
	{
		enmPrintFlag_8			= 0x1,
		enmPrintFlag_16			= 0x2,
		enmPrintFlag_32			= 0x4,
		enmPrintFlag_64			= 0x8,
		enmPrintFlag_F			= 0x10,
		enmPrintFlag_Decimal	= 0x20,//10进制
		enmPrintFlag_Hex_L		= 0x40,//十六进制小写
		enmPrintFlag_Hex_B		= 0x80,//十六进制大写
		enmPrintFlag_Zero		= 0x100,//用0补齐
		enmPrintFlag_Signed		= 0x200,
	};
public:
	static unsigned int GetNumberLength(unsigned char nValue,PrintType type);
	static unsigned int GetNumberLength(unsigned short nValue,PrintType type);
	static unsigned int GetNumberLength(unsigned int nValue,PrintType type);
	static unsigned int GetNumberLength(unsigned long long nValue,PrintType type);

	static int Snprintf(char *pBuf,unsigned int nMaxSize,const char *pFormat,...);
	static int Snprintf(char *pBuf,unsigned int nMaxSize,const char *pFormat,va_list ap);

	static int FormatValue(char *pBuf,unsigned int nMaxSize,char nValue,unsigned int nFlag,unsigned int nWidth);
	static int FormatValue(char *pBuf,unsigned int nMaxSize,unsigned char nValue,unsigned int nFlag,unsigned int nWidth);
	static int FormatValue(char *pBuf,unsigned int nMaxSize,short nValue,unsigned int nFlag,unsigned int nWidth);
	static int FormatValue(char *pBuf,unsigned int nMaxSize,unsigned short nValue,unsigned int nFlag,unsigned int nWidth);
	static int FormatValue(char *pBuf,unsigned int nMaxSize,int nValue,unsigned int nFlag,unsigned int nWidth);
	static int FormatValue(char *pBuf,unsigned int nMaxSize,unsigned int nValue,unsigned int nFlag,unsigned int nWidth);
	static int FormatValue(char *pBuf,unsigned int nMaxSize,long long  nValue,unsigned int nFlag,unsigned int nWidth);
	static int FormatValue(char *pBuf,unsigned int nMaxSize,unsigned long long nValue,unsigned int nFlag,unsigned int nWidth);

	static int SnprintfEX(char *pBuf,unsigned int nMaxSize,const char *pFormat,...);
	static int SnprintfEX(char *pBuf,unsigned int nMaxSize,const char *pFormat,va_list ap);

	static int FormatValueEX(char *pBuf,unsigned int nMaxSize,long long nValue,unsigned int nFlag,unsigned int nWidth);

	static int FormatValue(char *pBuf,unsigned int nMaxSize, double nValue,unsigned int nFlag,unsigned int nWidth,unsigned int nPrecision);
	static void FcvtEX(long double value,char *pBuf,unsigned int nPrecision);

	static std::string ToStr(long long  nValue);
	static std::string ToStr(unsigned long long nValue);
	static char* ToStr(unsigned long long nValue,char *pBuf);
private:
	static const char hexUp[17];
	static const char HexLow[17];
	static const char digits[201];
};
#endif