#include "pch.h"
#ifdef WIN32
#include <string>
#else
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <math.h>
#include <float.h>
#include "snprintf.h"
#include "define.h"

const char CSnprintf::hexUp[17]="0123456789ABCDEF";
const char CSnprintf::HexLow[17]="0123456789abcdef";
const char CSnprintf::digits[201] =
"0001020304050607080910111213141516171819"
"2021222324252627282930313233343536373839"
"4041424344454647484950515253545556575859"
"6061626364656667686970717273747576777879"
"8081828384858687888990919293949596979899";
unsigned int CSnprintf::GetNumberLength(unsigned char nValue,PrintType type)
{
	if (enmPrintFlag_Decimal != type)
	{
		if(nValue > 0x0F)
			return 2;
		return 1;
	} 
	if(nValue > 99)
		return 3;
	else if(nValue > 9)
		return 2;
	return 1;
}
unsigned int CSnprintf::GetNumberLength(unsigned short nValue,PrintType type)
{
	if (enmPrintFlag_Decimal != type)
	{
		if (nValue > 0x00FF)
		{
			if(nValue > 0x0FFF)
				return 4;
			return 3;
		}
		else if (nValue > 0x000F)
			return 2;
		return 1;
	} 
	if (nValue > 99)
	{
		if(nValue > 999)
		{
			if(nValue > 9999)
				return 5;
			return 4;
		}
		return 3;	
	}
	else if(nValue > 9)
		return 2;
	return 1;
}
unsigned int CSnprintf::GetNumberLength(unsigned int nValue,PrintType type)
{
	if (enmPrintFlag_Decimal != type)
	{
		if (nValue >= 0x00010000)
		{
			if (nValue >= 0x01000000 )
			{
				if(nValue >= 0x10000000)return 8;
				return 7;	
			}
			else if(nValue >= 0x00100000)return 6;
			return 5;	
		}
		else if (nValue >= 0x00000100)
		{
			if(nValue >= 0x00001000)return 4;
			return 3;	
		}
		else if(nValue >= 0x00000010)return 2;
		return 1;
	} 
	if (nValue > 99999999)
	{
		if (nValue > 999999999)return 10;
		return 9;	
	}
	else if (nValue > 9999)
	{
		if (nValue > 999999)
		{
			if(nValue > 9999999)return 8;
			return 7;	
		}
		else if(nValue > 99999)return 6;
		return 5;	
	}
	else if (nValue > 99)
	{
		if(nValue > 999)return 4;
		return 3;	
	}
	else if(nValue > 9)return 2;
	return 1;
}
unsigned int CSnprintf::GetNumberLength(unsigned long long nValue,PrintType type)
{
	if (enmPrintFlag_Decimal != type)
	{
		if (nValue >= 0x0000000100000000)
		{
			if(nValue >= 0x0001000000000000)
			{
				if(nValue >= 0x0100000000000000)
				{
					if(nValue >= 0x1000000000000000)return 16;
					return 15;	
				}
				else if(nValue >= 0x0010000000000000)return 14;
				return 13;	
			}
			else if (nValue >= 0x0000010000000000)
			{
				if (nValue >= 0x0000100000000000 )return 12;
				return 11;	
			}
			else if (nValue >= 0x0000001000000000)return 10;
			return 9;
		}
		else if(nValue >= 0x0000000000010000)
		{
			if (nValue >= 0x0000000001000000)
			{
				if (nValue >= 0x00000000010000000)return 8;
				return 7;	
			}
			else if(nValue >= 0x0000000000100000)return 6;
			return 5; 	
		}
		else if(nValue >= 0x0000000000000100)
		{
			if(nValue >= 0x0000000000001000 )return 4;
			return 3;
		}
		else if(nValue >= 0x0000000000000010)return 2;
		return 1;
	} 
	if (nValue > 9999999999999999)
	{
		if (nValue > 999999999999999999)
		{
			if(nValue > 9999999999999999999u)return 20;
			return 19;
		}
		else if(nValue > 99999999999999999)return 18;
		return 17;	
	}
	else if(nValue > 99999999)
	{
		if (nValue > 999999999999)
		{
			if (nValue > 99999999999999)
			{
				if(nValue > 999999999999999)return 16;
				return 15;	
			}
			else if (nValue > 9999999999999)return 14;
			return 13;	
		}
		else if(nValue > 9999999999)
		{
			if(nValue > 99999999999)return 12;
			return 11;	
		}
		else if(nValue > 999999999)return 10;
		return 9;
	}
	else if (nValue > 9999)
	{
		if (nValue > 999999)
		{
			if(nValue > 9999999)return 8;
			return 7;	
		}
		else if(nValue > 99999)return 6;
		return 5;	
	}
	else if (nValue > 99)
	{
		if(nValue > 999)return 4;
		return 3;	
	}
	else if(nValue > 9)return 2;
	return 1;
}
int CSnprintf::Snprintf(char *pBuf,unsigned int nMaxSize,const char *pFormat,...)
{
	va_list ap;
	va_start(ap,pFormat);
	int nSize = Snprintf(pBuf,nMaxSize-1,pFormat,ap);
	va_end(ap);
	if(likely(nSize > 0))
		pBuf[nSize]='\0';
	return nSize;
}
int CSnprintf::Snprintf(char *pBuf,unsigned int nMaxSize,const char *pFormat,va_list ap)
{
	const char *pBegin = pFormat;
	unsigned int nRet = 0;
	 while(*pFormat != '\0')
	 {
		 if ('%' != *pFormat)
		 {
			 ++pFormat;
			 continue;
		 }
		 int nLen = 0;
		 if (pFormat != pBegin)
		 {
			 nLen = int(pFormat - pBegin);
			 nRet += nLen;
			 if(nRet > nMaxSize)
				 return -1;
			 memcpy(pBuf,pBegin,nLen);
			 pBuf += nLen;
		 }
		 ++pFormat;//跳过%字符
		 switch(*pFormat)
		 {
		 case '%':
			 if(nRet == nMaxSize)
				 return -1;
			 *pBuf = '%';
			 ++pBuf;
			 ++nRet;
			 break;
		 case 'c':
			 {
				 if(nRet == nMaxSize)
					 return -1;
				 *pBuf= static_cast<char>(va_arg(ap,int));
				 ++pBuf;
				 ++nRet;
				 break;
			 }
		 case 's':
			 {
				 char *arg = va_arg(ap,char*);
				 nLen = (int)strlen(arg);
				 if (likely(nLen > 0))
				 {
					 nRet += nLen;
					 if(nRet > nMaxSize)
						 return -1;
					 memcpy(pBuf,arg,nLen);
					 pBuf += nLen;
				 }
				 break;
			 }
		 case 'b':
			 {
				 char * arg = va_arg(ap,char*);
				 nLen  = va_arg(ap,int);
				 if (likely(nLen > 0))
				 {
					 nRet += nLen;
					 if(nRet > nMaxSize)
						 return -1;
					 memcpy(pBuf,arg,nLen);
					 pBuf += nLen;
				 }
				 break;
			 }
		 default://数字
			 {
				 if(unlikely(nRet == nMaxSize))
					 return -1;
				 unsigned int nFlag = 0;
				 pBegin = pFormat;
				 if ('0' == *pFormat)
				 {
					 nFlag |= enmPrintFlag_Zero;
					 ++pFormat;
				 }
				 unsigned int nWidth = 0;//宽度
				 while (*pFormat >= '0' && *pFormat <= '9')
				 {
					 nWidth = nWidth*10+(*pFormat - '0');
					 ++pFormat;
				 }
				 unsigned int nPrecision = 0;
				 if ('.' == *pFormat)//精度
				 {
					 ++pFormat;
					 while (*pFormat >= '0' && *pFormat <= '9')
					 {
						 nPrecision = nPrecision*10+(*pFormat - '0');
						 ++pFormat;
					 }
					 if('*' == *pFormat)
					 {
						 nPrecision = va_arg(ap,unsigned int);
						 ++pFormat;
					 }
				 }
				 switch(*pFormat)
				 {
				 case 's':
					 {
						 char * arg = va_arg(ap,char*);
						 nLen  = nPrecision;
						 if (likely(nLen > 0))
						 {
							 if(nRet + nLen > nMaxSize)
								 return -1;
							 memcpy(pBuf,arg,nLen);
						 }
						 break;
					 }
				 case 'h':
					 {
						 ++pFormat;
						 if (*pFormat == 'h')
						 {
							 ++pFormat;
							 unsigned char nValue = va_arg(ap,int);
							 switch(*pFormat)
							 {
							 case 'u':
								 nFlag |= enmPrintFlag_Decimal;
								 nLen = FormatValue(pBuf,nMaxSize-nRet,nValue,nFlag,nWidth);
								 break;
							 case 'X':
								 nFlag |= enmPrintFlag_Hex_B;
								 nLen = FormatValue(pBuf,nMaxSize-nRet,nValue,nFlag,nWidth);
								 break;
							 case 'x':
								 nFlag |= enmPrintFlag_Hex_L;
								 nLen = FormatValue(pBuf,nMaxSize-nRet,nValue,nFlag,nWidth);
								 break;
							 case 'd':
								 nFlag |= enmPrintFlag_Decimal;
								 nLen = FormatValue(pBuf,nMaxSize-nRet,static_cast<char>(nValue),nFlag,nWidth);
								 break;
							 default:
								 return -2;
							 }//switch(h)
						 }
						 else
						 {
							 unsigned short nValue = va_arg(ap,int);
							 switch(*pFormat)
							 {
							 case 'u':
								 nFlag |= enmPrintFlag_Decimal;
								 nLen = FormatValue(pBuf,nMaxSize-nRet,nValue,nFlag,nWidth);
								 break;
							 case 'X':
								 nFlag |= enmPrintFlag_Hex_B;
								 nLen = FormatValue(pBuf,nMaxSize-nRet,nValue,nFlag,nWidth);
								 break;
							 case 'x':
								 nFlag |= enmPrintFlag_Hex_L;
								 nLen = FormatValue(pBuf,nMaxSize-nRet,nValue,nFlag,nWidth);
								 break;
							 case 'd':
								 nFlag |= enmPrintFlag_Decimal;
								 nLen = FormatValue(pBuf,nMaxSize-nRet,static_cast<short>(nValue),nFlag,nWidth);
								 break;
							 default:
								 return -2;
							 }
						 }
						 break;
					 }//case h
				 case 'l':
					 {
						 ++pFormat;
						 if(*pFormat == 'l')
							 ++pFormat;
						 else//64位系统sizeof(long)==sizeof(long long)=8 32为中sizeof(long)=sizeof(int)=4
						 {
#ifndef _64_PLATFORM_
							 return -2;
#endif
						 }
						 unsigned long long nValue = va_arg(ap,unsigned long long);
						 switch(*pFormat)
						 {
						 case 'u':
							 nFlag |= enmPrintFlag_Decimal;
							 nLen = FormatValue(pBuf,nMaxSize-nRet,nValue,nFlag,nWidth);
							 break;
						 case 'X':
							 nFlag |= enmPrintFlag_Hex_B;
							 nLen = FormatValue(pBuf,nMaxSize-nRet,nValue,nFlag,nWidth);
							 break;
						 case 'x':
							 nFlag |= enmPrintFlag_Hex_L;
							 nLen = FormatValue(pBuf,nMaxSize-nRet,nValue,nFlag,nWidth);
							 break;
						 case 'd':
							 nFlag |= enmPrintFlag_Decimal;
							 nLen = FormatValue(pBuf,nMaxSize-nRet,static_cast<long long>(nValue),nFlag,nWidth);
							 break;
						 default:
							 return -2;
						 }
						 break;
					 }//case l
				 case 'u':
					 {
						 unsigned int nValue = va_arg(ap,unsigned int);
						 nFlag |= enmPrintFlag_Decimal;
						 nLen = FormatValue(pBuf,nMaxSize-nRet,nValue,nFlag,nWidth);
						 break;
					 }
				 case 'X':
					 {
						 unsigned int nValue = va_arg(ap,unsigned int);
						 nFlag |= enmPrintFlag_Hex_B;
						 nLen = FormatValue(pBuf,nMaxSize-nRet,nValue,nFlag,nWidth);
						 break;
					 }
				 case 'x':
					 {
						 unsigned int nValue = va_arg(ap,unsigned int);
						 nFlag |= enmPrintFlag_Hex_L;
						 nLen = FormatValue(pBuf,nMaxSize-nRet,nValue,nFlag,nWidth);
						 break;
					 }
				 case 'd':
					 {
						 int nValue = va_arg(ap, int);
						 nFlag |= enmPrintFlag_Decimal;
						 nLen = FormatValue(pBuf,nMaxSize-nRet,nValue,nFlag,nWidth);
						 break;
					 }
				 case 'f':
					 {
						 double nValue = va_arg(ap,double);
						 char sztmp[16]={0};
						 memcpy(sztmp,pBegin-1,pFormat-pBegin+2);
#ifdef WIN32
						 nLen = _snprintf_s(pBuf,nMaxSize-nRet,_TRUNCATE,sztmp,nValue);
#else
						 nLen = snprintf(pBuf,nMaxSize-nRet,sztmp,nValue);
						 if(static_cast<uint32_t>(nLen) > nMaxSize-nRet)
							 nLen = -1;
#endif
						 break;
					 }
				 case 'z':
					 {
						 nFlag |= enmPrintFlag_Decimal;
#ifndef _64_PLATFORM_
						 unsigned int nValue = va_arg(ap,unsigned int);
						 nLen = FormatValue(pBuf,nMaxSize-nRet,nValue,nFlag,nWidth);
#else
						 unsigned long long nValue = va_arg(ap,unsigned long long);
						 nLen = FormatValue(pBuf,nMaxSize-nRet,nValue,nFlag,nWidth);	
#endif
						 break;
					 }
				 case 't'://time_t=long
					 {
						 nFlag |= enmPrintFlag_Decimal;
#ifndef _64_PLATFORM_
						 int nValue = va_arg(ap, int);
						 nLen = FormatValue(pBuf,nMaxSize-nRet,nValue,nFlag,nWidth);
#else
						 long long nValue = va_arg(ap, long long);
						 nLen = FormatValue(pBuf,nMaxSize-nRet,nValue,nFlag,nWidth);	
#endif
						 break;
					 }
				 case 'p':
					 {
						 nFlag |= enmPrintFlag_Hex_B|enmPrintFlag_Zero;
#ifndef _64_PLATFORM_
						 if(0 == nWidth)
							 nWidth = 8;
						 unsigned int nValue = va_arg(ap,unsigned int);
						 nLen = FormatValue(pBuf,nMaxSize-nRet,nValue,nFlag,nWidth);
#else
						 if(0 == nWidth)
							 nWidth = 16;
						 unsigned long long nValue = va_arg(ap,unsigned long long);
						 nLen = FormatValue(pBuf,nMaxSize-nRet,nValue,nFlag,nWidth);	
#endif
						 break;
					 }
				 default:
					 return -2;
				 }//switch 数字
				 if(nLen < 0)
					 return -1;
				 nRet += nLen;
				 pBuf += nLen;
				 break;
			 }//default
		 }//switch
		 ++pFormat;
		 pBegin = pFormat;
	 }//while
	 if (pFormat != pBegin)
	 {
		 int nLen =int( pFormat - pBegin);
		 nRet += nLen;
		 if(nRet > nMaxSize)
			 return -1;
		 memcpy(pBuf,pBegin,nLen);
	 }
	 return nRet;
}
 int CSnprintf::FormatValue(char * pBuf,unsigned int nMaxSize, char nValue,unsigned int nFlag,unsigned int nWidth)
 {
	 if (nValue < 0)
	 {
		 *pBuf = '-';
		 int nLen = FormatValue(pBuf+1,nMaxSize-1,static_cast<unsigned char>(-nValue),nFlag,nWidth);
		 if(nLen < 0)
			 return nLen;
		 return nLen+1;
	 }
	return FormatValue(pBuf,nMaxSize,static_cast<unsigned char>(nValue),nFlag,nWidth);
 }
 int CSnprintf::FormatValue(char * pBuf,unsigned int nMaxSize,unsigned char nValue,unsigned int nFlag,unsigned int nWidth)
 {
	 unsigned int nLen = 0;
	 int j = 0;
	 if (nFlag & (enmPrintFlag_Hex_L|enmPrintFlag_Hex_B))
	 {
		 if (nWidth >= 2)
			 nLen = nWidth;
		 else
			 nLen = GetNumberLength(nValue,enmPrintFlag_Hex_B);
		 if (nLen > nMaxSize)
			 return -1;
		 const char *pUse = hexUp;
		 if(nFlag & enmPrintFlag_Hex_L)
			 pUse = HexLow;
		 j = nLen - 1;
		 do 
		 {
			 pBuf[j--] = pUse[nValue&0xF];
			 nValue >>= 4;
		 } while (nValue > 0);	
	 }
	 else
	 {
		 if(nWidth >= 3)
			 nLen = nWidth;
		 else
		 {
			 nLen = GetNumberLength(nValue,enmPrintFlag_Decimal);
			 if(nWidth > nLen)
				 nLen = nWidth;
		 }
		 if (nLen > nMaxSize)
			 return -1;
		 j = nLen -1;
		 while (nValue >= 100) {
			 unsigned int i = (nValue % 100) << 1;
			 nValue /= 100;
			 pBuf[j--] = digits[i+1];
			 pBuf[j--] = digits[i];
		 }
		 if (nValue < 10) {
			 pBuf[j--] = '0' + static_cast<unsigned int>(nValue);
		 } else {
			 unsigned int i = static_cast<unsigned int>(nValue) << 1;
			 pBuf[j--] = digits[i+1];
			 pBuf[j--] = digits[i];
		 }
	 }
	 if (j >= 0)
	 {
		 char ch =' ';
		 if(nFlag &enmPrintFlag_Zero)
			 ch='0';
		 memset(pBuf,ch, uint64_t(j)+1);
	 }
	 return nLen;
 }
 int CSnprintf::FormatValue(char * pBuf,unsigned int nMaxSize, short nValue,unsigned int nFlag,unsigned int nWidth)
 {
	 if (nValue < 0)
	 {
		 *pBuf = '-';
		 int nLen = FormatValue(pBuf+1,nMaxSize-1,static_cast<unsigned short>(-nValue),nFlag,nWidth);
		 if(nLen < 0)
			 return nLen;
		 return nLen+1;
	 }
	 return FormatValue(pBuf,nMaxSize,static_cast<unsigned short>(nValue),nFlag,nWidth);
 }
 int CSnprintf::FormatValue(char * pBuf,unsigned int nMaxSize, unsigned short nValue,unsigned int nFlag,unsigned int nWidth)
 {
	 unsigned int nLen = 0;
	 int j = 0;
	 if (nFlag &(enmPrintFlag_Hex_L|enmPrintFlag_Hex_B))
	 {
		 if(nWidth >= 4)
			 nLen = 4;
		 else
		 {
			 nLen = GetNumberLength(nValue,enmPrintFlag_Hex_B);
			 if(nWidth > nLen)
				 nLen = nWidth;
		 }
		 if(nLen > nMaxSize)
			 return -1;
		 const char *pUse = hexUp;
		 if(nFlag &enmPrintFlag_Hex_L)
			 pUse = HexLow;
		 j = nLen - 1;
		 do 
		 {
			 pBuf[j--] = pUse[nValue&0xF];
			 nValue >>= 4;
		 } while (nValue > 0);	
	 }
	 else
	 {
		 if(nWidth >= 5)
			 nLen = 5;
		 else
		 {
			 nLen = GetNumberLength(nValue,enmPrintFlag_Decimal);
			 if(nWidth > nLen)
				 nLen = nWidth;
		 }
		 if(nLen > nMaxSize)
			 return -1;
		 j = nLen -1;
		 while (nValue >= 100) {
			 unsigned int i = (nValue % 100) << 1;
			 nValue /= 100;
			 pBuf[j--] = digits[i+1];
			 pBuf[j--] = digits[i];
		 }
		 if (nValue < 10) {
			 pBuf[j--] = '0' + static_cast<unsigned int>(nValue);
		 } else {
			 unsigned int i = static_cast<unsigned int>(nValue) << 1;
			 pBuf[j--] = digits[i+1];
			 pBuf[j--] = digits[i];
		 }
	 }
	 if (j >= 0)
	 {
		 char ch =' ';
		 if(nFlag &enmPrintFlag_Zero)
			 ch='0';
		 memset(pBuf,ch, uint64_t(j)+1);
	 }
	 return nLen;
 }
 int CSnprintf::FormatValue(char * pBuf,unsigned int nMaxSize, int nValue,unsigned int nFlag,unsigned int nWidth)
 {
	 if (nValue < 0)
	 {
		 *pBuf = '-';
		 int nLen = FormatValue(pBuf+1,nMaxSize-1,static_cast<unsigned int>(-nValue),nFlag,nWidth);
		 if(nLen < 0)
			 return nLen;
		 return nLen+1;
	 }
	 return FormatValue(pBuf,nMaxSize,static_cast<unsigned int>(nValue),nFlag,nWidth);
 }
 int CSnprintf::FormatValue(char * pBuf,unsigned int nMaxSize, unsigned int nValue,unsigned int nFlag,unsigned int nWidth)
 {
	 unsigned int nLen = 0;
	 int j = 0;
	 if (nFlag &(enmPrintFlag_Hex_L|enmPrintFlag_Hex_B))
	 {
		 if(nWidth >= 8)
			 nLen = 8;
		 else
		 {
			 nLen = GetNumberLength(nValue,enmPrintFlag_Hex_B);
			 if(nWidth > nLen)
				 nLen = nWidth;
		 }
		 if(nLen > nMaxSize)
			 return -1;
		 const char *pUse = hexUp;
		 if(nFlag &enmPrintFlag_Hex_L)
			 pUse = HexLow;
		 j = nLen - 1;
		 do 
		 {
			 pBuf[j--] = pUse[nValue&0xF];
			 nValue >>= 4;
		 } while (nValue > 0);	
	 }
	 else
	 {
		 if(nWidth >= 10)
			 nLen = 10;
		 else
		 {
			 nLen = GetNumberLength(nValue,enmPrintFlag_Decimal);
			 if(nWidth > nLen)
				 nLen = nWidth;
		 }
		 if(nLen > nMaxSize)
			 return -1;
		 j = nLen -1;
		 while (nValue >= 100) {
			 unsigned int i = (nValue % 100) << 1;
			 nValue /= 100;
			 pBuf[j--] = digits[i+1];
			 pBuf[j--] = digits[i];
		 }
		 if (nValue < 10) {
			 pBuf[j--] = '0' + static_cast<unsigned int>(nValue);
		 } else {
			 unsigned int i = static_cast<unsigned int>(nValue) << 1;
			 pBuf[j--] = digits[i+1];
			 pBuf[j--] = digits[i];
		 }
	 }
	 if (j >= 0)
	 {
		 char ch =' ';
		 if(nFlag &enmPrintFlag_Zero)
			 ch='0';
		 memset(pBuf,ch, uint64_t(j)+1);
	 }
	 return nLen;
 }
 int CSnprintf::FormatValue(char * pBuf,unsigned int nMaxSize, long long  nValue,unsigned int nFlag,unsigned int nWidth)
 {
	 if (nValue < 0)
	 {
		 *pBuf = '-';
		 int nLen = FormatValue(pBuf+1,nMaxSize-1,static_cast<unsigned long long>(-nValue),nFlag,nWidth);
		 if(nLen < 0)
			 return nLen;
		 return nLen+1;
	 }
	 return FormatValue(pBuf,nMaxSize,static_cast<unsigned long long>(nValue),nFlag,nWidth);
 }
 int CSnprintf::FormatValue(char * pBuf,unsigned int nMaxSize, unsigned long long nValue,unsigned int nFlag,unsigned int nWidth)
 {
	 unsigned int nLen = 0;
	 int j = 0;
	 if (nFlag &(enmPrintFlag_Hex_L|enmPrintFlag_Hex_B))
	 {
		 if(nWidth >= 16)
			 nLen = 16;
		 else
		 {
			 nLen = GetNumberLength(nValue,enmPrintFlag_Hex_B);
			 if(nWidth > nLen)
				 nLen = nWidth;
		 }
		 if(nLen > nMaxSize)
			 return -1;
		 const char *pUse = hexUp;
		 if(nFlag &enmPrintFlag_Hex_L)
			 pUse = HexLow;
		 j = nLen - 1;
		 do 
		 {
			 pBuf[j--] = pUse[nValue&0xF];
			 nValue >>= 4;
		 } while (nValue > 0);	
	 }
	 else
	 {
		 if(nWidth >= 20)
			 nLen = 20;
		 else
		 {
			 nLen = GetNumberLength(nValue,enmPrintFlag_Decimal);
			 if(nWidth > nLen)
				 nLen = nWidth;
		 }
		 if(nLen > nMaxSize)
			 return -1;
		 j = nLen -1;
		 while (nValue >= 100) {
			 unsigned int i = (nValue % 100) << 1;
			 nValue /= 100;
			 pBuf[j--] = digits[i+1];
			 pBuf[j--] = digits[i];
		 }
		 if (nValue < 10) {
			 pBuf[j--] = '0' + static_cast<unsigned int>(nValue);
		 } else {
			 unsigned int i = static_cast<unsigned int>(nValue) << 1;
			 pBuf[j--] = digits[i+1];
			 pBuf[j--] = digits[i];
		 }
	 }
	 if (j >= 0)
	 {
		 char ch =' ';
		 if(nFlag &enmPrintFlag_Zero)
			 ch='0';
		 memset(pBuf,ch, uint64_t(j)+1);
	 }
	 return nLen;
 }
 int CSnprintf::SnprintfEX(char *pBuf,unsigned int nMaxSize,const char *pFormat,...)
 {
	 va_list ap;
	 va_start(ap,pFormat);
	 int nSize = SnprintfEX(pBuf,nMaxSize-1,pFormat,ap);
	 va_end(ap);
	 if(likely(nSize > 0))
		 pBuf[nSize]='\0';
	 return nSize;
 }
 int CSnprintf::SnprintfEX(char *pBuf,unsigned int nMaxSize,const char *pFormat,va_list ap)
 {
	 const char *pBegin = pFormat;
	 unsigned int nRet = 0;
	 while(*pFormat != '\0')
	 {
		 if ('%' != *pFormat)
		 {
			 ++pFormat;
			 continue;
		 }
		 int nLen = int(pFormat - pBegin);
		 if (nLen > 0)
		 {
			 nRet += nLen;
			 if(nRet > nMaxSize)
				 return -1;
			 memcpy(pBuf,pBegin,nLen);
			 pBuf += nLen;
		 }
		 ++pFormat;
		 switch(*pFormat)
		 {
		 case '%':
			 if(nRet == nMaxSize)
				 return -1;
			 *pBuf = '%';
			 ++pBuf;
			 nRet++;
			 break;
		 case 'c':
			 {
				 if(nRet == nMaxSize)
					 return -1;
				 *pBuf = static_cast<char>(va_arg(ap,int));
				 ++pBuf;
				 nRet++;
				 break;
			 }
		 case 's':
			 {
				 char *arg = va_arg(ap,char*);
				 nLen = (int)strlen(arg);
				 if (likely(nLen > 0))
				 {
					 nRet += nLen;
					 if(nRet > nMaxSize)
						 return -1;
					 memcpy(pBuf,arg,nLen);
					 pBuf += nLen;
				 }
				 break;
			 }
		 case 'b':
			 {
				 char * arg = va_arg(ap,char*);
				 nLen  = va_arg(ap,int);
				 if (likely(nLen > 0))
				 {
					 nRet += nLen;
					 if(nRet > nMaxSize)
						 return -1;
					 memcpy(pBuf,arg,nLen);
					 pBuf += nLen;
				 }
				 break;
			 }
		 default://数字
			 {
				 if(unlikely(nRet == nMaxSize))
					 return -1;
				 pBegin = pFormat;
				 unsigned int nFlag = 0;
				 if ('0' == *pFormat)
				 {
					 nFlag |= enmPrintFlag_Zero;
					 ++pFormat;
				 }
				 unsigned int nWidth = 0;//宽度
				 while (*pFormat >= '0' && *pFormat <= '9')
				 {
					 nWidth = nWidth*10+(*pFormat - '0');
					 ++pFormat;
				 }
				 unsigned int nPrecision = 0;
				 if ('.' == *pFormat)//精度
				 {
					 ++pFormat;
					 while (*pFormat >= '0' && *pFormat <= '9')
					 {
						 nPrecision = nPrecision*10+(*pFormat - '0');
						 ++pFormat;
					 }
					 if('*' == *pFormat)
					 {
						 nPrecision = va_arg(ap,unsigned int);
						 ++pFormat;
					 }
				 }
				 if(*pFormat == 's')//和%b效果一样加入编译器检查%b会有警告宽度不实现
				 {
					 char * arg = va_arg(ap,char*);
					 nLen  = nPrecision;
					 if (likely(nLen > 0))
					 {
						 if(nRet + nLen > nMaxSize)
							 return -1;
						 memcpy(pBuf,arg,nLen);
					 }
				 }
				 else if(*pFormat == 'f')
				 {
					 double nValue = va_arg(ap,double);
					 char sztmp[16]={0};
					 memcpy(sztmp,pBegin-1,pFormat-pBegin+2);
#ifdef WIN32
					 nLen = _snprintf_s(pBuf,nMaxSize-nRet,_TRUNCATE,sztmp,nValue);
#else
					 nLen = snprintf(pBuf,nMaxSize-nRet,sztmp,nValue);
					 if(static_cast<uint32_t>(nLen) > nMaxSize-nRet)
						 nLen = -1;
#endif
				 }
				 else
				 {
					 long long nValue;
					 if (*pFormat == 'l')
					 {
						 ++pFormat;
						 if(*pFormat == 'l')
							 ++pFormat;
						 else//64位系统sizeof(long)==sizeof(long long)=8 32为中sizeof(long)=sizeof(int)=4
						 {
#ifndef _64_PLATFORM_
							 return -2;
#endif
						 }
						 nFlag |= enmPrintFlag_64;
						 nValue = va_arg(ap,long long);
					 }
					 else if(*pFormat == 'h')
					 {
						 ++pFormat;
						 if (*pFormat == 'h')
						 {
							 ++pFormat;
							 nFlag |= enmPrintFlag_8;
							 if('d' == *pFormat)
								 nValue = static_cast<char>(va_arg(ap,int)); 
							 else
								 nValue = static_cast<unsigned char>(va_arg(ap,unsigned int));
								 
						 }
						 else
						 {
							 nFlag |= enmPrintFlag_16;
							 if('d' == *pFormat)
								 nValue = static_cast<short>(va_arg(ap,int));
							 else
								 nValue = static_cast<unsigned short>(va_arg(ap,unsigned int));
								 
						 }
						 
					 }
					 else if(*pFormat == 'z')
					 {
#ifndef _64_PLATFORM_
						 nFlag |= enmPrintFlag_32|enmPrintFlag_Decimal;
						 nValue = va_arg(ap,unsigned int);
#else
						 nFlag |= enmPrintFlag_64|enmPrintFlag_Decimal;
						 nValue = va_arg(ap,unsigned long long);
#endif

					 }
					 else if(*pFormat == 't')//time_t=long
					 {
#ifndef _64_PLATFORM_
						 nFlag |= enmPrintFlag_32|enmPrintFlag_Signed|enmPrintFlag_Decimal;
						 nValue = va_arg(ap, int);
#else
						 nFlag |= enmPrintFlag_64|enmPrintFlag_Signed|enmPrintFlag_Decimal;
						 nValue = va_arg(ap, long long);
#endif
					 }
					 else if(*pFormat == 'p')
					 {
#ifndef _64_PLATFORM_
						 if(0 == nWidth)
							 nWidth = 8;
						 nFlag |= enmPrintFlag_32|enmPrintFlag_Hex_B|enmPrintFlag_Zero;
						 nValue = va_arg(ap,unsigned int);
#else
						 if(0 == nWidth)
							 nWidth = 16;
						 nFlag |= enmPrintFlag_64|enmPrintFlag_Hex_B|enmPrintFlag_Zero;
						 nValue = va_arg(ap,unsigned long long);
#endif
					 }
					 else if (*pFormat == 'd')
					 {
						 nFlag |= enmPrintFlag_32;
						 nValue = va_arg(ap,int);
					 }
					 else if(*pFormat == 'u')
					 {
						 nFlag |= enmPrintFlag_32;
						 nValue = va_arg(ap,unsigned int);
					 }
					 else//不知道的打印格式
						 return -2;
					 if(*pFormat == 'd')//lld hhd hd d
						 nFlag |= enmPrintFlag_Signed|enmPrintFlag_Decimal;
					 else if(*pFormat == 'x')//llx hhx hx x
						 nFlag |= enmPrintFlag_Hex_L;
					 else if(*pFormat == 'X')//llX hhX hX X
						 nFlag |= enmPrintFlag_Hex_B;
					 else if(*pFormat == 'u')//llu hhu hu u
						 nFlag |= enmPrintFlag_Decimal;
					 nLen = FormatValueEX(pBuf,nMaxSize-nRet,nValue,nFlag,nWidth);
				 }//else for != 'f'
				 if(nLen < 0)
					 return nLen;
				 nRet += nLen;
				 pBuf += nLen;
				 break;
			 }//default
		 }//switch
		 ++pFormat;
		 pBegin = pFormat;
	 }//while
	 if (pFormat != pBegin)
	 {
		 int nLen = int(pFormat - pBegin);
		 nRet += nLen;
		 if(nRet > nMaxSize)
			 return -1;
		 memcpy(pBuf,pBegin,nLen);
	 }
	 return nRet;
 }
 int CSnprintf::FormatValueEX(char * pBuf,unsigned int nMaxSize,long long  nValue,unsigned int nFlag,unsigned int nWidth)
 {
	unsigned long long nTemp = nValue;
	unsigned int nLen = 0;
	if((nFlag & enmPrintFlag_Signed) && nValue < 0)
	{
		nTemp = -nValue;
		*pBuf = '-';
		++pBuf;
		nLen = 1;
	}
	unsigned int j  = GetNumberLength(nTemp,nFlag & 0xF0);
	if(nWidth > j)
		j = nWidth;
	nLen += j;
	if (nLen > nMaxSize)
		return -1;
	if (nFlag &(enmPrintFlag_Hex_L|enmPrintFlag_Hex_B))
	{
		 const char *pUse = hexUp;
		 if(nFlag &enmPrintFlag_Hex_L)
			 pUse = HexLow;
		 do 
		 {
			 pBuf[--j] = pUse[nTemp&0xF];
			 nTemp >>= 4;
		 } while (nTemp > 0);		 
	}
	else
	{
		while (nTemp >= 100) {
			unsigned int i = (nTemp % 100) << 1;
			nTemp /= 100;
			pBuf[--j] = digits[i+1];
			pBuf[--j] = digits[i];
		}

		// Handle last 1-2 digits
		if (nTemp < 10) {
			pBuf[--j] = '0' + static_cast<unsigned int>(nTemp);
		} else {
			unsigned int i = static_cast<unsigned int>(nTemp) << 1;
			pBuf[--j] = digits[i+1];
			pBuf[--j] = digits[i];
		}
	}
	if (j > 0)
	{
		char ch =' ';
		if(nFlag &enmPrintFlag_Zero)
			ch='0';
		memset(pBuf,ch,j);
	}
	 return nLen;
 }
 int CSnprintf::FormatValue(char * pBuf,unsigned int nMaxSize, double nValue,unsigned int nFlag,unsigned int nWidth,unsigned int nPrecision)
 {
	 if(0 == nPrecision || nPrecision > 64)
		 nPrecision = 6;
	 //double的最大长度E308
	 const size_t nMaxDoubleLen = 512;
	 char szTempBuf[nMaxDoubleLen + 1]={0};
	 //输出要用的字符串
	 int decimal = 0;
	 int sign = 0;
#ifdef WIN32
	 _fcvt_s(szTempBuf,nMaxDoubleLen,nValue,nPrecision,&decimal,&sign);
#else
	 fcvt_r(nValue,nPrecision,&decimal,&sign,szTempBuf,nMaxDoubleLen);
#endif
	 int nLen = static_cast<int>( strlen(szTempBuf));
	 if (nLen <= 0)
		 return -3;
	 unsigned int nRet = 0;
	 if(sign)
	 {
		 *pBuf = '-';
		 ++pBuf;
		 ++nRet;
	 }
	 if(decimal > 0)//加小数点
		 nRet += nLen+1;
	 else//0.xxx
		 nRet += 2 + nPrecision;
	 if(nRet > nMaxSize)
		 return -1;
	 //整数和小数混合输出
	 if (decimal > 0)
	 {
		 //将整数部分输出
		 int nTemp = nLen-nPrecision;
		 memcpy(pBuf,szTempBuf,nTemp);
		 pBuf[nTemp] = '.';
		 memcpy(&pBuf[nTemp+1],&szTempBuf[nTemp],nPrecision);
	 }
	 //纯小数输出
	 else
	 {
		 pBuf[0] = '0';
		 pBuf[1] = '.';
		 memcpy(&pBuf[2],szTempBuf,nLen);
	 }
	 return nRet;
 }
 void CSnprintf::FcvtEX(long double value,char *pBuf,unsigned int nPrecision)
 {
	 //char *pBegin = pBuf;
	 double d2 = 0.0;
	 double d1= modf(value,&d2);
	 //d1 = value;
	 const char BASE_UPPERCASE_OUTCHAR[] = {"0123456789"};
	 d2 = value;
	 while (d2 > 0.0)
	 {
		 int j = static_cast<int>(fmod(d2,10.0));
		 *pBuf = BASE_UPPERCASE_OUTCHAR[j];
		 ++pBuf;
		 d2 /= 10.0;
		 //modf(d2/10,&d2);
	 }
	 *pBuf = '.';
	 ++pBuf;
	 unsigned int i = 0;
	 while(d1 > 0.0 && i < nPrecision)
	 {
		 d1 *= 10;
		 int j = static_cast<int>(fmod(d1,10.0));
		 *pBuf = BASE_UPPERCASE_OUTCHAR[j];
		 ++pBuf;
		 ++i;
	 }
	 *pBuf='\0';
 }
 std::string CSnprintf::ToStr(long long  nValue)
 {
	 char pBuf[22];
	 char*pEnd = pBuf+22;
	 if (nValue < 0)
	 {
		 char* pTmp = ToStr((unsigned long long)(-nValue),pEnd);
		 *--pTmp='-';
		 return std::string(pTmp,pEnd-pTmp);
	 }
	 char* pTmp = ToStr(nValue,pEnd);
	 return std::string(pTmp,pEnd-pTmp);
 }
 std::string CSnprintf::ToStr(unsigned long long nValue)
 {
	 char pBuf[22];
	 char*pEnd = pBuf+22;
	 char* pTmp = ToStr(nValue,pEnd);
	 return std::string(pTmp,pEnd-pTmp);
 }
 char* CSnprintf::ToStr(unsigned long long nValue,char *pBuf)
 {
	 while (nValue >= 100) {
		 unsigned int i = (nValue % 100) << 1;
		 nValue /= 100;
		 *--pBuf = digits[i+1];
		 *--pBuf = digits[i];
	 }
	 if (nValue < 10) {
		*--pBuf = '0' + static_cast<unsigned int>(nValue);
	 } else {
		 unsigned int i = static_cast<unsigned int>(nValue) << 1;
		 *--pBuf = digits[i+1];
		 *--pBuf = digits[i];
	 }
	 return pBuf;
 }
#ifndef WIN32
 int test()
 {
	 char szstr[1024]= {0};
	 int value = 132;
	 unsigned int NUM = 1000000;
	 timeval begin;
	 timeval end ;
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::Snprintf(szstr,1024,"0x%s","test");
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::SnprintfEX(szstr,1024,"0x%s","test");
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 snprintf(szstr,1024,"0x%s","test");
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::Snprintf(szstr,1024,"0x%d",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::SnprintfEX(szstr,1024,"0x%d",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 snprintf(szstr,1024,"0x%d",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);

	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::Snprintf(szstr,1024,"0x%u",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::SnprintfEX(szstr,1024,"0x%u",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 snprintf(szstr,1024,"0x%u",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);

	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::Snprintf(szstr,1024,"0x%x",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::SnprintfEX(szstr,1024,"0x%x",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 snprintf(szstr,1024,"0x%x",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 //
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::Snprintf(szstr,1024,"0x%hhd",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::SnprintfEX(szstr,1024,"0x%hhd",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 snprintf(szstr,1024,"0x%d",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);

	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::Snprintf(szstr,1024,"0x%hhu",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::SnprintfEX(szstr,1024,"0x%hhu",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 snprintf(szstr,1024,"0x%d",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);

	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::Snprintf(szstr,1024,"0x%hhx",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::SnprintfEX(szstr,1024,"0x%hhx",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 snprintf(szstr,1024,"0x%x",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 //
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::Snprintf(szstr,1024,"0x%hd",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::SnprintfEX(szstr,1024,"0x%hd",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 snprintf(szstr,1024,"0x%d",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);

	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::Snprintf(szstr,1024,"0x%hu",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::SnprintfEX(szstr,1024,"0x%hu",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 snprintf(szstr,1024,"0x%d",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);

	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::Snprintf(szstr,1024,"0x%hx",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::SnprintfEX(szstr,1024,"0x%hx",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 snprintf(szstr,1024,"0x%x",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);

	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::Snprintf(szstr,1024,"0x%lld",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::SnprintfEX(szstr,1024,"0x%lld",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 snprintf(szstr,1024,"0x%d",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::Snprintf(szstr,1024,"0x%llu",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::SnprintfEX(szstr,1024,"0x%llu",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 snprintf(szstr,1024,"0x%d",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::Snprintf(szstr,1024,"0x%llx",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 CSnprintf::SnprintfEX(szstr,1024,"0x%llx",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 gettimeofday(&begin,NULL);
	 for (unsigned int i = 0; i < NUM;++i)
		 snprintf(szstr,1024,"0x%x",value);
	 gettimeofday(&end,NULL);
	 printf("%s:%ld\n",szstr,(end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
	 return 0;
 };
#endif
