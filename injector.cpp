#include <windows.h>
#include <Dbghelp.h>
#include <iostream>
#include <tlhelp32.h>
#include "injector.h"

#pragma comment(lib, "Dbghelp.lib")
#pragma warning(disable: 4311 4312)

using namespace std;

PVOID GetImportFuncAddr(PVOID pBase, LPCSTR lpModuleName, LPCSTR lpFuncName)
{
	ULONG lSectionSize;
	PVOID pSection = ImageDirectoryEntryToData(pBase, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &lSectionSize); 
	if(!pSection)
		return NULL;
	if(IsBadReadPtr(pSection, lSectionSize))
		return NULL;
	IMAGE_IMPORT_DESCRIPTOR* pImportDescriptor = (IMAGE_IMPORT_DESCRIPTOR*)pSection;
	while(pImportDescriptor->Name)
	{
		PCHAR name = (PCHAR)(pImportDescriptor->Name+(DWORD)pBase);
		if(!strcmp(lpModuleName, (PCHAR)(pImportDescriptor->Name+(DWORD)pBase)))
			break;
		pImportDescriptor++;
	}
	if(!pImportDescriptor->Name)
		return NULL;
	IMAGE_THUNK_DATA* pThunkData = (IMAGE_THUNK_DATA*)(pImportDescriptor->OriginalFirstThunk+(DWORD)pBase);
	UINT i = 0;
	while(pThunkData->u1.AddressOfData)
	{
		if(!IMAGE_SNAP_BY_ORDINAL(pThunkData->u1.Ordinal))
		{
			IMAGE_IMPORT_BY_NAME* p = (IMAGE_IMPORT_BY_NAME*)(pThunkData->u1.AddressOfData+(DWORD)pBase);
			if(!strcmp((PCHAR)&p->Name, lpFuncName))
				break;
		}
		pThunkData++;
		i++;
	}
	if(!pThunkData->u1.AddressOfData)
		return NULL;
	IMAGE_THUNK_DATA* pThunkDataA = (IMAGE_THUNK_DATA*)(pImportDescriptor->FirstThunk+(DWORD)pBase);
	return (PVOID)&pThunkDataA[i].u1.AddressOfData;
}
SwapFunc::SwapFunc(LPCSTR lpModuleName, LPCSTR lpFuncName, SIZE_T dwNew, PVOID pBase):
	swp(NULL)
{
	PVOID pvImport = GetImportFuncAddr(pBase, lpModuleName, lpFuncName);
	if(!pvImport)
		throw(ER_NO_FUNC());
	swp = new SwitchPair(pvImport, dwNew);
}
SwapFunc::~SwapFunc()
{
	if(swp)
		delete swp;
}
SwapFunc::operator BOOL()
{
	if(swp)
		return TRUE;
	return FALSE;
}
SwapFuncEvery::SwapFuncEvery(LPCSTR lpModuleName, LPCSTR lpFuncName, SIZE_T dwNew)
{
	HANDLE hModules = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	if(hModules == INVALID_HANDLE_VALUE)
		return;
	MODULEENTRY32 me;
	me.dwSize = sizeof(MODULEENTRY32);
	if(!Module32First(hModules, &me))
	{
		CloseHandle(hModules);
		return;
	}
	do
	{
		try
		{
			SwapFunc* s = new SwapFunc(lpModuleName, lpFuncName, dwNew, me.modBaseAddr);
			sf.push_back(s);
		}
		catch(ER_NO_FUNC)
		{
			
			//при исключении в конструкторе - ничего не делаем, в список sf просто не добавится элемент
		}
	}while(Module32Next(hModules, &me));	
	if(sf.size())
		pOriginal = (*sf.begin())->swp->pOriginal;
}
SwapFuncEvery::~SwapFuncEvery()
{
	for(std::list<SwapFunc*>::reverse_iterator i = sf.rbegin(); i != sf.rend(); i++)
		delete (*i);	
}
SwapFuncEvery::operator BOOL()
{
	return BOOL(sf.size());
}