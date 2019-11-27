#ifndef _H_NO_COPY_
#define _H_NO_COPY_
//不可复制的类型需要继承本类
class NoCopy
{
protected:
	NoCopy(){};
	~NoCopy(){};
private:
	NoCopy(const NoCopy&);
	const NoCopy &operator=(const NoCopy&);
};
#endif