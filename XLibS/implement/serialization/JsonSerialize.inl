template<class T>
VOID XLibS::JsonSerializeTool::JsonUnserialize( const Json::Value &jvJsonValue, T ** pPt )
{
	*pPt = new T;
	if (!*pPt)
		return;
	JsonUnserialize(jvJsonValue, *pPt);
}

template<class T>
VOID XLibS::JsonSerializeTool::JsonSerialize( Json::Value *pJsonValue, T * const & pt )
{
	JsonSerialize(pJsonValue, *pt);
}


template<class T>
VOID XLibS::JsonSerializeTool::JsonUnserialize( const Json::Value &jvJsonValue, std::map<DWORD, T> *pMap )
{
	if (!pMap)
		return;

	try
	{
		Json::Value::iterator it = jvJsonValue.begin();
		for (; it != jvJsonValue.end(); it++)
		{
			T tNewItem;
			JsonUnserialize(*it, &tNewItem);
			pMap->insert(make_pair(strtoul(it.key().asString().c_str(), NULL, 10), tNewItem));
		}
	}
	catch (std::runtime_error e)
	{
		assert(NULL);
	}
}

template<class T>
VOID XLibS::JsonSerializeTool::JsonUnserialize( const Json::Value &jvJsonValue, std::map<CString, T> *pMap )
{
	if (!pMap)
		return;

	try
	{
		Json::Value::iterator it = jvJsonValue.begin();
		for (; it != jvJsonValue.end(); it++)
		{
			T tNewItem;
			JsonUnserialize(*it, &tNewItem);
			pMap->insert(make_pair(Util::String::ConvertUTF8toUnicode(it.key().asString().c_str()), tNewItem));
		}
	}
	catch (std::runtime_error e)
	{
		assert(NULL);
	}
}

template<class T>
VOID XLibS::JsonSerializeTool::JsonSerialize( Json::Value *pJsonValue, const std::map<DWORD, T> & mapp )
{
	if (!pJsonValue)
		return;

	Json::Value &jvJsonValue = *pJsonValue;
	jvJsonValue = Json::Value(Json::objectValue);

	std::map<DWORD, T>::const_iterator it = mapp.begin();

	for (; it != mapp.end(); it++)
	{
		CStringA strA;
		strA.Format("%lu", it->first);
		Json::Value &v = jvJsonValue[strA] = Json::Value(Json::nullValue);
		JsonSerialize( &v, it->second );
	}
}

template<class T>
VOID XLibS::JsonSerializeTool::JsonSerialize( Json::Value *pJsonValue, const std::map<CString, T> & mapp )
{
	if (!pJsonValue)
		return;

	Json::Value &jvJsonValue = *pJsonValue;
	jvJsonValue = Json::Value(Json::objectValue);

	std::map<CString, T>::const_iterator it = mapp.begin();

	for (; it != mapp.end(); it++)
	{
		CStringA strA;
		strA = Util::String::ConvertUnicodeToUTF8(it->first);
		Json::Value &v = jvJsonValue[strA] = Json::Value(Json::nullValue);
		JsonSerialize( &v, it->second );
	}
}


template<class T>
VOID XLibS::JsonSerializeTool::JsonUnserialize( const Json::Value &jvJsonValue, std::list<T> *pList )
{
	if (!pList)
		return;

	try
	{
		for (Json::Value::UInt i = 0; i < jvJsonValue.size(); i++)
		{
			T tNewItem;
			JsonUnserialize(jvJsonValue[i], &tNewItem);
			pList->push_back(tNewItem);
		}
	}
	catch (std::runtime_error e)
	{
		assert(NULL);
	}
}

template<class T>
VOID XLibS::JsonSerializeTool::JsonSerialize( Json::Value *pJsonValue, const std::list<T> & listt )
{
	if (!pJsonValue)
		return;

	Json::Value & jvJsonValue = *pJsonValue;

	jvJsonValue = Json::Value(Json::arrayValue);
	std::list<T>::const_iterator it = listt.begin();
	for (UINT i = 0; it != listt.end(); it++, i++)
	{
		jvJsonValue[i] = Json::Value(Json::nullValue);
		JsonSerialize(&jvJsonValue[i], *it);
	}
}



template<class T>
VOID XLibS::JsonSerializeTool::JsonUnserialize( const Json::Value &jvJsonValue, std::set<T> *pSet )
{
	if (!pSet)
		return;

	try
	{
		for (Json::Value::UInt i = 0; i < jvJsonValue.size(); i++)
		{
			T tNewItem;
			JsonUnserialize(jvJsonValue[i], &tNewItem);
			pSet->insert(tNewItem);
		}
	}
	catch (std::runtime_error e)
	{
		assert(NULL);
	}
}

template<class T>
VOID XLibS::JsonSerializeTool::JsonSerialize( Json::Value *pJsonValue, const std::set<T> & sett )
{
	if (!pJsonValue)
		return;

	Json::Value & jvJsonValue = *pJsonValue;
	jvJsonValue = Json::Value(Json::arrayValue);
	std::set<T>::const_iterator it = sett.begin();
	for (UINT i = 0; it != sett.end(); it++, i++)
	{
		jvJsonValue[i] = Json::Value(Json::nullValue);
		JsonSerialize(&jvJsonValue[i], *it);
	}

}


template<class T>
VOID XLibS::JsonSerializeTool::JsonUnserialize( const Json::Value &jvJsonValue, std::vector<T> *pVect )
{
	if (!pVect)
		return;

	try
	{
		for (Json::Value::UInt i = 0; i < jvJsonValue.size(); i++)
		{
			T tNewItem;
			JsonUnserialize(jvJsonValue[i], &tNewItem);
			pVect->push_back(tNewItem);
		}
	}
	catch (std::runtime_error e)
	{
		assert(NULL);
	}
}

template<class T>
VOID XLibS::JsonSerializeTool::JsonSerialize( Json::Value *pJsonValue, const std::vector<T> & vect )
{
	if (!pJsonValue)
		return;

	Json::Value & jvJsonValue = *pJsonValue;

	jvJsonValue = Json::Value(Json::arrayValue);
	std::vector<T>::const_iterator it = vect.begin();
	for (UINT i = 0; it != vect.end(); it++, i++)
	{
		jvJsonValue[i] = Json::Value(Json::nullValue);
		JsonSerialize(&jvJsonValue[i], *it);
	}
}