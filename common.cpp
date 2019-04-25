#define _CRT_RAND_S
#include "common.h"
#include <stdlib.h>
#include <shlobj.h>
#include <windowsx.h>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "Comdlg32.lib")
#pragma warning(disable: 4267 4996 4244 4311 4302 4477)

DWORD BreakCmd(PTCHAR cmdLine, PTCHAR* pointers, SIZE_T nPointers)
{
	DWORD dwArgs = 0;
	if(!cmdLine)
		return 0;
	if(!pointers)
		nPointers = 0;
	SIZE_T sLen = _tcslen(cmdLine);
	if(!sLen)
		return dwArgs;
	BOOL quotes = FALSE;
	BOOL arg = FALSE;
	while(sLen--)
	{
		if(quotes)
		{
			if(*cmdLine == _T('"'))
			{
				quotes = FALSE;
				if(pointers)
					*cmdLine = _T('\0');
				cmdLine++;
				arg = FALSE;
				continue;
			}
			if(!arg)
			{
				if(nPointers > 0)
				{
					pointers[dwArgs] = cmdLine;
					nPointers--;
				}
				arg = TRUE;
				dwArgs++;
			}
		}
		else
		{
			if((*cmdLine == _T(' ')) || (*cmdLine == _T('\t')) || (*cmdLine == _T('=')))
			{
				arg = FALSE;
				if(pointers)
					*cmdLine = _T('\0');
				cmdLine++;
				continue;
			}
			if(*cmdLine == _T('"'))
			{
				arg = FALSE;
				quotes = TRUE;
				if(pointers)
					*cmdLine = _T('\0');
				cmdLine++;
				continue;
			}
			if(!arg)
			{
				if(nPointers > 0)
				{
					pointers[dwArgs] = cmdLine;
					nPointers--;
				}
				arg = TRUE;
				dwArgs++;
			}
		}
		cmdLine++;
	}
	return dwArgs;
}

BOOL InvalidateChildRect(HWND hwndParent, HWND hwndChild)
{
	RECT rc;
	if(!GetClientRect(hwndChild, &rc))
		return FALSE;
	MapWindowPoints(hwndChild, hwndParent, (POINT*)&rc, 2);
	InvalidateRect(hwndParent, &rc, TRUE);
	return TRUE;
}
BOOL ValidateChildRect(HWND hwndParent, HWND hwndChild)
{
	RECT rc;
	if(!GetClientRect(hwndChild, &rc))
		return FALSE;
	MapWindowPoints(hwndChild, hwndParent, (POINT*)&rc, 2);
	ValidateRect(hwndParent, &rc);
	return TRUE;
}
Sentinel::Sentinel(CRITICAL_SECTION* _cs)
{
	cs = _cs;
	EnterCriticalSection(cs);
}
Sentinel::~Sentinel()
{
	LeaveCriticalSection(cs);
}
lockable_resource::lockable_resource()
{
	InitializeCriticalSection(&cs);
}
lockable_resource::~lockable_resource()
{
	DeleteCriticalSection(&cs);
}
void lockable_resource::Lock()
{
	EnterCriticalSection(&cs);
}
void lockable_resource::Unlock()
{
	LeaveCriticalSection(&cs);
}
BOOL GetStringReg(LPTSTR* ptDest, HKEY hKey, LPCTSTR ptName, LPCTSTR ptDefault)
{
	if(ptDest == NULL)
		return FALSE;
	if((*ptDest) != NULL)
	{
		free(ptDest);
		*ptDest = NULL;
	}
	if(!ptDefault)
		return FALSE;
	std::tstring strDefault = ExpandPath(ptDefault);
	DWORD dwSize = 0;
	if(RegQueryValueEx(hKey, ptName, 0, NULL, NULL, &dwSize) != ERROR_SUCCESS)
	{
		if(RegSetValueEx(hKey, ptName, 0, REG_SZ, (const BYTE*)strDefault.c_str(), (strDefault.size()+1)*sizeof(TCHAR))!= ERROR_SUCCESS)
			return FALSE;
		if(RegQueryValueEx(hKey, ptName, 0, NULL, NULL, &dwSize) != ERROR_SUCCESS)
			return FALSE;
	}
	*ptDest = (PTCHAR)malloc(dwSize);
	if(RegQueryValueEx(hKey, ptName, 0, NULL, (PBYTE)*ptDest, &dwSize) != ERROR_SUCCESS)
			return FALSE;
	return TRUE;
}
BOOL GetStringReg(std::tstring& pTstr, HKEY hKey, LPCTSTR ptValueName, LPCTSTR ptDefault)
{
	TCHAR* ptDest = NULL;
	BOOL res = GetStringReg(&ptDest, hKey, ptValueName, ptDefault);
	if(res)
	{
		pTstr = ptDest;
		free(ptDest);
	}
	return res;
}
BOOL GetQWORDReg(ULONGLONG* pulDest, HKEY hKey, LPCTSTR ptName, ULONGLONG ulDefault)
{
	if(pulDest == NULL)
		return FALSE;
	DWORD dwSize = sizeof(ULONGLONG);
	if(RegQueryValueEx(hKey, ptName, 0, NULL, (PBYTE)pulDest, &dwSize) != ERROR_SUCCESS)
	{
		if(RegSetValueEx(hKey, ptName, 0, REG_QWORD, (const BYTE*)&ulDefault, sizeof(ULONGLONG))!= ERROR_SUCCESS)
			return FALSE;
		if(RegQueryValueEx(hKey, ptName, 0, NULL, (PBYTE)pulDest, &dwSize)!= ERROR_SUCCESS)
			return FALSE;
	}
	return TRUE;
}
BOOL GetDWORDReg(DWORD* pdwDest, HKEY hKey, LPCTSTR ptName, DWORD dwDefault)
{
	if(pdwDest == NULL)
		return FALSE;
	DWORD dwSize = sizeof(DWORD);
	if(RegQueryValueEx(hKey, ptName, 0, NULL, (PBYTE)pdwDest, &dwSize) != ERROR_SUCCESS)
	{
		if(RegSetValueEx(hKey, ptName, 0, REG_DWORD, (const BYTE*)&dwDefault, sizeof(DWORD))!= ERROR_SUCCESS)
			return FALSE;
		if(RegQueryValueEx(hKey, ptName, 0, NULL, (PBYTE)pdwDest, &dwSize)!= ERROR_SUCCESS)
			return FALSE;
	}
	return TRUE;
}
BOOL SetStringReg(LPCTSTR ptSrc, HKEY hKey, LPCTSTR ptValueName)
{
	if(!ptSrc)
		return FALSE;
	return (RegSetValueEx(hKey, ptValueName, 0, REG_SZ, (BYTE*)ptSrc, sizeof(TCHAR)*(_tcslen(ptSrc)+1)) == ERROR_SUCCESS);
}
BOOL SetStringReg(std::tstring& ptSrc, HKEY hKey, LPCTSTR ptValueName)
{
	return SetStringReg(ptSrc.c_str(), hKey, ptValueName);
}
BOOL SetDWORDReg(DWORD dwSrc, HKEY hKey, LPCTSTR ptValueName)
{
	return RegSetValueEx(hKey, ptValueName, 0, REG_DWORD, (BYTE*)&dwSrc, sizeof(DWORD)) == ERROR_SUCCESS ? TRUE : FALSE;
}
BOOL SetQWORDReg(ULONGLONG ullSrc, HKEY hKey, LPCTSTR ptValueName)
{
	return RegSetValueEx(hKey, ptValueName, 0, REG_QWORD, (BYTE*)&ullSrc, sizeof(ULONGLONG)) == ERROR_SUCCESS ? TRUE : FALSE;
}

BOOL CopyStringReg(HKEY hSourceKey, LPCTSTR ptSourceValue, HKEY hDestKey, LPCTSTR ptDestValue, LPCTSTR ptDefault)
{
	std::tstring reg_str;
	BOOL res = GetStringReg(reg_str, hSourceKey, ptSourceValue, ptDefault);
	if(!res)
		return FALSE;
	res = SetStringReg(reg_str, hDestKey, ptDestValue);
	return res;
}
BOOL AddStringReg(LPCTSTR ptSrc, HKEY hKey, LPCTSTR ptValueName, LPCTSTR ptDelimeter)
{
	if(!ptSrc || !hKey)
		return FALSE;
	std::tstring ptStr;
	BOOL res = GetStringReg(ptStr, hKey, ptValueName, _T(""));
	if(!res)
		return FALSE;
	if(ptStr.compare(_T("")))
		ptStr += ptDelimeter;
	ptStr += ptSrc;
	res = SetStringReg(ptStr, hKey, ptValueName);
	return res;
}
BOOL AddStringReg(std::tstring &ptSrc, HKEY hKey, LPCTSTR ptValueName, LPCTSTR ptDelimeter)
{
	return AddStringReg(ptSrc.c_str(), hKey, ptValueName, ptDelimeter);
}
BOOL RemoveStringReg(LPCTSTR ptSrc, HKEY hKey, LPCTSTR ptValueName, LPCTSTR ptDelimeter) // доработать
{
	return FALSE;
}
BOOL RemoveStringReg(std::tstring &ptSrc, HKEY hKey, LPCTSTR ptValueName, LPCTSTR ptDelimeter)
{
	return RemoveStringReg(ptSrc.c_str(), hKey, ptValueName, ptDelimeter);
}

BOOL IsURL(LPCTSTR str) // надо доработать
{
	return FALSE;
}
INT IsImage(LPCTSTR file)
{
	INT res = NOT_IMAGE;
	HANDLE hFile = CreateFile(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return res;
	DWORD dwSign;
	DWORD dwSignJpeg;
	DWORD dwBytesRead;
	if(!ReadFile(hFile, &dwSign, sizeof(DWORD), &dwBytesRead, NULL) || (dwBytesRead != sizeof(DWORD)))
	{
		CloseHandle(hFile);
		return res;
	}
	SetFilePointer(hFile, -4, NULL, FILE_END);
	if(!ReadFile(hFile, &dwSignJpeg, 4, &dwBytesRead, NULL) || (dwBytesRead != 4))
	{
		CloseHandle(hFile);
		return res;
	}
	CloseHandle(hFile);
	DWORD asd = SIGN_JPG;
	dwSignJpeg = (dwSignJpeg & 0xFFFF0000) + (dwSign & 0x0000FFFF);
	if(dwSign == SIGN_PNG)
		res = IMAGE_PNG;
	if((dwSign & 0x00FFFFFF) == SIGN_GIF)
		res = IMAGE_GIF;
	if((dwSign & 0x0000FFFF) == SIGN_BMP)
		res = IMAGE_BMP;
	if((dwSignJpeg & 0xFFFF) == (SIGN_JPG & 0xFFFF)) //из-за того что некоторые файлы дополнены нул€ми
		res = IMAGE_JPG;
	return res;
}
BOOL HasExt(LPCTSTR file, LPCTSTR ext)
{
	BOOL res = FALSE;
	if(!ext)
		return TRUE;
	if(!file)
		return FALSE;
	PTCHAR ext_str = new TCHAR[_tcslen(ext)+1];
	_tcscpy(ext_str, ext);
	std::list<PTCHAR> ext_list;
	PTCHAR tok = _tcstok(ext_str, _T(" "));
	while(tok)
	{
		ext_list.push_back(tok);
		tok = _tcstok(NULL, _T(" "));
	}
	LPCTSTR point_ptr = _tcsrchr(file, _T('.'));
	if(point_ptr)
		for(std::list<PTCHAR>::iterator i = ext_list.begin(); i != ext_list.end(); i++)
			if (!_tcsicmp(point_ptr + 1, (*i)))
			{
				res = TRUE;
				break;
			}
	delete [] ext_str;
	return res;
}
int CreateDirFull(LPCTSTR ptDir)
{
	DWORD dwSize = ExpandEnvironmentStrings(ptDir, NULL, 0);
	LPTSTR ptDest = new TCHAR[dwSize];
	ExpandEnvironmentStrings(ptDir, ptDest, dwSize);
	int res = SHCreateDirectoryEx(NULL, ptDest, NULL);
	delete [] ptDest;
	return res;
}
int CreateFileFull(LPCTSTR ptFile)
{
	DWORD path_len = GetFullPathName(ptFile, 0, NULL, NULL);
	if(!path_len)
		return 0;
	TCHAR* full_path = new TCHAR[path_len];
	PTCHAR file_tail;
	path_len = GetFullPathName(ptFile, path_len, full_path, &file_tail);
	if(path_len)
	{
		TCHAR swap_ch = file_tail[0];
		file_tail[0] = _T('\0');
		CreateDirFull(full_path);
		file_tail[0] = swap_ch;
		HANDLE hFile = CreateFile(full_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
		if(hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
		else
			path_len = 0;
	}
	delete [] full_path;
	return path_len;
}
std::string CreateRandomNameA(DWORD dwNumChars, LPCSTR ptPrefix, LPCSTR ptPostfix)
{
	return WcharToChar(CreateRandomNameW(dwNumChars, CharToWchar(ptPrefix, 1251).c_str(), CharToWchar(ptPostfix, 1251).c_str()).c_str(), 1251);
}
std::wstring CreateRandomNameW(DWORD dwNumChars, LPCWSTR ptPrefix, LPCWSTR ptPostfix)
{
	std::wstring strNewName = L"";
	if(ptPrefix)
		strNewName = ptPrefix;
	if(dwNumChars)
	{
		if(dwNumChars > RANDOM_NAME_MAX_CHARS)
			dwNumChars = RANDOM_NAME_MAX_CHARS;
		DWORD dwRandomChunkSize = 5;
		SIZE_T sCharsToAlloc = ((dwNumChars+dwRandomChunkSize-1)/dwRandomChunkSize)*dwRandomChunkSize;
		DWORD dwChunks = sCharsToAlloc / dwRandomChunkSize;
		WCHAR* ptNewName = new WCHAR[sCharsToAlloc+1]; ptNewName[0] = L'\0';
		UINT uRand;
		for(DWORD i = 0; i < dwChunks; i++)
		{
			rand_s(&uRand);
			swprintf(ptNewName, L"%s%05d", ptNewName, uRand%100000);
		}
		ptNewName[dwNumChars] = L'\0';
		strNewName += ptNewName;
		delete [] ptNewName;
	}
	if(ptPostfix)
		strNewName += ptPostfix;
	return strNewName;
}
LPCTSTR GetExtention(LPCTSTR ptName)
{
	LPCTSTR ptExt;
	ptExt = _tcsrchr(ptName, _T('\\'));
	if(!ptExt)
		ptExt = ptName;
	ptExt = _tcsrchr(ptExt, _T('.'));
	return ptExt;
}
DWORD LurkFolder(LPCTSTR ptDir, DWORD dwDepth, DWORD dwMaxDepth, std::list<std::tstring> *file_list, LPCTSTR ptTypes)
{
	DWORD dwFiles = 0;
	if(dwDepth > dwMaxDepth)
		return 0;
	if(!ptDir)
		return 0;
	if((GetFileAttributes(ptDir) & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
		return 0;
	std::tstring strSearch;
	if(!ptDir || (!_tcscmp(ptDir, _T(""))))
	{
		DWORD dwSize = GetCurrentDirectory(0, NULL);
		TCHAR* ptCurDir = new TCHAR[dwSize];
		GetCurrentDirectory(dwSize, ptCurDir);
		strSearch = ptCurDir;
		delete [] ptCurDir;
	}
	else
		strSearch = ptDir;
	if(*strSearch.rbegin() != _T('\\'))
		strSearch += _T("\\");
	std::tstring strPath = strSearch;
	strSearch += _T("*");
	WIN32_FIND_DATA fd;
	HANDLE hSearch = FindFirstFile(strSearch.c_str(), &fd);
	if(hSearch == INVALID_HANDLE_VALUE)
		return 0;
	do
	{
		std::tstring strFullName = strPath;
		strFullName += fd.cFileName;
		if((GetFileAttributes(strFullName.c_str()) & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
		{
			if((!_tcscmp(fd.cFileName, _T("."))) || (!_tcscmp(fd.cFileName, _T(".."))))
				continue;
			dwFiles += LurkFolder(strFullName.c_str(), dwDepth+1, dwMaxDepth, file_list, ptTypes);
		}
		else
		{
			if(!HasExt(strFullName.c_str(), ptTypes))
				continue;
			file_list->push_back(strFullName);
			dwFiles++;
		}
	}while(FindNextFile(hSearch, &fd));
	FindClose(hSearch);
	return dwFiles;
}
std::tstring ChoosePath(HWND hWndOwner, LPCTSTR tCaption)
{
	LPITEMIDLIST iidl;
	BROWSEINFO bi;

	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = NULL;
	bi.lpszTitle = tCaption;
	bi.ulFlags = BIF_NEWDIALOGSTYLE;
	bi.lpfn = NULL;
	bi.lParam = 0;

	IMalloc *mal;
	SHGetMalloc(&mal);

	iidl = SHBrowseForFolder(&bi);
	if (!iidl)
	{
	  mal->Release();
	  return _T("");
	}
	TCHAR f_name[MAX_PATH];
	SHGetPathFromIDList(iidl, f_name);
	mal->Free(iidl);
	mal->Release();
	return f_name;
}
std::tstring ChooseFile(HWND hWndOwner, LPCTSTR tCaption)
{
	std::tstring res(_T(""));
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWndOwner;
	ofn.lpstrFilter = _T("Text files (*.txt)\0*.TXT\0All files\0*\0");
	ofn.lpstrFile = new TCHAR[MAX_PATH*2]; ofn.lpstrFile[0] = _T('\0');
	ofn.nMaxFile = MAX_PATH*2-1;
	ofn.lpstrTitle = tCaption;
	ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	if(GetOpenFileName(&ofn))
		res = ofn.lpstrFile;
	delete [] ofn.lpstrFile;
	return res;
}
std::tstring ChooseFileFormat(HWND hWndOwner, LPCTSTR tCaption, LPCTSTR szFormat)
{
	std::tstring res(_T(""));
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWndOwner;
	ofn.lpstrFilter = szFormat;
	ofn.lpstrFile = new TCHAR[MAX_PATH * 2];
	ofn.lpstrFile[0] = _T('\0');
	ofn.nMaxFile = MAX_PATH * 2 - 1;
	ofn.lpstrTitle = tCaption;
	ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	if (GetOpenFileName(&ofn))
		res = ofn.lpstrFile;
	delete[] ofn.lpstrFile;
	return res;
}
std::vector<std::tstring> ChooseFilesFormat(HWND hWndOwner, LPCTSTR tCaption, LPCTSTR szFormat)
{
	std::vector<std::tstring> res;
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWndOwner;
	ofn.lpstrFilter = szFormat;
	ofn.lpstrFile = new TCHAR[MAX_PATH * 100];
	ofn.lpstrFile[0] = _T('\0');
	ofn.nMaxFile = MAX_PATH * 100 - 1;
	ofn.lpstrTitle = tCaption;
	ofn.Flags = OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT;
	if (GetOpenFileName(&ofn))
	{
		if (ofn.nFileExtension)
			res.push_back(ofn.lpstrFile);
		else
		{
			std::tstring file_folder = ofn.lpstrFile;
			file_folder += _T('\\');
			LPTSTR file_ptr = ofn.lpstrFile + _tcslen(ofn.lpstrFile) + 1;
			while (_tcslen(file_ptr))
			{
				res.push_back(file_folder + file_ptr);
				file_ptr += _tcslen(file_ptr) + 1;
			}
		}
	}
	delete[] ofn.lpstrFile;
	return res;
}
std::tstring GetDlgItemString(HWND hDlg, UINT nControl)
{
	HWND hControl = GetDlgItem(hDlg, nControl);
	if(!hControl)
		return _T("");
	int textlen = GetWindowTextLength(hControl) + 1;
	if(textlen < 2)
		return _T("");
	TCHAR* buf = new TCHAR[textlen];
	int err = GetWindowText(hControl, buf, textlen);
	std::tstring res;
	if(err)
		res = buf;
	delete [] buf;
	return res;
}	
VOID ShowError(UINT uReason, LPCTSTR tStr)
{
	std::tstring strMessage = error_reasons[uReason];
	if((SIZE_T)tStr < 0x10000)
	{
		TCHAR num[10];
		strMessage += _itot((int)tStr, num, 10);
	}
	else
		strMessage += tStr;
	MessageBox(NULL, strMessage.c_str(), NULL, MB_OK | MB_ICONEXCLAMATION);
}

extern LPCTSTR error_reasons[] = {	_T("Ќе найден ресурс: "),
									_T("Ќет доступа к ресурсу"),
									_T("Ќевозможно создать файл: "),
									_T("Ќевозможно записать файл: "),
									_T("ќпераци€ прервана пользователем.")};
std::tstring ExpandPath(LPCTSTR ptPath)
{
	DWORD dwSize = ExpandEnvironmentStrings(ptPath, NULL, 0);
	LPTSTR ptTemp = new TCHAR[dwSize];
	ExpandEnvironmentStrings(ptPath, ptTemp, dwSize);
	std::tstring strExpanded = ptTemp;
	delete [] ptTemp;
	return strExpanded;
}
BOOL CopyFromResource(LPCTSTR tDest, UINT uRes)
{
	HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(uRes), _T("BINARY"));
	DWORD dwBytesToWrite = SizeofResource(NULL, hRes);
	
	HGLOBAL hMem = LoadResource(NULL, hRes);
	PBYTE pRes = (PBYTE)LockResource(hMem);
	
	HANDLE hFile = CreateFile(ExpandPath(tDest).c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	DWORD dwBytesWritten = 0;
	if(!WriteFile(hFile, pRes, dwBytesToWrite, &dwBytesWritten, NULL) || (dwBytesWritten != dwBytesToWrite))
	{
		CloseHandle(hFile);
		return FALSE;
	}
	CloseHandle(hFile);
	return TRUE;
}
HANDLE CopyFromResourceTemp(LPCTSTR tDest, UINT uRes)
{
	HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(uRes), _T("BINARY"));
	DWORD dwBytesToWrite = SizeofResource(NULL, hRes);
	
	HGLOBAL hMem = LoadResource(NULL, hRes);
	PBYTE pRes = (PBYTE)LockResource(hMem);
	
	HANDLE hFile = CreateFile(ExpandPath(tDest).c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return hFile;
	DWORD dwBytesWritten = 0;
	if(!WriteFile(hFile, pRes, dwBytesToWrite, &dwBytesWritten, NULL) || (dwBytesWritten != dwBytesToWrite))
	{
		CloseHandle(hFile);
		return INVALID_HANDLE_VALUE;
	}
	//CloseHandle(hFile);
	return hFile;
}


std::string WcharToChar(LPCWSTR str, UINT codepage, int nWchars)
{
	LPSTR a_str = WcharToCharBuff(str, codepage, nWchars);
	if(!a_str)
		return "";
	std::string res_str(a_str);
	free(a_str);
	return res_str;
}
std::string CharToChar(LPCSTR str, UINT codepage_from, int nChars, UINT codepage_to)
{
	LPSTR a_str = CharToCharBuff(str, codepage_from, nChars, codepage_to);
	if(!a_str)
		return "";
	std::string res_str(a_str);
	free(a_str);
	return res_str;
}
std::wstring CharToWchar(LPCSTR str, UINT codepage, int nChars)
{
	LPWSTR w_str = CharToWcharBuff(str, codepage, nChars);
	if(!w_str)
		return L"";
	std::wstring res_str(w_str);
	free(w_str);
	return res_str;
}
std::wstring WcharToWchar(LPCWSTR str, UINT fake_codepage, int nChars)
{
	LPWSTR w_str = WcharToWcharBuff(str, fake_codepage, nChars);
	if(!w_str)
		return L"";
	std::wstring res_str(w_str);
	free(w_str);
	return res_str;
}
LPSTR WcharToCharBuff(LPCWSTR str, UINT codepage, int nWchars)
{
	if(!str)
		return NULL;
	int len = WideCharToMultiByte(codepage, 0, str, nWchars, NULL, 0, NULL, NULL);
	if(!len)
		return NULL;
	LPSTR res_str = (LPSTR)malloc(len);
	if(!res_str)
		return NULL;
	WideCharToMultiByte(codepage, 0, str, nWchars, res_str, len, NULL, NULL);
	return res_str;
}
LPSTR CharToCharBuff(LPCSTR str, UINT codepage_from, int nChars, UINT codepage_to)
{
	if(!str)
		return NULL;
	if(!codepage_to || (codepage_from == codepage_to))
		if (nChars != 0)
		{
			if(nChars < 0)
				nChars = strlen(str);
			LPSTR res_str = (LPSTR)malloc(nChars+1);
			if(!res_str)
				return NULL;
			strncpy(res_str, str, nChars);
			return res_str;
		}
		else
			return NULL;
	LPWSTR w_str = CharToWcharBuff(str, codepage_from, nChars);
	if(!w_str)
		return NULL;
	LPSTR a_str = WcharToCharBuff(w_str, codepage_to, nChars);
	free(w_str);
	return a_str;
}
LPWSTR CharToWcharBuff(LPCSTR str, UINT codepage, int nChars)
{
	if(!str)
		return NULL;
	int len = MultiByteToWideChar(codepage, 0, str, nChars, NULL, 0);
	if(!len)
		return NULL;
	if(nChars > 0)
		len += sizeof(WCHAR);
	LPWSTR res_str = (LPWSTR)malloc(len*sizeof(WCHAR));
	if(!res_str)
		return NULL;
	MultiByteToWideChar(codepage, 0, str, nChars, res_str, len);
	if(nChars > 0)
		res_str[len-1] = L'\0';
	return res_str;	
}
LPWSTR WcharToWcharBuff(LPCWSTR str, UINT fake_codepage, int nChars)
{
	if(!str)
		return NULL;
	if(!nChars)
		return NULL;
	if(nChars < 0)
		nChars = wcslen(str);
	LPWSTR res_str = (LPWSTR)malloc((nChars+1)*sizeof(WCHAR));
	if(!res_str)
		return NULL;
	wcsncpy(res_str, str, nChars);
	return res_str;
}
UINT ReplaceSubstr(std::wstring& str, LPCWSTR substr, LPCWSTR newsubstr)
{
	UINT replaced = 0;
	std::wstring::size_type f_pos = str.rfind(substr);
	while(f_pos != str.npos)
	{
		str.replace(f_pos, wcslen(substr), newsubstr);
		f_pos = str.rfind(substr, f_pos);
		replaced++;
	}
	return replaced;
}
UINT AddLogEntry(LPCTSTR ptFile, LPCTSTR ptEntry)
{
	TCHAR ptEntryHeader[40];
	SYSTEMTIME st;
	GetSystemTime(&st);
	_stprintf(ptEntryHeader, _T("[%02d.%02d.%04d %02d:%02d:%02d] "),
		st.wDay, st.wMonth, st.wYear+1601, st.wHour, st.wMinute, st.wSecond);
	std::tstring entry = ptEntryHeader;
	entry += ptEntry;
	entry += _T("\x0D\x0A");
	CreateFileFull(ptFile);
	HANDLE hLog = CreateFile(ptFile, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);
	if(hLog == INVALID_HANDLE_VALUE)
		return 0;
	SetFilePointer(hLog, 0, NULL, FILE_END);
	DWORD dwBytesToWrite = entry.size()*sizeof(TCHAR),
		dwBytesWritten = 0;
	UINT res = 0;
	if(!WriteFile(hLog, entry.c_str(), dwBytesToWrite, &dwBytesWritten, NULL) || (dwBytesToWrite != dwBytesWritten))
		res = 1;
	CloseHandle(hLog);
	return res;
}
VOID SetValidDate(SYSTEMTIME* st)
{
	PLACE_IN_BORDERS(st->wMilliseconds, 0, 999);
	PLACE_IN_BORDERS(st->wSecond, 0, 59);
	PLACE_IN_BORDERS(st->wMinute, 0, 59);
	PLACE_IN_BORDERS(st->wHour, 0, 23);
	PLACE_IN_BORDERS(st->wDayOfWeek, 0, 6);
	PLACE_IN_BORDERS(st->wMonth, 1, 12);
	PLACE_IN_BORDERS(st->wYear, 1601, 30827);
	WORD DayTopLimit = month_days[st->wMonth];
	if(st->wMonth == 2)
		if((((st->wYear % 4) == 0) && ((st->wYear % 100) != 0)) || ((st->wYear % 400) == 0))
			DayTopLimit++;
	PLACE_IN_BORDERS(st->wDay, 1, DayTopLimit);
}
const UINT month_days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

std::tstring GetFileName(std::tstring file_path)
{
	int pos = file_path.rfind(_T('\\'));
	if(pos == file_path.npos)
		return file_path;
	if(pos == file_path.length()-1)
		return _T("");
	return file_path.substr(pos+1);
}
std::tstring GetFileDir(std::tstring file_path)
{
	int pos = file_path.rfind(_T('\\'));
	if (pos == file_path.npos)
		return file_path;
	if (pos == file_path.length() - 1)
		return _T("");
	return file_path.substr(0, pos);
}
std::tstring GetStreamName(std::tstring stream)
{
	if (stream.length() < 2) // should at least has two '::' and stream type
		return _T("");
	int pos = stream.rfind(_T(':'));
	if (pos == stream.npos)
		return _T("");
	return stream.substr(1, pos-1);
}
void FTtoLT(FILETIME* ft)
{
	FILETIME loc_ft;
	FileTimeToLocalFileTime(ft, &loc_ft);
	ft->dwHighDateTime = loc_ft.dwHighDateTime;
	ft->dwLowDateTime = loc_ft.dwLowDateTime;
}
void LTtoFT(FILETIME* ft)
{
	FILETIME ft_ft;
	LocalFileTimeToFileTime(ft, &ft_ft);
	ft->dwHighDateTime = ft_ft.dwHighDateTime;
	ft->dwLowDateTime = ft_ft.dwLowDateTime;
}
void FTtoLT(ULONGLONG* ull)
{
	ULARGE_INTEGER uli;
	uli.QuadPart = *ull;
	FILETIME ft = {uli.LowPart, uli.HighPart};
	FTtoLT(&ft);
	uli.HighPart = ft.dwHighDateTime;
	uli.LowPart = ft.dwLowDateTime;
	*ull = uli.QuadPart;
}
void LTtoFT(ULONGLONG* ull)
{
	ULARGE_INTEGER uli;
	uli.QuadPart = *ull;
	FILETIME ft = {uli.LowPart, uli.HighPart};
	LTtoFT(&ft);
	uli.HighPart = ft.dwHighDateTime;
	uli.LowPart = ft.dwLowDateTime;
	*ull = uli.QuadPart;
}
ULONGLONG GetFT()
{
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	ULARGE_INTEGER uli;
	uli.HighPart = ft.dwHighDateTime;
	uli.LowPart = ft.dwLowDateTime;
	return uli.QuadPart;
}
ULONGLONG GetLT()
{
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	FTtoLT(&ft);
	ULARGE_INTEGER uli;
	uli.HighPart = ft.dwHighDateTime;
	uli.LowPart = ft.dwLowDateTime;
	return uli.QuadPart;
}
std::tstring FTtoString(ULONGLONG ull)
{
	TCHAR str_time[40]; 
	ULARGE_INTEGER ulitime;
	ulitime.QuadPart = ull;
	FILETIME ft_time = {ulitime.LowPart, ulitime.HighPart};
	SYSTEMTIME st_time;
	FileTimeToSystemTime(&ft_time, &st_time);
	_stprintf_s(str_time, 20, _T("%02d.%02d.%04d %02d:%02d"),
		st_time.wDay, st_time.wMonth, st_time.wYear, st_time.wHour, st_time.wMinute);
	return str_time;
}
BOOL VistaAndHigher()
{
    OSVERSIONINFOEX osver;
	osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    osver.dwMajorVersion = 6;
    osver.dwMinorVersion = 0;

    ULONGLONG ullCondInfo = 0x0;
    BYTE cCondMask = VER_GREATER_EQUAL;
    VER_SET_CONDITION(ullCondInfo, VER_MAJORVERSION, cCondMask);
    VER_SET_CONDITION(ullCondInfo, VER_MINORVERSION, cCondMask);
    return VerifyVersionInfo(&osver, VER_MAJORVERSION | VER_MINORVERSION, ullCondInfo);
}
void OpenFolder(LPCTSTR folder)
{/*
	CoInitialize(NULL);
	LPITEMIDLIST folder_iidl;
	HRESULT result = SHParseDisplayName(folder, NULL, &folder_iidl, 0, NULL);
	result = SHOpenFolderAndSelectItems(folder_iidl, 0, NULL, 0);
	Sleep(0);*/
	ShellExecute(NULL, _T("open"), folder, NULL, NULL, SW_SHOWNORMAL);
}

LRESULT ComboBox_GetCurSelData(HWND hCB)
{
	int cursel = ComboBox_GetCurSel(hCB);
	if(cursel == -1)
		return CB_ERR;
	return ComboBox_GetItemData(hCB, cursel);
}
int ComboBox_GetSelByData(HWND hCB, LPARAM cb_data)
{
	int item_count = ComboBox_GetCount(hCB);
	for(int i = 0; i < item_count; i++)
	{
		LPARAM param = ComboBox_GetItemData(hCB, i);
		if(cb_data == param)
			return i;
	}
	return -1;
}
BOOL ComboBox_SetSelByData(HWND hCB, LPARAM cb_data)
{
	int index = ComboBox_GetSelByData(hCB, cb_data);
	if(index == -1)
		return FALSE;
	ComboBox_SetCurSel(hCB, index);
	return TRUE;
}
LPARAM ListView_GetItemParam(HWND hLV, int item_index)
{
	if(item_index < 0)
		return 0;
	LVITEM lvItem;
	lvItem.mask = LVIF_PARAM;
	lvItem.iItem = item_index;
	lvItem.iSubItem = 0;
	ListView_GetItem(hLV, &lvItem);
	return lvItem.lParam;
}
std::tstring GetLVItemText(HWND hLV, int item, int subitem)
{
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.cchTextMax = MAX_PATH*2;
	TCHAR str[MAX_PATH*2];
	lvItem.pszText = str;
	lvItem.iItem = item;
	lvItem.iSubItem = subitem;
	int str_size = SendMessage(hLV, LVM_GETITEMTEXT, item, (LPARAM)&lvItem);
	if(!str_size)
		return _T("");
	return str;
}
BOOL ListView_SetCheckByParam(HWND hLV, LPARAM param)
{
	int item_count = ListView_GetItemCount(hLV);
	for(int i = 0; i < item_count; i++)
	{
		LPARAM cur_param = ListView_GetItemParam(hLV, i);
		if(cur_param == param)
		{
			ListView_SetCheckState(hLV, i, TRUE);
			return TRUE;
		}
	}
	return FALSE;
}
BOOL ListView_GetCheckByParam(HWND hLV, LPARAM param)
{
	int item_count = ListView_GetItemCount(hLV);
	for(int i = 0; i < item_count; i++)
	{
		if(ListView_GetCheckState(hLV, i))
		{
			LPARAM cur_param = ListView_GetItemParam(hLV, i);
			if(cur_param == param)
				return TRUE;
		}	
	}
	return FALSE;
}
int ListView_InsertItemWithParam(HWND hLV, int item_index, LPARAM param)
{
	LVITEM lvItem;
	ZeroMemory(&lvItem, sizeof(LVITEM));
	if(item_index == -1)
		item_index = ListView_GetItemCount(hLV);
	lvItem.iItem = item_index;
	lvItem.mask = LVIF_PARAM;
	lvItem.lParam = param;
	lvItem.iSubItem = 0;
	int res = ListView_InsertItem(hLV, &lvItem);

	lvItem.mask = LVIF_TEXT;
	lvItem.pszText = const_cast<LPTSTR>(_T("0"));
	lvItem.iSubItem = 1;
	ListView_SetItem(hLV, &lvItem);
	return res;
}
void ListView_AddColumn(HWND hLV, LPCTSTR caption, int subitem, int width, int order)
{
	LVCOLUMN lvColumn;
	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM | (order != -1 ? LVCF_ORDER : 0);
	lvColumn.pszText = const_cast<LPTSTR>(caption);
	lvColumn.iOrder = order;
	lvColumn.cx = width;
	lvColumn.iSubItem = subitem;
	ListView_InsertColumn(hLV, subitem, &lvColumn);
}
void ListView_Activate(HWND hDlg, HWND hLV, INT id)
{
	NMITEMACTIVATE di;
	di.hdr.code = LVN_ITEMACTIVATE;
	di.hdr.hwndFrom = hLV;
	di.hdr.idFrom = id;
	di.iItem = ListView_GetSelectionMark(hLV);
	if (di.iItem == -1)
		return;
	SendMessage(hDlg, WM_NOTIFY, 0, (LPARAM)&di);
}
int ListView_GetItemByParam(HWND hLV, LPARAM param)
{
	int res = -1;
	int count = ListView_GetItemCount(hLV);
	for (int i = 0; i < count; i++)
	{
		if(ListView_GetItemParam(hLV, i) == param)
			return i;
	}
	return res;
}
BOOL IsDirectory(LPCTSTR file)
{
	return ((GetFileAttributes(file) & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
}
BOOL CrawlFolder(LPCTSTR first_dir, unsigned max_depth, unsigned cur_depth, FileAction* file_action)
{
	if(!first_dir)
		return FALSE;
	if(max_depth < cur_depth)
		return FALSE;
	if (!IsDirectory(first_dir))
		return FALSE;
	std::tstring strSearch;
	if(!_tcslen(first_dir))
	{
		DWORD dir_size = GetCurrentDirectory(0, NULL);
		TCHAR* cur_dir = new TCHAR[dir_size];
		GetCurrentDirectory(dir_size, cur_dir);
		strSearch = cur_dir;
		delete [] cur_dir;
	}
	else
		strSearch = first_dir;
	if(*strSearch.rbegin() != _T('\\'))
		strSearch += _T('\\');
	std::tstring strPath = strSearch;
	strSearch += _T('*');
	WIN32_FIND_DATA fd;
	HANDLE hSearch = FindFirstFile(strSearch.c_str(), &fd);
	if(hSearch == INVALID_HANDLE_VALUE)
		return FALSE;
	BOOL res = FALSE;
	do
	{
		std::tstring strFullName = strPath;
		strFullName += fd.cFileName;
		if (IsDirectory(strFullName.c_str()))
		{
			if((!_tcscmp(fd.cFileName, _T("."))) || (!_tcscmp(fd.cFileName, _T(".."))))
				continue;
			if (CrawlFolder(strFullName.c_str(), max_depth, cur_depth + 1, file_action))
			{
				res = TRUE;
				break;
			}
		}
		else
			if(file_action->Proceed(strFullName.c_str(), fd.cFileName))
			{
				res = TRUE;
				break;
			}
	}while(FindNextFile(hSearch, &fd));
	FindClose(hSearch);
	return res;
}

cmd_option::cmd_option(LPCTSTR option_name, bool paramized, std::tstring descr, std::list<std::tstring> ac_list):
_paramized(paramized)
{
	_option_name = option_name;
	is_set = false;
	_param = _T("");
	_acceptable = ac_list;
	_descr = descr;
}
bool cmd_option::Set(LPCTSTR param)
{
	if(_paramized)
		if(!param)
			return false;
		else
		{
			//Empty _acceptable means every parameter is fine
			if (!_acceptable.size())
			{
				_param = param;
				is_set = true;
				return true;
			}
			for (auto& acc : _acceptable)
			{
				if (acc == param)
				{
					_param = param;
					is_set = true;
					return true;
				}
			}
			is_set = false;
			return false;
		}
	is_set = true;
	return true;
}
std::tstring cmd_option::GetName()
{
	return _option_name;
}
bool cmd_option::IsParamized()
{
	return _paramized;
}
void cmd_option::Reset()
{
	is_set = false;
	_param.clear();
}
bool cmd_option::IsSet()
{
	return is_set;
}
int cmd_option::GetInt()
{
	return _ttoi(_param.c_str());
}
std::tstring cmd_option::GetString()
{
	return _param;
}
std::tstring cmd_option::GetOptionDescription()
{
	std::tstring res;
	res = _option_name;
	res += _T("\t");
	if (_paramized) 
		res += _T("<param> "); // надо добавить именованый параметр
	res += _descr;
	return res;
}

CmdLine::CmdLine()
{
	
}
CmdLine::~CmdLine()
{
	is_set = false;
}
CmdLine::operator bool()
{
	return is_set;
}
bool CmdLine::AddOption(LPCTSTR option_name, bool paramized, std::tstring descr, std::list<std::tstring> ac_list)
{
	cmd_option* existing_option = GetOption(option_name);
	if(existing_option)
		return false;
	options_list.push_back(new cmd_option(option_name, paramized, descr, ac_list));
	return true;
}
bool CmdLine::DeleteOption(LPCTSTR option_name)
{
	std::list<cmd_option*>::iterator i = options_list.begin();
	for(; i != options_list.end(); i++)
	{
		std::tstring temp = (*i)->GetName();
		if(!_tcscmp(option_name, temp.c_str()))
			break;
	}
	if(i == options_list.end())
		return false;
	delete *i;
	options_list.erase(i);
	return true;
}
bool CmdLine::SetCmd(LPCTSTR cmd_line)
{
	if((!cmd_line) || (!_tcslen(cmd_line)))
		return false;
	ResetCmd();
	PTCHAR cmd_dup = _tcsdup(cmd_line);
	DWORD argc = BreakCmd(cmd_dup, NULL, 0);
	free(cmd_dup);
	PTCHAR* args = new PTCHAR[argc];
	cmd_dup =_tcsdup(cmd_line);
	BreakCmd(cmd_dup, args, argc);
	is_set = true;
	for(unsigned i = 1; i < argc; i++)
	{
		cmd_option* cur_option = GetOption(args[i]);
		if(!cur_option)
		{
			is_set = false; // если встречаетс€ незнакома€ опци€, то cmd-строка считаетс€ недействительной
			break;
		}
		if(cur_option->IsParamized()) // проверим на параметр
		{
			if(i == argc-1) // если последний, то провал
			{
				is_set = false;
				break;
			}
			cur_option->Set(args[i+1]);
			i++;
		}
		else
			cur_option->Set();
	}
	delete [] args;
	free(cmd_dup);
	return is_set;
}
void CmdLine::ResetCmd()
{
	std::list<cmd_option*>::iterator i = options_list.begin();
	for(; i != options_list.end(); i++)
	{
		(*i)->Reset();
	}
	is_set = false;
}

bool CmdLine::Set(LPCTSTR option_name, LPCTSTR param)
{
	cmd_option* existing_option = GetOption(option_name);
	if(!existing_option)
		return false;
	existing_option->Set(param);
	return true;
}
bool CmdLine::Reset(LPCTSTR option_name)
{
	cmd_option* existing_option = GetOption(option_name);
	if(!existing_option)
		return false;
	existing_option->Reset();
	return true;
}
bool CmdLine::IsSet(LPCTSTR option_name)
{
	cmd_option* existing_option = GetOption(option_name);
	if(!existing_option)
		return false;
	return existing_option->IsSet();
}
int CmdLine::GetInt(LPCTSTR option_name)
{
	cmd_option* existing_option = GetOption(option_name);
	if(!existing_option)
		return 0;
	return existing_option->GetInt();
}
std::tstring CmdLine::GetString(LPCTSTR option_name)
{
	cmd_option* existing_option = GetOption(option_name);
	if(!existing_option)
		return _T("");
	return existing_option->GetString();
}
cmd_option* CmdLine::GetOption(LPCTSTR option_name)
{
	std::list<cmd_option*>::iterator i = options_list.begin();
	for(; i != options_list.end(); i++)
	{
		std::tstring temp = (*i)->GetName();
		if(!_tcscmp(option_name, temp.c_str()))
			break;
	}
	if(i == options_list.end())
		return NULL;
	return *i;
}
void CmdLine::ShowUsage()
{
	for (auto& opt : options_list)
	{
		printf("\t%s\n", opt->GetOptionDescription().c_str());
	}
}
DWORD GetDWORDOption(LPCTSTR app_name, LPCTSTR opt_name, DWORD default_val, BOOL global)
{
	if(!app_name)
		return 0;
	HKEY app_key;
	DWORD res = 0;
	std::tstring key_str = SOFTWARE_KEY; key_str += _T("\\"); key_str += app_name;
	if(RegOpenKeyEx((global ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER),
								key_str.c_str(), 0, KEY_QUERY_VALUE, &app_key) == ERROR_SUCCESS)
	{
		GetDWORDReg(&res, app_key, opt_name, default_val);
		RegCloseKey(app_key);
	}
	return res;
}
ULONGLONG GetQWORDOption(LPCTSTR app_name, LPCTSTR opt_name, ULONGLONG default_val, BOOL global)
{
	if(!app_name)
		return 0;
	HKEY app_key;
	ULONGLONG res = 0;
	std::tstring key_str = SOFTWARE_KEY; key_str += _T("\\"); key_str += app_name;
	if(RegOpenKeyEx((global ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER),
								key_str.c_str(), 0, KEY_QUERY_VALUE, &app_key) == ERROR_SUCCESS)
	{
		GetQWORDReg(&res, app_key, opt_name, default_val);
		RegCloseKey(app_key);
	}
	return res;
}
std::tstring GetStringOption(LPCTSTR app_name, LPCTSTR opt_name, LPCTSTR default_val, BOOL global)
{
	if(!app_name)
		return 0;
	HKEY app_key;
	std::tstring res = _T("");
	std::tstring key_str = SOFTWARE_KEY; key_str += _T("\\"); key_str += app_name;
	if(RegOpenKeyEx((global ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER),
								key_str.c_str(), 0, KEY_QUERY_VALUE, &app_key) == ERROR_SUCCESS)
	{
		GetStringReg(res, app_key, opt_name, default_val);
		RegCloseKey(app_key);
	}
	return res;
}
BOOL SetDWORDOption(LPCTSTR app_name, LPCTSTR opt_name, DWORD opt_val, BOOL global)
{
	if(!app_name)
		return 0;
	HKEY app_key;
	BOOL res = FALSE;
	std::tstring key_str = SOFTWARE_KEY; key_str += _T("\\"); key_str += app_name;
	if(RegOpenKeyEx((global ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER),
								key_str.c_str(), 0, KEY_SET_VALUE , &app_key) == ERROR_SUCCESS)
	{
		res = SetDWORDReg(opt_val, app_key, opt_name);
		RegCloseKey(app_key);
	}
	return res;
}
BOOL SetQWORDOption(LPCTSTR app_name, LPCTSTR opt_name, ULONGLONG opt_val, BOOL global)
{
	if(!app_name)
		return 0;
	HKEY app_key;
	BOOL res = FALSE;
	std::tstring key_str = SOFTWARE_KEY; key_str += _T("\\"); key_str += app_name;
	if(RegOpenKeyEx((global ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER),
								key_str.c_str(), 0, KEY_SET_VALUE , &app_key) == ERROR_SUCCESS)
	{
		res = SetQWORDReg(opt_val, app_key, opt_name);
		RegCloseKey(app_key);
	}
	return res;
}
BOOL SetStringOption(LPCTSTR app_name, LPCTSTR opt_name, LPCTSTR opt_val, BOOL global)
{
	if(!app_name)
		return 0;
	HKEY app_key;
	BOOL res = FALSE;
	std::tstring key_str = SOFTWARE_KEY; key_str += _T("\\"); key_str += app_name;
	if(RegOpenKeyEx((global ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER),
								key_str.c_str(), 0, KEY_SET_VALUE , &app_key) == ERROR_SUCCESS)
	{
		res = SetStringReg(opt_val, app_key, opt_name);
		RegCloseKey(app_key);
	}
	return res;
}
std::vector<std::tstring> GetDroppedFiles(HWND hWnd, HDROP hDrop)
{
	std::vector<std::tstring> res;
	UINT nFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
	TCHAR* str;
	for (UINT i = 0; i < nFiles; i++)
	{
		UINT size = DragQueryFile(hDrop, i, NULL, 0);
		str = new TCHAR[size + 1];
		DragQueryFile(hDrop, i, str, size + 1);
		res.push_back(str);
		delete [] str;
	}
	DragFinish(hDrop);
	return res;
}
UINT GetRandomNumber(UINT start, UINT end)
{
	UINT res;
	if (start >= end)
		return start;
	rand_s(&res);
	return floor(double(res) / UINT_MAX * (end - start) + start);
}
unsigned GetRandomChoice(std::vector<double> probs)
{
	unsigned res = 0;
	if (!probs.size())
		return res;
	double sum_power = 0;
	for (auto& prob : probs)
	{
		sum_power += prob;
	}
	double random_num_d = double(GetRandomNumber()) / UINT_MAX * sum_power;
	for (unsigned i = 0; i < probs.size(); i++)
	{
		random_num_d -= probs[i];
		if (random_num_d <= 0)
		{
			res = i;
			break;
		}
	}
	return res;
}