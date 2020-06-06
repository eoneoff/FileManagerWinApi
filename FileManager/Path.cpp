#include "Path.h"
#include <algorithm>
#pragma warning(disable:4996)


/**
*Конструкток копіювання
*/
Path::Path(const Path& nPath)
{
	this->path = nPath.path;
}

/**
*Додавання рівня до шляху
*/
void Path::AddLevel(TCHAR* level)
{
	String s = level;
	if (s.back() == '\\')
		s.pop_back();

	path.push_back(s);
}

/**
*Перехід на рівень вище
*/
void Path::LevelUp()
{
	path.pop_back();
}

/**
*Встановлення кореневої директорії та перехід в неї
*@param root коренева директорія
*/
void Path::SetRoot(TCHAR* root)
{
	path.clear();
	AddLevel(root);
}

/**
*Вивід поточного шляху у c-style string
*@param rString вказівник на c-style string,
*в яку вноситься шлях у вигляді стрічки
*/
void Path::GetPath(TCHAR** rString, bool noLastSlash)
{
	String s;
	for (String l : path)
	{
		s.append(l);
		s.push_back('\\');
	}

	if (noLastSlash)
		s.pop_back();

	*rString = new TCHAR[s.size() + 1];

	_tcscpy(*rString, s.c_str());
}

/**
*Вивід шлях до файлу в данній директорії
*@param file ім'я файла
*/
TCHAR* Path::PathToFile(const TCHAR* file)
{
	TCHAR* path = nullptr;
	GetPath(&path);
	TCHAR buffer[MAX_PATH];
	_tcscpy_s(buffer, path);
	_tcscat_s(buffer, file);

	return buffer;
}

/**
*Повернення оснаннього рівню в директорії
*/
TCHAR* Path::Last()
{
	String s = path.back();
	TCHAR* rVal = new TCHAR[s.size() + 1];
	_tcscpy_s(rVal, s.size() + 1, s.c_str());
	return rVal;
}