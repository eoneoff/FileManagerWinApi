#pragma once
#include "FileManagerDlg.h"

/**
*Класс диалогового вікна введення імені для нової папки
*/
class _NewFolderDlg
{
public:
	_NewFolderDlg();
	static _NewFolderDlg* pNF;
	static BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
private:
	void Cls_OnCommand(HWND, int, HWND, UINT);
	void Cls_OnClose(HWND);
};