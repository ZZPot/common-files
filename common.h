#pragma once
#include <windows.h>
#include <process.h>
#include <tchar.h>
#include <string>
#include <list>
#include <vector>

typedef unsigned (__stdcall *PTHREAD_FUNC)(PVOID);
#define BeginThreadEx(LPSA, SSIZE, PFUNC, pvARG, FLAGS, PTID) \
	(HANDLE)_beginthreadex(	(PVOID)(LPSA), \
					(unsigned)(SSIZE),\
					(PTHREAD_FUNC)(PFUNC),\
					(PVOID)(pvARG),\
					(unsigned)(FLAGS),\
					(unsigned*)(PTID))

DWORD BreakCmd(PTCHAR cmdLine, PTCHAR* pointers, SIZE_T nPointers);
BOOL InvalidateChildRect(HWND hwndParent, HWND hwndChild);
BOOL ValidateChildRect(HWND hwndParent, HWND hwndChild);

struct Sentinel  //for single operation
{
	CRITICAL_SECTION* cs;
	Sentinel(CRITICAL_SECTION*);
	~Sentinel();
};

struct lockable_resource //for multiple operations
{
	lockable_resource();
	virtual ~lockable_resource();
	void Lock();
	void Unlock();
protected:
	CRITICAL_SECTION cs;
};

#ifdef _UNICODE
	#define tstring wstring
#else
	#define tstring string
#endif
BOOL GetStringReg(LPTSTR* ptStr, HKEY hKey, LPCTSTR ptValueName, LPCTSTR ptDefault);
BOOL GetStringReg(std::tstring& pTstr, HKEY hKey, LPCTSTR ptValueName, LPCTSTR ptDefault);
BOOL GetQWORDReg(ULONGLONG* uQword, HKEY hKey, LPCTSTR ptValueName, ULONGLONG ullDefault);
BOOL GetDWORDReg(DWORD* dwNum, HKEY hKey, LPCTSTR ptValueName, DWORD dwDefault);

BOOL SetStringReg(LPCTSTR ptSrc, HKEY hKey, LPCTSTR ptValueName);
BOOL SetStringReg(std::tstring& ptSrc, HKEY hKey, LPCTSTR ptValueName);
BOOL SetDWORDReg(DWORD dwSrc, HKEY hKey, LPCTSTR ptValueName);
BOOL SetQWORDReg(ULONGLONG ullSrc, HKEY hKey, LPCTSTR ptValueName); 

BOOL CopyStringReg(HKEY hSourceKey, LPCTSTR ptSourceValue, HKEY hDestKey, LPCTSTR ptDestValue, LPCTSTR ptDefault);

BOOL AddStringReg(LPCTSTR ptSrc, HKEY hKey, LPCTSTR ptValueName, LPCTSTR ptDelimeter);
BOOL AddStringReg(std::tstring &ptSrc, HKEY hKey, LPCTSTR ptValueName, LPCTSTR ptDelimeter);
BOOL RemoveStringReg(LPCTSTR ptSrc, HKEY hKey, LPCTSTR ptValueName, LPCTSTR ptDelimeter);
BOOL RemoveStringReg(std::tstring &ptSrc, HKEY hKey, LPCTSTR ptValueName, LPCTSTR ptDelimeter);

INT IsImage(LPCTSTR file);
BOOL HasExt(LPCTSTR file, LPCTSTR ext);
BOOL IsURL(LPCTSTR str); //проверяет является ли строка путём к файлу, ссылкой или ни тем, ни другим
DWORD LurkFolder(LPCTSTR ptDir, DWORD dwDepth, DWORD dwMaxDepth, std::list<std::tstring>* file_list, LPCTSTR ptTypes);
std::tstring ChoosePath(HWND hWndOwner, LPCTSTR tCaption);
std::tstring ChooseFile(HWND hWndOwner, LPCTSTR tCaption);
std::tstring ChooseFileFormat(HWND hWndOwner, LPCTSTR tCaption, LPCTSTR szFormat);
std::vector<std::tstring> ChooseFilesFormat(HWND hWndOwner, LPCTSTR tCaption, LPCTSTR szFormat);

#define NOT_IMAGE	0
#define IMAGE_JPG	1
#define IMAGE_PNG	2
#define IMAGE_GIF	3
#define IMAGE_BMP	4

#define SIGN_PNG		0x474E5089		//‰PNG
#define SIGN_JPG_BEGIN	0xD8FF			//Сигнатура начала
#define SIGN_JPG_END	0xD9FF			//Сигнатура конца
#define SIGN_JPG		(SIGN_JPG_BEGIN+(SIGN_JPG_END<<16))
#define SIGN_GIF		0x00464947		//GIF
#define SIGN_BMP		0x00004D42		//BM

int CreateDirFull(LPCTSTR);
int CreateFileFull(LPCTSTR);
std::string CreateRandomNameA(DWORD dwNumChars, LPCSTR ptPrefix="", LPCSTR ptPostfix="");
std::wstring CreateRandomNameW(DWORD dwNumChars, LPCWSTR ptPrefix=L"", LPCWSTR ptPostfix=L"");

LPCTSTR GetExtention(LPCTSTR ptName);
std::tstring ExpandPath(LPCTSTR ptPath);
BOOL CopyFromResource(LPCTSTR tDest, UINT uRes);
HANDLE CopyFromResourceTemp(LPCTSTR tDest, UINT uRes);

VOID ShowError(UINT uReason, LPCTSTR tStr);

#define ER_RESOURCE_NOT_FOUND	0
#define ER_CANT_LOCK_RESOURCE	1
#define ER_CANT_CREATE_FILE		2
#define ER_CANT_WRITE_FILE		3
#define ER_USER_ABORT			4

extern LPCTSTR error_reasons[];

#define RANDOM_NAME_MAX_CHARS 50

#define PLACE_IN_BORDERS(_NUM_, _MIN_, _MAX_)\
{if(_NUM_ < _MIN_) _NUM_ = _MIN_; if(_NUM_ > _MAX_) _NUM_ = _MAX_;}
//слишком длинная
#define SET_IN_RANGE(_NUM_, _MIN_, _MAX_)\
{if(_NUM_ < _MIN_) _NUM_ = _MIN_; if(_NUM_ > _MAX_) _NUM_ = _MAX_;}

std::string WcharToChar(LPCWSTR str, UINT codepage, int nWchars = -1);
std::string CharToChar(LPCSTR str, UINT codepage_from, int nChars = -1, UINT codepage_to = 1251);
std::wstring CharToWchar(LPCSTR str, UINT codepage, int nChars = -1);
std::wstring WcharToWchar(LPCWSTR str, UINT fake_codepage, int nChars = -1);

LPSTR WcharToCharBuff(LPCWSTR str, UINT codepage, int nWchars = -1);
LPSTR CharToCharBuff(LPCSTR str, UINT codepage_from,  int nChars = -1, UINT codepage_to = 1251);
LPWSTR CharToWcharBuff(LPCSTR str, UINT codepage, int nChars = -1);
LPWSTR WcharToWcharBuff(LPCWSTR str, UINT fake_codepage, int nChars = -1);

#ifdef _UNICODE
	#define TcharToChar		WcharToChar
	#define CharToTchar		CharToWchar
	#define TcharToCharBuff WcharToCharBuff
	#define CharToTcharBuff CharToWcharBuff
	#define TcharToWchar	WcharToWchar
	#define WcharToTchar	WcharToWchar
	#define TcharToWcharBuff	WcharToWcharBuff
	#define WcharToTcharBuff	WcharToWcharBuff
	#define CreateRandomName	CreateRandomNameW
#else
	#define TcharToChar		CharToChar
	#define CharToTchar		CharToChar
	#define TcharToCharBuff CharToCharBuff
	#define CharToTcharBuff CharToCharBuff
	#define TcharToWchar	CharToWchar
	#define WcharToTchar	WcharToChar
	#define TcharToWcharBuff	CharToWcharBuff
	#define WcharToTcharBuff	WcharToCharBuff
	#define CreateRandomName	CreateRandomNameA
#endif
UINT ReplaceSubstr(std::wstring& str, LPCWSTR substr, LPCWSTR newsubstr);
UINT AddLogEntry(LPCTSTR ptFile, LPCTSTR ptEntry);

VOID SetValidDate(SYSTEMTIME* st);
extern const UINT month_days[];

std::tstring GetFileName(std::tstring file_path);

template<class T>
struct ts_state
{
public:
	ts_state(const T& t):
	state(t)
	{
		InitializeCriticalSection(&cs);		
	}
	ts_state()
	{
		InitializeCriticalSection(&cs);		
	}
	virtual ~ts_state()
	{
		DeleteCriticalSection(&cs);
	}
	T operator=(const T t)
	{
		Sentinel sen(&cs);
		state = t;
		return state;
	}
	BOOL operator==(const T t)
	{
		Sentinel sen(&cs);
		return (state == t);
	}
	BOOL operator!=(const T t)
	{
		Sentinel sen(&cs);
		return (state != t);
	}
	operator T()
	{
		Sentinel sen(&cs);
		return state;
	}
protected:
	T state;
	CRITICAL_SECTION cs;
};
void FTtoLT(FILETIME* ft);
void LTtoFT(FILETIME* ft);
void FTtoLT(ULONGLONG* ull);
void LTtoFT(ULONGLONG* ull);
ULONGLONG GetFT();
ULONGLONG GetLT();
std::tstring FTtoString(ULONGLONG ull);

BOOL VistaAndHigher();
void OpenFolder(LPCTSTR folder);

std::tstring GetDlgItemString(HWND hDlg, UINT nControl); 

// Удобные функции, скрывают возню со структурами и множественные очевидные вызовы
LRESULT ComboBox_GetCurSelData(HWND hCB);
int ComboBox_GetSelByData(HWND hCB, LPARAM cb_data); // выберет первый подходящий
BOOL ComboBox_SetSelByData(HWND hCB, LPARAM cb_data); // установит выбранным первый подходящий

std::tstring GetLVItemText(HWND hLV, int item, int subitem);
LPARAM ListView_GetItemParam(HWND hLV, int item_index);
BOOL ListView_SetCheckByParam(HWND hLV, LPARAM param);
BOOL ListView_GetCheckByParam(HWND hLV, LPARAM param);
int ListView_InsertItemWithParam(HWND hLV, int item_index, LPARAM param);
void ListView_AddColumn(HWND hLV, LPCTSTR caption, int subitem, int width, int order = -1);
void ListView_Activate(HWND hDlg, HWND hLV, INT id);
int ListView_GetItemByParam(HWND hLV, LPARAM param);
class FileAction
{
public:
	virtual ~FileAction(){};
	virtual BOOL Proceed(LPCTSTR full_file_name, //полный путь к файлу
				LPCTSTR file_name) = 0; // указатель на имя файла
};

BOOL IsDirectory(LPCTSTR file);
//ExecuteFolder
BOOL CrawlFolder(LPCTSTR first_dir, unsigned max_depth, unsigned cur_depth, FileAction* file_action);

class cmd_option
{
public:
	cmd_option(LPCTSTR option_name, bool paramized, std::tstring descr, std::list<std::tstring> ac_list = std::list<std::tstring>());
	std::tstring GetName();
	bool IsParamized();
	bool Set(LPCTSTR param = NULL);
	void Reset();
	bool IsSet();
	int GetInt();
	std::tstring GetString();
	std::tstring GetOptionDescription();
protected:
	std::tstring _option_name;
	bool is_set;
	const bool _paramized;
	std::tstring _descr;
	std::tstring _param;
	std::list<std::tstring> _acceptable;
};

class CmdLine
{
public:
	CmdLine();
	virtual ~CmdLine();
	operator bool(); // корректно ли выставлены опции
	bool AddOption(	LPCTSTR option_name, // полное имя опции
					bool param, // параметризирована ли опция
					std::tstring descr, // описание опции
					std::list<std::tstring> ac_list = std::list<std::tstring>()); // список допустимых параметров
	bool DeleteOption(LPCTSTR option_name);
	bool SetCmd(LPCTSTR cmd_line);
	void ResetCmd();
	bool Set(LPCTSTR option_name, LPCTSTR param = NULL);
	bool Reset(LPCTSTR option_name);
	bool IsSet(LPCTSTR option_name);
	int GetInt(LPCTSTR option_name);
	std::tstring GetString(LPCTSTR option_name);
	void ShowUsage(); // только опции, без шаблона вызова
protected:
	cmd_option* GetOption(LPCTSTR option_name);
	std::list<cmd_option*> options_list;
	bool is_set;
};

#define SOFTWARE_KEY	_T("SOFTWARE")

DWORD GetDWORDOption(LPCTSTR app_name, LPCTSTR opt_name, DWORD default_val, BOOL global = FALSE); 
ULONGLONG GetQWORDOption(LPCTSTR app_name, LPCTSTR opt_name, ULONGLONG default_val, BOOL global = FALSE); 
std::tstring GetStringOption(LPCTSTR app_name, LPCTSTR opt_name, LPCTSTR default_val, BOOL global = FALSE); 
BOOL SetDWORDOption(LPCTSTR app_name, LPCTSTR opt_name, DWORD opt_val, BOOL global = FALSE);
BOOL SetQWORDOption(LPCTSTR app_name, LPCTSTR opt_name, ULONGLONG opt_val, BOOL global = FALSE);
BOOL SetStringOption(LPCTSTR app_name, LPCTSTR opt_name, LPCTSTR opt_val, BOOL global = FALSE);

//WM_DROPFILES
std::vector<std::tstring> GetDroppedFiles(HWND hWnd, HDROP hDrop);
UINT GetRandomNumber(UINT start = 0, UINT end = 100);