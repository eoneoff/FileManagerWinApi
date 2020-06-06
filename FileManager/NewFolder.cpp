#include "NewFolderDlg.h"

_NewFolderDlg* _NewFolderDlg::pNF = nullptr;

_NewFolderDlg::_NewFolderDlg()
{
	pNF = this;
}

/**
*��������� ����� ������� ���� ��������� ����
*/
BOOL _NewFolderDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hwnd, WM_COMMAND, _NewFolderDlg::pNF->Cls_OnCommand);
		HANDLE_MSG(hwnd, WM_CLOSE, _NewFolderDlg::pNF->Cls_OnClose);
	}
	return FALSE;
}

/**
*����� ��� ������� ����������� WM_COMMAND
*/
void _NewFolderDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtrl, UINT codeNotify)
{
	TCHAR forbidden[] = _T("<>:\"/\\|/'");//������ ����������� ������� � ���� �����
	TCHAR buffer[256];
	HWND textBox = GetDlgItem(hwnd, IDC_EDIT_PATH);
	int length;

	switch (id)
	{
	case(IDOK)://��������� ������ ��
		Edit_GetText(textBox, buffer, 256);
		length = _tcslen(buffer) + 1;
		_FileManagerDlg::pFM->newPath = new TCHAR[length];
		_tcscpy_s(_FileManagerDlg::pFM->newPath, length, buffer);
		EndDialog(hwnd, IDOK);
		break;
	case(IDCANCEL)://��������� ������ �����
		EndDialog(hwnd, IDCANCEL);
		break;
	case(IDC_EDIT_PATH)://������� ������� ����� �� ��������� ������� �� �� ��������� ��� ��������
		if (codeNotify == EN_CHANGE)
		{
			Edit_GetText(textBox, buffer, 256);
			int i = 0;
			while (buffer[i] != '\0' && _tcschr(forbidden, buffer[i]) == nullptr)
			{
				++i;
			}
			int a = 0;
			if (buffer[i] != '\0')
			{
				do
				{
					buffer[i] = buffer[i+1];
					++i;
				} while (buffer[i] != '\0');
				Edit_SetText(textBox, buffer);
			}
		}
		break;
	}
}

/*
*������� ��������� ���������� ����
*/
void _NewFolderDlg::Cls_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 0);
}