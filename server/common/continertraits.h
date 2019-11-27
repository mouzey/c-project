#ifndef _CONTINER_TRAITS_H_
#define _CONTINER_TRAITS_H_
#include <type_traits>
//通用的萃取函数
template <typename T> 
class has_mapped_type
{ 
public:
	template <class C> static uint8_t check(typename std::decay<typename C::mapped_type>::type*) { return 0; }
	template <class C> static uint16_t check(...) { return 0; }
	enum { value = (sizeof(check<T>(nullptr))==1) };
};

template <typename T> 
class has_const_iterator
{ 
public:
	template <class C> static uint8_t check(typename std::decay<typename C::const_iterator>::type *) { return 0; }
	template <class C> static uint16_t check(...) { return 0; }
	enum { value = (sizeof(check<T>(nullptr))==1) };
};

template <typename T> 
class has_container_type
{ 
public:
	template <class C> static uint8_t check(typename std::decay<typename C::container_type>::type *) { return 0; }
	template <class C> static uint16_t check(...) { return 0; }
	enum { value = (sizeof(check<T>(nullptr))==1) };
};

template <typename T> 
class has_key_compare
{ 
public:
	template <class C> static uint8_t check(typename std::decay<typename C::key_compare>::type *) { return 0; }
	template <class C> static uint16_t check(...) { return 0; }
	enum { value = (sizeof(check<T>(nullptr))==1) };
};

template <typename T> 
class is_map
{ 
public:
	template <class C> static uint8_t check(typename std::map<typename C::key_type,typename C::mapped_type,typename C::key_compare> *) { return 0; }
	template <class C> static uint16_t check(...) { return 0; }
	enum { value = (sizeof(check<T>(nullptr))==1) };
};
template<typename T>
struct has_member_foo//检查是否有foo函数 用宏来进行替换
{
private:
	template<typename U>
	static std::enable_if_t<std::is_member_function_pointer<decltype(&U::foo)>::value, uint8_t> Check(int) { return 0; }
	template<typename U>
	static uint16_t Check(...) { return 0; }
public:
	enum { value = 1 == sizeof(Check<T>(0)) };
};
#endif