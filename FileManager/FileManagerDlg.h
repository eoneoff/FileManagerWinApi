#pragma once

#include "FileManager.h"
#include "Path.h"

/**
 *����� ������ ��������� ����
 *@detailed ̳����� �������� �������� �� ��������� ��������� ����
 * �� CALLBACK ����� ��� ������� ���� ����
 */
class _FileManagerDlg
{
	HWND lPanel, rPanel, lList, rList;
	Path rPath;
	Path lPath;
	HIMAGELIST icons;

public:
	_FileManagerDlg();
	static _FileManagerDlg* pFM;
	TCHAR* newPath;//������ ��� ��������� ���� ���� �����

	static BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
private:
	BOOL Cls_OnInitDialog(HWND, HWND, LPARAM);
	void Cls_OnCommand(HWND, int, HWND, UINT);
	BOOL OnNotify(HWND, int, NMHDR *);
	void Cls_OnClose(HWND);
	void GetFiles(HWND);
	bool FileExists(TCHAR*, bool silent = false);
	void CopySingleFile(Path*, Path*);
	void Copy(Path*, Path*);
	void Delete(Path*);
};