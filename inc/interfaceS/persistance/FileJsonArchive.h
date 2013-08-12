#pragma once

#include <atlstr.h>

#include "..\..\..\Lib\include\json\value.h"

namespace XLibS
{
	class CFileJsonArchive
	{
	public:
		CFileJsonArchive(LPCWSTR szFileName);

	public:
		BOOL SaveToFile(const Json::Value &);
		BOOL LoadFromFile(Json::Value *);

	private:
		CStringW m_strFileName;
	};
}

