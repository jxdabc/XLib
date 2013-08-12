#pragma once

#include "../../../Lib/include/json/value.h"

#include <windows.h>
#include <atlstr.h>

#include <set>
#include <list>

namespace XLibS
{
	class IJsonSerializable
	{
	public:
		virtual VOID JsonSerialize(Json::Value *) const = 0;
		virtual VOID JsonUnserialize(const Json::Value &) = 0;
	};

	namespace JsonSerializeTool
	{
		VOID JsonSerialize( Json::Value *pJsonValue, const DWORD & n );
		VOID JsonUnserialize( const Json::Value &jvJsonValue, DWORD *n );

		VOID JsonSerialize( Json::Value *pJsonValue, const INT & n );
		VOID JsonUnserialize( const Json::Value &jvJsonValue, INT *n );

		VOID JsonSerialize( Json::Value *pJsonValue, const CStringW & str );
		VOID JsonUnserialize( const Json::Value &jvJsonValue, CStringW *pStr );

		VOID JsonSerialize( Json::Value *pJsonValue, const CStringA & str );
		VOID JsonUnserialize( const Json::Value &jvJsonValue, CStringA *pStr );

		VOID JsonSerialize( Json::Value *pJsonValue, const IJsonSerializable & obj );
		VOID JsonUnserialize( const Json::Value &jvJsonValue, IJsonSerializable *pObj );

		template<class T> VOID JsonSerialize( Json::Value *pJsonValue, const std::map<CString, T> & mapp );
		template<class T> VOID JsonUnserialize( const Json::Value &jvJsonValue, std::map<CString, T> *pMap );

		template<class T> VOID JsonSerialize( Json::Value *pJsonValue, const std::map<DWORD, T> & mapp );
		template<class T> VOID JsonUnserialize( const Json::Value &jvJsonValue, std::map<DWORD, T> *pMap );

		template<class T> VOID JsonSerialize( Json::Value *pJsonValue, const std::set<T> & sett);
		template<class T> VOID JsonUnserialize( const Json::Value &jvJsonValue, std::set<T> *pSet );

		template<class T> VOID JsonSerialize( Json::Value *pJsonValue, const std::vector<T> & vect);
		template<class T> VOID JsonUnserialize( const Json::Value &jvJsonValue, std::vector<T> *pVect );

		template<class T> VOID JsonSerialize( Json::Value *pJsonValue, const std::list<T> & listt);
		template<class T> VOID JsonUnserialize( const Json::Value &jvJsonValue, std::list<T> *pList );

		template<class T> VOID JsonSerialize( Json::Value *pJsonValue, T * const & pt );
		template<class T> VOID JsonUnserialize( const Json::Value &jvJsonValue, T ** pPt );
	}
}

#include "../../../XLibS/implement/serialization/JsonSerialize.inl"
