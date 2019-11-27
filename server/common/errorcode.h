#ifndef _H_ERRORCODE_H
#define _H_ERRORCODE_H
#ifdef WIN32
#include <winerror.h>
#endif
//成功
#ifndef S_OK
#define S_OK 0 
#endif
//空指针
#define E_NULLPOINTER -1
//未知错误
#define E_UNKNOW -2
//缓冲区不够
#define E_LESSMEMORY -3
#ifndef WSAEWOULDBLOCK
#define  WSAEWOULDBLOCK E_UNKNOW
#endif
#endif