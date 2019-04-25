#include "UniversalSubclassing.h"


wnd_proc::wnd_proc()
{
	_proc = NULL;
	_old_proc = NULL;
}
void wnd_proc::Subclass(HWND hWnd, EXT_WNDPROC proc)
{
	SetProc(proc);
	SetProp(hWnd, WND_PROC_PROP, (HANDLE)this);
	SetOld((WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)UniversalWndProc));
}
EXT_WNDPROC wnd_proc::SetProc(EXT_WNDPROC proc)
{
	EXT_WNDPROC prev = _proc;
	_proc = proc;
	return prev;
}
WNDPROC wnd_proc::SetOld(WNDPROC old_proc)
{
	WNDPROC prev = _old_proc;
	_old_proc = old_proc;
	return prev;
}
LRESULT wnd_proc::operator()(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT res;
	if(_proc != NULL)
		if(_proc(hWnd, uMsg, wParam, lParam, &res))
			return res;
	if(_old_proc != NULL)
		return CallWindowProc(_old_proc, hWnd, uMsg, wParam, lParam);
	return (LRESULT)0;
}

LRESULT CALLBACK UniversalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	wnd_proc* wp = (wnd_proc*)GetProp(hWnd, WND_PROC_PROP);
	if(wp == NULL)
		return (LRESULT)0; // It should return something, right?
	return (*wp)(hWnd, uMsg, wParam, lParam); // calling () operator of wnd_proc
}