#include "FileManagerDlg.h"
#include "NewFolderDlg.h"
#include <fstream>
#include <iostream>
#include <io.h>
#include <direct.h>
#include <shellapi.h>

_FileManagerDlg* _FileManagerDlg::pFM = nullptr;

/**
*Конструктор діалогу основного вікна
*@detailed присвоює новий об'єкт вікна статичному вказівнику
*/
_FileManagerDlg::_FileManagerDlg()
{
	pFM = this;
	newPath = nullptr;
}

/**
*Статичний метод обробки подій основного вікна
*@detailed За допомогою макросів розпакувальника повідомлень 
*здійснює розпізнавання повідомлень вікна та виклик відповідних методів
*/
BOOL _FileManagerDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		HANDLE_MSG(hwnd, WM_INITDIALOG, pFM->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_CLOSE, pFM->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_COMMAND, pFM->Cls_OnCommand);
		HANDLE_MSG(hwnd, WM_NOTIFY, pFM->OnNotify);
	}

	return FALSE;
}

/**
*Метод, який викликаєтья при ініціалізації головного вікна
*/
BOOL _FileManagerDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	//Отримання ідентифікаторів панелей дисків та списків директорій
	lPanel = GetDlgItem(hwnd, IDC_LPANEL_COMBO);
	rPanel = GetDlgItem(hwnd, IDC_RPANEL_COMBO);
	lList = GetDlgItem(hwnd, IDC_LPANEL_LIST);
	rList = GetDlgItem(hwnd, IDC_RPANEL_LIST);

	//Отримання хендлів на ресурси іконок
	HANDLE hDelIcon = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_DELETE), IMAGE_ICON, 25, 25, NULL);
	HANDLE hCopyIcon = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_COPY), IMAGE_ICON, 25, 25, NULL);
	HANDLE hNewIcon = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_NEW), IMAGE_ICON, 25, 25, NULL);
	HANDLE hMoveIcon = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_MOVE), IMAGE_ICON, 25, 25, NULL);
	HANDLE title = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_TITLE), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON), 0);

	//встановлення іконок кнопок та додатку
	SendMessage(GetDlgItem(hwnd, IDC_DEL_BUTTON), BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hDelIcon);
	SendMessage(GetDlgItem(hwnd, IDC_COPY_BUTTON), BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hCopyIcon);
	SendMessage(GetDlgItem(hwnd, IDC_NEW_BUTTON), BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hNewIcon);
	SendMessage(GetDlgItem(hwnd, IDC_MOVE_BUTTON), BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hMoveIcon);
	SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)title);

	//Отримання списку дисків та заповнення ним панелей дисків
	TCHAR buffer[256];
	TCHAR driveString[64];
	TCHAR* disk;
	GetLogicalDriveStrings(256, buffer);
	disk = buffer;
	while (*disk)
	{
		_tcscpy_s(driveString, disk);
		ComboBox_AddString(lPanel, driveString);
		ComboBox_AddString(rPanel, driveString);
		disk = disk + _tcslen(disk) + 1;
	}

	//Встановленя обраним першого диску та встановлення його
	//у якості директорій обох панелей
	ComboBox_SetCurSel(lPanel, 0);
	ComboBox_SetCurSel(rPanel, 0);
	ComboBox_GetLBText(lPanel, 0, buffer);
	lPath.SetRoot(buffer);
	ComboBox_GetLBText(rPanel, 0, buffer);
	rPath.SetRoot(buffer);

	//Створення колонок файлових вікон
	ListView_SetExtendedListViewStyle(lList, LVS_EX_FULLROWSELECT);
	ListView_SetExtendedListViewStyle(rList, LVS_EX_FULLROWSELECT);
	_TCHAR colHead[5][10] = {_T("Имя"), _T("Тип"), _T("Размер"), _T("Дата") };
	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.cx = 90;
	lvc.fmt = LVCFMT_LEFT;

	for (int i = 0; i < 5; ++i)
	{
		lvc.iSubItem = i;
		lvc.pszText = colHead[i];
		ListView_InsertColumn(lList, i, &lvc);
		ListView_InsertColumn(rList, i, &lvc);
	}

	//Завантаження системних іконок у якості бібліотеки зображень для списків файлів
	SHFILEINFO iconsfi;
	icons = (HIMAGELIST)SHGetFileInfo(_T("C:\\"), 0, &iconsfi, sizeof(iconsfi), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	ListView_SetImageList(lList, icons, LVSIL_SMALL);
	ListView_SetImageList(rList, icons, LVSIL_SMALL);

	//Вивід файлів у списки
	GetFiles(lList);
	GetFiles(rList);

	return TRUE;
}

/**
*Функцыф обробки повідомлення WM_COMMAND
*/
void _FileManagerDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	HWND comboBox=0;
	HWND list;
	HWND oppList;
	Path*path;
	Path*oppPath;

	TCHAR buffer[MAX_PATH];

	int left = ListView_GetNextItem(lList, -1, LVNI_SELECTED);
	int right = ListView_GetNextItem(rList, -1, LVNI_SELECTED);

	switch (id)
	{
	case(IDC_LPANEL_COMBO)://активована ліве меню вибору диску
		comboBox = lPanel;
		list = lList;
		path = &lPath;
		break;
	case (IDC_RPANEL_COMBO)://активоване праве меню вибору диску
		comboBox = rPanel;
		list = rList;
		path = &rPath;
		break;
	case (IDC_COPY_BUTTON)://копіювання
		if (left == -1 && right == -1)
		{
			MessageBox(0,_T("Отметье файл для копирования"), _T("Внимание!"), MB_OK|MB_ICONEXCLAMATION);
		}
		else
		{
			if (right == -1)
			{
				path = &lPath;
				oppPath = &rPath;
				list = lList;
				oppList = rList;
			}
			else
			{
				path = &rPath;
				oppPath = &lPath;
				list = rList;
				oppList = lList;
			}

			ListView_GetItemText(list, 
				ListView_GetNextItem(list, -1, LVNI_SELECTED), 0, buffer, MAX_PATH);

			if (!FileExists(oppPath->PathToFile(buffer)))
			{
				Path srcPath = *path;
				Path dstPath = *oppPath;
				srcPath.AddLevel(buffer);
				dstPath.AddLevel(buffer);

				Copy(&srcPath, &dstPath);

				GetFiles(oppList);
			}
			
		}
		break;
	case IDC_MOVE_BUTTON://переміщення
		if (left == -1 && right == -1)
		{
			MessageBox(0, _T("Отметье файл для перемещения"), _T("Внимание!"), MB_OK | MB_ICONEXCLAMATION);
		}
		else
		{
			if (right == -1)
			{
				path = &lPath;
				oppPath = &rPath;
				list = lList;
				oppList = rList;
			}
			else
			{
				path = &rPath;
				oppPath = &lPath;
				list = rList;
				oppList = lList;
			}

			ListView_GetItemText(list,
				ListView_GetNextItem(list, -1, LVNI_SELECTED), 0, buffer, MAX_PATH);

			Path src = *path;
			Path dest = *oppPath;
			src.AddLevel(buffer);
			dest.AddLevel(buffer);

			TCHAR*sPath;
			TCHAR*dPath;
			src.GetPath(&sPath);
			dest.GetPath(&dPath);
			_stprintf_s(buffer, MAX_PATH, _T("Вы точно хотите переместить %s в %s"), sPath, dPath);
			if (MessageBox(0, buffer, _T("Вы уверены?"), MB_ICONQUESTION | MB_YESNO) == IDYES)
			{
				Copy(&src, &dest);
				Delete(&src);
				GetFiles(list);
				GetFiles(oppList);
			}

		}
		break;
	case IDC_DEL_BUTTON://видалення
		if (left == -1 && right == -1)
		{
			MessageBox(0, _T("Отметье файл для удаления"), _T("Внимание!"), MB_OK | MB_ICONEXCLAMATION);
		}
		else
		{
			if (right == -1)
			{
				path = &lPath;
				list = lList;
			}
			else
			{
				path = &rPath;
				list = rList;
			}

			ListView_GetItemText(list,
				ListView_GetNextItem(list, -1, LVNI_SELECTED), 0, buffer, MAX_PATH);

			Path delPath = *path;

			delPath.AddLevel(buffer);
			TCHAR* file;
			delPath.GetPath(&file, true);

			_stprintf_s(buffer, MAX_PATH, _T("Вы точно хотите удалить файл %s?"), file);

			if (MessageBox(0, buffer, _T("Вы уверены?"), MB_ICONQUESTION | MB_YESNO) == IDYES)
			{

				Delete(&delPath);
			}

			GetFiles(list);
		}
		break;
	case IDC_NEW_BUTTON://створення папки
		if (right == -1 && left == -1)
		{
			MessageBox(0, _T("Выберите место для создания папки"), _T("Внимание!"), MB_OK | MB_ICONEXCLAMATION);
		}
		else
		{
			if (right == -1)
			{
				path = &lPath;
				list = lList; 
			}
			else
			{
				path = &rPath;
				list = rList;
			}
			_NewFolderDlg nf;
			if (DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG2), hwnd, _NewFolderDlg::DlgProc) == IDOK)
			{
				_tcscpy_s(buffer, MAX_PATH, path->PathToFile(newPath));
				if (FileExists(buffer, true))
				{
					TCHAR message[256];
					_stprintf_s(message, 256, _T("Папка %s уже существует"), buffer);
					MessageBox(0, message, _T("Внимание!"), MB_OK | MB_ICONEXCLAMATION);
				}
				else
				{
					_tmkdir(buffer);
					GetFiles(list);
				}

				delete newPath;
			}
		}
		break;
	}

	if (codeNotify == CBN_SELCHANGE && comboBox != 0)//зміна кореневого диску панели
	{
		TCHAR buffer[8];
		ComboBox_GetText(comboBox, buffer, 8);
		path->SetRoot(buffer);
		GetFiles(list);
	}
}

/**
*Обробка повідомлення WM_NOTIFY (від List View)
*/
BOOL _FileManagerDlg::OnNotify(HWND hWnd, int id, NMHDR *hdr)
{
	HWND list= 0;
	HWND opposite=0;
	Path* path;
	int selectedItem;
	DWORD attr;

	switch (id)
	{
	case(IDC_LPANEL_LIST)://активований лівий список
		list = lList;
		opposite = rList;
		path = &lPath;
		break;
	case(IDC_RPANEL_LIST)://активований правий список
		list = rList;
		opposite = lList;
		path = &rPath;
		break;
	}
	if (list!=0)
	{
		switch (hdr->code)
		{
		case NM_DBLCLK://перехід до підпапки
			selectedItem = ListView_GetNextItem(list, -1, LVNI_SELECTED);
			TCHAR buffer[MAX_PATH];
			TCHAR p[MAX_PATH];
			ListView_GetItemText(list, selectedItem, 0, buffer, MAX_PATH);
			_tcscpy_s(p, MAX_PATH, path->PathToFile(buffer));
			attr = GetFileAttributes(p);
			if (_tcscmp(buffer, _T("..")) == 0)
			{
				path->LevelUp();
				GetFiles(list);
			}
			else if (_tcscmp(buffer, _T(".")) != 0)
			{
				if (attr & FILE_ATTRIBUTE_DIRECTORY)
				{
					path->AddLevel(buffer);
					GetFiles(list);
				}
				else
				{
					TCHAR b[MAX_PATH];
					_tcscpy_s(b, MAX_PATH, path->PathToFile(buffer));
					HINSTANCE ret = ShellExecute(0, NULL, b, 0, 0, SW_SHOWNORMAL);
				}
			}
			break;
		case NM_CLICK://зняття виділення з протилежної панелі
			GetFiles(opposite);
			break;
		}
	}
	return TRUE;
}


/**
*Метод, який викликається при закритті основного вікна
*/
void _FileManagerDlg::Cls_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 0);
}

/**
*Отримання файлів з поточної директорії на вивід їх до вікон програми
*/
void _FileManagerDlg::_FileManagerDlg::GetFiles(HWND list)
{
	ListView_DeleteAllItems(list);

	TCHAR* path = nullptr;

	Path* current;//отримання шляху панелі
	if (list == lList)
		current = &lPath;
	else
		current = &rPath;
	current->GetPath(&path);

	WIN32_FIND_DATA fileData;
	HANDLE found;
	SHFILEINFO info;
	SYSTEMTIME sTime;

	LVITEM lvi;
	memset(&lvi, 0, sizeof(lvi));
	lvi.iSubItem = 0;
	lvi.stateMask = 0;
	
	TCHAR buffer[MAX_PATH];
	_tcscpy_s(buffer, path);
	_tcscat_s(buffer, _T("*"));

	found = FindFirstFile(buffer, &fileData);

	int iItem = 0;
	do//Зчитування параметрів файлів та заповлення списку
	{
		if (_tcslen(fileData.cFileName)<=MAX_PATH)
		{
			lvi.mask = LVIF_PARAM | LVIF_IMAGE | LVIF_TEXT;
			lvi.iItem = iItem++;
			lvi.lParam = iItem;
			lvi.iSubItem = 0;
			lvi.pszText = fileData.cFileName;
			TCHAR* file = current->PathToFile(fileData.cFileName);
			SHGetFileInfo(file, -1, &info, sizeof(info), SHGFI_SYSICONINDEX | SHGFI_DISPLAYNAME | SHGFI_TYPENAME);
			lvi.iImage = info.iIcon;
			ListView_InsertItem(list, &lvi);

			lvi.mask = LVCF_SUBITEM | LVIF_TEXT;
			lvi.iSubItem = 1;
			lvi.pszText = info.szTypeName;
			ListView_SetItem(list, &lvi);

			lvi.iSubItem = 2;
			int fileSize = (fileData.nFileSizeHigh*(MAXWORD + 1) + fileData.nFileSizeLow);
			if (fileSize>0)
			{
				_stprintf_s(buffer, MAX_PATH, _T("%d"), fileSize);
				lvi.pszText = buffer;
			}
			else
			{
				lvi.pszText = nullptr;
			}
			ListView_SetItem(list, &lvi);

			lvi.iSubItem = 3;
			FileTimeToSystemTime(&fileData.ftCreationTime, &sTime);
			_stprintf_s(buffer, MAX_PATH, _T("%02d.%02d.%04d %02d:%02d"), sTime.wYear, sTime.wDay, sTime.wYear, sTime.wHour, sTime.wMinute);
			lvi.pszText = buffer;
			ListView_SetItem(list, &lvi);
		}
	} while (FindNextFile(found, &fileData) != 0);

	delete path;


}

/**
*Перевірка існування файлу чи директорії (з підтвердженням дії або без неї)
*/
bool _FileManagerDlg::FileExists(TCHAR* path, bool silent)
{
	TCHAR buffer[256];
	if (_taccess(path,00))
	{
		return false;
	}
	if (!silent)
	{
		_stprintf_s(buffer, 256, _T("Файл %s существует. Хотите его перезаписать?"), path);
		return MessageBox(0, buffer, _T("Внимание"), MB_ICONQUESTION | MB_YESNO) == IDNO;
	}
	return true;
}


/**
*Рекурсивне копіювання вмісту папки
*/
void _FileManagerDlg::Copy(Path* src, Path* dest)
{
	_tfinddata_t fileinfo;
	TCHAR *srcPath;
	TCHAR*destPath;
	src->GetPath(&srcPath, true);
	dest->GetPath(&destPath, true);
	if (srcPath[_tcslen(srcPath)-1]!='.')
	{
		long done = _tfindfirst(srcPath, &fileinfo);
		if(fileinfo.attrib == _A_SUBDIR)
		{
			_findclose(done);
			_tmkdir(destPath);
			done = _tfindfirst(src->PathToFile(_T("*.*")), &fileinfo);
			do
			{
				src->AddLevel(fileinfo.name);
				dest->AddLevel(fileinfo.name);
				Copy(src, dest);
				src->LevelUp();
				dest->LevelUp();

			} while (_tfindnext(done, &fileinfo) != -1);
			_findclose(done);
		}
		else
		{
			_findclose(done);
			CopySingleFile(src, dest);
		}
	}
}

/**
*Копіювання єдиного файлу
*/
void _FileManagerDlg::CopySingleFile(Path* src, Path* dest)
{
	TCHAR*srcPath;
	TCHAR*destPath;
	src->GetPath(&srcPath, true);
	dest->GetPath(&destPath, true);
	std::fstream in(srcPath, std::ios::in|std::ios::binary);
	std::fstream out(destPath, std::ios::out|std::ios::binary);

	out << in.rdbuf();

	in.close();
	out.close();

	delete srcPath;
	delete destPath;
}

/**
*Рекурсивне видалення файлів та директорій
*/
void _FileManagerDlg::Delete(Path* file)
{
	TCHAR*path;
	file -> GetPath(&path, true);
	if (_tcscmp(file->Last(),_T("."))!=0 && _tcscmp(file->Last(), _T(".."))!=0)
	{
		_tfinddata_t fileinfo;
		long done = _tfindfirst(path, &fileinfo);
		if (fileinfo.attrib == _A_SUBDIR)
		{
			_findclose(done);
			TCHAR search[MAX_PATH];
			_tcscpy_s(search, file->PathToFile(_T("*")));
			done = _tfindfirst(search, &fileinfo);
			do
			{
				file->AddLevel(fileinfo.name);
				Delete(file);
				file->LevelUp();
			} while (_tfindnext(done, &fileinfo) != -1);
			_findclose(done);
			_trmdir(path);
		}
		else
		{
			_findclose(done);
			_tremove(path);
		}
	}
}