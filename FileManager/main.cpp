#include "FileManagerDlg.h"
/**
 *������� ������� �������� ����
 *@detailed ������ ����������� ��������� ��������� �������� ���������
 *�� ������� ������ ��������� ����
 */
int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPinst, LPTSTR lpszCmdLine, int sShowMode)
{
	INITCOMMONCONTROLSEX icc = { sizeof(INITCOMMONCONTROLSEX) };
	icc.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icc);
	_FileManagerDlg pFM;
	return DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, _FileManagerDlg::DlgProc);
}