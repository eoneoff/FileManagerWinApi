#include "Path.h"
#include <algorithm>
#pragma warning(disable:4996)


/**
*����������� ���������
*/
Path::Path(const Path& nPath)
{
	this->path = nPath.path;
}

/**
*��������� ���� �� �����
*/
void Path::AddLevel(TCHAR* level)
{
	String s = level;
	if (s.back() == '\\')
		s.pop_back();

	path.push_back(s);
}

/**
*������� �� ����� ����
*/
void Path::LevelUp()
{
	path.pop_back();
}

/**
*������������ �������� �������� �� ������� � ��
*@param root �������� ���������
*/
void Path::SetRoot(TCHAR* root)
{
	path.clear();
	AddLevel(root);
}

/**
*���� ��������� ����� � c-style string
*@param rString �������� �� c-style string,
*� ��� ��������� ���� � ������ ������
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
*���� ���� �� ����� � ����� ��������
*@param file ��'� �����
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
*���������� ���������� ���� � ��������
*/
TCHAR* Path::Last()
{
	String s = path.back();
	TCHAR* rVal = new TCHAR[s.size() + 1];
	_tcscpy_s(rVal, s.size() + 1, s.c_str());
	return rVal;
}