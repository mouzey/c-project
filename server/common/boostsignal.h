#ifndef _SIGNAL_H_
#define _SIGNAL_H_
#include "recvsignal.h"
#include "sendsignal.h"
#include "signaltype.h"
class CSignal:public CRecvSignal,public CSendSignal
{
public:
	CSignal(uint32_t nSignalNum):CSendSignal(nSignalNum){}
	CSignal():CSendSignal(0){}
	virtual ~CSignal(){}
};
#endif