#pragma once
#include <Dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")
#pragma warning(disable: 4311 4312)

struct SwitchPair
{
	PVOID pvImportCell;
	PVOID   pOriginal;
	PVOID   pFake;
	SwitchPair(PVOID addr, PVOID sw):pvImportCell(addr), pFake(sw)
	{
		pOriginal = *(PVOID*)pvImportCell;
		SetFake();
	}
	~SwitchPair()
	{
		SetOriginal();
	}
	VOID SetOriginal()
	{
		*(PVOID*)pvImportCell = pOriginal;
	}
	VOID SetFake()
	{
		*(PVOID*)pvImportCell = pFake;
	}
};
PVOID GetImportFuncAddr(PVOID pBase, LPCSTR lpModuleName, LPCSTR lpFuncName)
{
	ULONG lSectionSize;
	PVOID pSection = ImageDirectoryEntryToData(pBase, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &lSectionSize); 
	if(IsBadReadPtr(pSection, lSectionSize))
		return NULL;
	IMAGE_IMPORT_DESCRIPTOR* pImportDescriptor = (IMAGE_IMPORT_DESCRIPTOR*)pSection;
	while(pImportDescriptor->Name)
	{
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
class SwapFunc //ќбЄртка над SwitchPair
{
public:
	SwapFunc(LPCSTR lpModuleName, LPCSTR lpFuncName, PVOID dwNew, PVOID pBase);
	~SwapFunc();
	operator BOOL();
	SwitchPair* swp;
};
SwapFunc::SwapFunc(LPCSTR lpModuleName, LPCSTR lpFuncName, PVOID dwNew, PVOID pBase)
{
	PVOID pvImport = GetImportFuncAddr(pBase, lpModuleName, lpFuncName);
	if(!pvImport)
		return;
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