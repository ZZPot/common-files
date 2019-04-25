#pragma once
#include <windows.h>
/*
	returning result in last parameter (for dialog controls)
	Return bool: true if LRESULT should supersede result value of original function. Original function won't be called
				 false if original function should be called and it's result will be returned as result of message procession.
*/
typedef bool (*EXT_WNDPROC)(HWND, UINT, WPARAM, LPARAM, LRESULT*); 
#define WND_PROC_PROP	"wnd_proc pointer"

class wnd_proc
{
public:
	wnd_proc();
	virtual ~wnd_proc(){};
	void Subclass(HWND hWnd, EXT_WNDPROC proc);
	EXT_WNDPROC SetProc(EXT_WNDPROC proc);
	WNDPROC SetOld(WNDPROC old_proc);
	LRESULT operator()(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	EXT_WNDPROC _proc;
	WNDPROC _old_proc;
};


LRESULT CALLBACK UniversalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);