#pragma once

#include "FileManager.h"
#include <string>

#ifndef _UNICODE
typedef std::string String;
#else
typedef std::wstring String;
#endif

/**
*Класс, який зберегає шлях до директорії
*@detailed шлях зберігається у вигляді
*окремих стрічок.
*/
class Path
{
	std::vector<String> path;

public:
	Path() {}
	Path(const Path&);
	void AddLevel(TCHAR*);
	void LevelUp();
	void SetRoot(TCHAR*);
	void GetPath(TCHAR**, bool noLastSlash = false);
	TCHAR* PathToFile(const TCHAR*);
	TCHAR* Last();
};
