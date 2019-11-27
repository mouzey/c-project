#include "pch.h"
#include "codebase.h"
char CCode::m_szBuff[MAXCODEBUFFSIZE];
const char* CCode::m_pBuff = nullptr;
uint32_t CCode::m_nSize = 0;
uint32_t CCode::m_nReadSize = 0;
bool CCode::m_error = false;