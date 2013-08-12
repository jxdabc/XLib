#include "../../libcommon.h"
#include "../../../inc/interfaceS/serialization/JsonSerialize.h"
#include "../../../inc/interfaceS/string/StringCode.h"
#include <assert.h>


VOID XLibS::JsonSerializeTool::JsonSerialize( Json::Value *pJsonValue, const IJsonSerializable & obj )
{
	if (!pJsonValue)
		return;

	obj.JsonSerialize(pJsonValue);
}

VOID XLibS::JsonSerializeTool::JsonSerialize( Json::Value *pJsonValue, const DWORD & n )
{
	if (!pJsonValue)
		return;

	*pJsonValue = Json::Value((Json::Value::UInt)n);
}

VOID XLibS::JsonSerializeTool::JsonSerialize( Json::Value *pJsonValue, const INT & n )
{
	if (!pJsonValue)
		return;

	*pJsonValue = Json::Value((Json::Value::Int)n);
}

VOID XLibS::JsonSerializeTool::JsonSerialize( Json::Value *pJsonValue, const CStringW & str )
{
	if (!pJsonValue)
		return;

	*pJsonValue = Json::Value((const char *)XLibS::StringCode::ConvertUnicodeToUTF8(str));
}

VOID XLibS::JsonSerializeTool::JsonSerialize( Json::Value *pJsonValue, const CStringA & str )
{
	if (!pJsonValue)
		return;

	CStringW strw = XLibS::StringCode::ConvertAnsiStrToWideStr(str);

	XLibS::JsonSerializeTool::JsonSerialize(pJsonValue, strw);
}



VOID XLibS::JsonSerializeTool::JsonUnserialize( const Json::Value &jvJsonValue, IJsonSerializable *pObj )
{
	if (!pObj)
		return;

	pObj->JsonUnserialize(jvJsonValue);
}

VOID XLibS::JsonSerializeTool::JsonUnserialize( const Json::Value &jvJsonValue, DWORD *n )
{
	if (!n)
		return;

	try
	{
		*n = jvJsonValue.asUInt();
	}
	catch (std::runtime_error e)
	{
		assert(NULL);
	}
}

VOID XLibS::JsonSerializeTool::JsonUnserialize( const Json::Value &jvJsonValue, INT *n )
{
	if (!n)
		return;

	try
	{
		*n = jvJsonValue.asInt();
	}
	catch (std::runtime_error e)
	{
		assert(NULL);
	}
}


VOID XLibS::JsonSerializeTool::JsonUnserialize( const Json::Value &jvJsonValue, CStringW *pStr )
{
	if (!pStr)
		return;

	try
	{
		*pStr = XLibS::StringCode::ConvertUTF8toUnicode(jvJsonValue.asString().c_str());
	}
	catch (std::runtime_error e)
	{
		assert(NULL);
	}
}

VOID XLibS::JsonSerializeTool::JsonUnserialize( const Json::Value &jvJsonValue, CStringA *pStr )
{
	if (!pStr)
		return;

	try
	{
		CStringW strw;
		XLibS::JsonSerializeTool::JsonUnserialize(jvJsonValue, &strw);
		*pStr = XLibS::StringCode::ConvertWideStrToAnsiStr(strw);
	}
	catch (std::runtime_error e)
	{
		assert(NULL);
	}
}


