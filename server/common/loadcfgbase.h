#pragma once
#include<unordered_map>
#include<vector>
#include<map>
#include "define.h"
#include "readcsv.h"
#include "singleton.h"
#include "log.h"
//≈‰÷√º”‘ÿª˘¿‡
template<typename T, typename MapType = std::unordered_map<uint32_t, const T*>>
class CBaseLoader
{
private:
	template <class M>
	struct IsVector
	{
		enum { value = false, };
		typedef typename M::key_type	KeyType;
		typedef typename M::mapped_type ValType;
	};
	template <class M>
	struct IsVector<std::vector<M>>
	{
		enum { value = true, };
		typedef uint32_t KeyType;
		typedef  M ValType;
	};
	template <class M> struct IsMultiMap { enum { value = false, }; };
	template <class K, class V>struct IsMultiMap<std::multimap<K, V>> { enum { value = true, }; };
	template <class K, class V> struct IsMultiMap<std::unordered_multimap<K, V>> { enum { value = true, }; };
	template<class M>
	inline std::enable_if_t<IsVector<M>::value, typename IsVector<M>::ValType> FindCfg(typename IsVector<M>::KeyType key)const
	{
		return m_vecMap.empty() || key >= m_vecMap.back().size() ? nullptr : m_vecMap.back()[key];
	}
	template<class M>
	inline std::enable_if_t<!IsVector<M>::value, typename IsVector<M>::ValType> FindCfg(typename IsVector<M>::KeyType key)const
	{
		if (m_vecMap.empty()) return nullptr;
		auto iter = m_vecMap.back().find(key);
		return iter != m_vecMap.back().end() ? iter->second : nullptr;
	}
	template<class M>
	inline std::enable_if_t<IsVector<M>::value, bool> Insert(typename IsVector<M>::ValType pVal)
	{
		auto pMap = &m_vecMap.back();
		if ((size_t)pVal->GetId() < pMap->size() && pMap->at(pVal->GetId()) != nullptr)
			return false;
		if (pMap->size() <= (size_t)pVal->GetId())
			pMap->resize(pVal->GetId() + 1, nullptr);
		pMap->at(pVal->GetId()) = pVal;
		return true;
	}
	template<class M >
	inline std::enable_if_t<IsMultiMap<M>::value, bool> Insert(typename IsVector<M>::ValType pVal)
	{
		m_vecMap.back().emplace(pVal->GetId(), pVal);
		return true;
	}
	template<class M >
	inline std::enable_if_t<!IsVector<M>::value && !IsMultiMap<M>::value, bool> Insert(typename IsVector<M>::ValType pVal)
	{
		return m_vecMap.back().emplace(pVal->GetId(), pVal).second;
	}
	template<class M>
	inline std::enable_if_t<IsVector<M>::value, void> DelCfg(M& map)
	{
		for (auto& iter : map)
			delete iter;
	}
	template<class M>
	inline std::enable_if_t<!IsVector<M>::value, void> DelCfg(M& map)
	{
		for (auto& iter : map)
			delete iter.second;
	}
public:
	typedef T ConfigValueType;
	CBaseLoader() {}
	virtual ~CBaseLoader()
	{
		for (auto& iter : m_vecMap)
			DelCfg<MapType>(iter);
	}
	bool Load(const std::string& str)
	{
		m_vecMap.emplace_back();
		auto& csv = CSingleton<CReadCsv>::GetInstance();
		if(!csv.OpenFile(str) || !csv.ParseHead(T::GetCol()))
			return false;
		while (csv.FetchRow())
		{
			T* p = new T;
			p->ReadData(csv);
			if (!Parse(p))
			{
				WRITE_ERROR_LOG("parse data has error row=%u id=%s", csv.GetRow(), ToStr(p->GetId()).data());
				return false;
			}
			if (!Insert<MapType>(p))
			{
				WRITE_ERROR_LOG("insert data has error row=%u id=%s", csv.GetRow(), ToStr(p->GetId()).data());
				return false;
			}
		}
		return true;
	}
	bool Load(std::string&& str) { return Load(str); }
	const T* FindCfg(typename IsVector<MapType>::KeyType key)const { return FindCfg<MapType>(key); }
	virtual bool Parse(T* pValue) { return true; }
	const MapType* GetCurrMap()const { return m_vecMap.empty() ? nullptr : &m_vecMap.back(); }
	bool IsReload()const { return !m_vecMap.empty(); }
private:
	std::vector<MapType> m_vecMap;
};
