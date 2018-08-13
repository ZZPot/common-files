#pragma once
#include <list>
struct ER_NO_FUNC{};
struct SwitchPair
{
	PVOID pvImportCell;
	SIZE_T   pOriginal;
	SIZE_T   pFake;
	DWORD oldProtect;
	BOOL MakeWriteable(PVOID pMem)
	{
		return VirtualProtect(pMem, sizeof(SIZE_T), PAGE_READWRITE, &oldProtect);
	}
	BOOL RestoreProtect(PVOID pMem)
	{
		DWORD d;
		return VirtualProtect(pMem, sizeof(SIZE_T), oldProtect, &d);
	}
	SwitchPair(PVOID addr, SIZE_T sw):pvImportCell(addr), pFake(sw)
	{
		pOriginal = *(PDWORD)pvImportCell;
		SetFake();
	}
	~SwitchPair()
	{
		SetOriginal();
	}
	VOID SetOriginal()
	{
		BOOL res = MakeWriteable(pvImportCell);
		*(SIZE_T*)pvImportCell = pOriginal;
		RestoreProtect(pvImportCell);
	}
	VOID SetFake()
	{
		BOOL res = MakeWriteable(pvImportCell);
		*(SIZE_T*)pvImportCell = pFake;
		RestoreProtect(pvImportCell);
	}
};
class SwapFunc //Обёртка над SwitchPair
{
public:
	SwapFunc(LPCSTR lpModuleName, LPCSTR lpFuncName, SIZE_T dwNew, PVOID pBase);
	~SwapFunc();
	operator BOOL();
	SwitchPair* swp;
};
class SwapFuncEvery //подмена функции во всех модулях
{
	std::list<SwapFunc*> sf;
public:
	SwapFuncEvery(LPCSTR lpModuleName, LPCSTR lpFuncName, SIZE_T dwNew);
	~SwapFuncEvery();
	SIZE_T pOriginal;
	operator BOOL();
};
//--*EXCEPTIONS*--
