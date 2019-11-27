#include "pch.h"
#include "msgbase.h"
#include"codebase.h"
 bool CMsgBase::Encode()const
 {
	 if (!CCode::Encode(m_len))return false;
	 if (!CCode::Encode(m_msgId))return false;
	 return true;
 }
 bool CMsgBase::Decode() 
 { 
	 if (!CCode::Decode(m_len))return false;
	 if (!CCode::Decode(m_msgId))return false;
	 return true;
 }