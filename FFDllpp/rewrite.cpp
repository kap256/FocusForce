//////////////////////////////////////////////////////////////////////////
//
//  rewrite.cpp @ https://qiita.com/kobake@github/items/8d3d3637c7af0b270098
//
//////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#pragma comment(lib,"Dbghelp.lib")

void* RewriteFunctionImp(const char* szRewriteModuleName, const char* szRewriteFunctionName, void* pRewriteFunctionPointer)
{
	for (int i = 0; i < 2; i++) {
		// ベースアドレス
		DWORD dwBase = 0;
		if (i == 0) {
			if (szRewriteModuleName) {
				dwBase = (DWORD)(intptr_t)::GetModuleHandleA(szRewriteModuleName);
			}
		} else if (i == 1) {
			dwBase = (DWORD)(intptr_t)GetModuleHandle(NULL);
		}
		if (!dwBase) {
			Print(_T("GetModuleHandle failed."));
			continue;
		}

		// イメージ列挙
		ULONG ulSize;
		PIMAGE_IMPORT_DESCRIPTOR pImgDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData((HMODULE)(intptr_t)dwBase, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);
		for (; pImgDesc->Name; pImgDesc++) {
			const char* szModuleName = (char*)(intptr_t)(dwBase + pImgDesc->Name);
			// THUNK情報
			PIMAGE_THUNK_DATA pFirstThunk = (PIMAGE_THUNK_DATA)(intptr_t)(dwBase + pImgDesc->FirstThunk);
			PIMAGE_THUNK_DATA pOrgFirstThunk = (PIMAGE_THUNK_DATA)(intptr_t)(dwBase + pImgDesc->OriginalFirstThunk);
			// 関数列挙
			for (; pFirstThunk->u1.Function; pFirstThunk++, pOrgFirstThunk++) {
				if (IMAGE_SNAP_BY_ORDINAL(pOrgFirstThunk->u1.Ordinal))continue;
				PIMAGE_IMPORT_BY_NAME pImportName = (PIMAGE_IMPORT_BY_NAME)(intptr_t)(dwBase + (DWORD)pOrgFirstThunk->u1.AddressOfData);

				if (!szRewriteFunctionName) {
					// 表示のみ
					_sntprintf_s(buf, buf_size, _T("Module:%hs Hint : %d, Name : %hs"), szModuleName, pImportName->Hint, pImportName->Name);
					Print(buf);
				} else {
					// 書き換え判定
					if (_stricmp((const char*)pImportName->Name, szRewriteFunctionName) != 0)continue;

					// 保護状態変更
					DWORD dwOldProtect;
					if (!VirtualProtect(&pFirstThunk->u1.Function, sizeof(pFirstThunk->u1.Function), PAGE_READWRITE, &dwOldProtect)) {
						Print(_T("VirtualProtect failed."));
						return NULL; // エラー
					}

					// 書き換え
					void* pOrgFunc = (void*)(intptr_t)pFirstThunk->u1.Function; // 元のアドレスを保存しておく
					WriteProcessMemory(GetCurrentProcess(), &pFirstThunk->u1.Function, &pRewriteFunctionPointer, sizeof(pFirstThunk->u1.Function), NULL);
					pFirstThunk->u1.Function = (DWORD)(intptr_t)pRewriteFunctionPointer;

					// 保護状態戻し
					VirtualProtect(&pFirstThunk->u1.Function, sizeof(pFirstThunk->u1.Function), dwOldProtect, &dwOldProtect);
					return pOrgFunc; // 元のアドレスを返す
				}
			}
		}
	}

	if (szRewriteFunctionName) {
		Print(_T("Function not found."));
	}
	return NULL;
}

void* RewriteFunction(const char* szRewriteModuleName, const char* szRewriteFunctionName, void* pRewriteFunctionPointer)
{
	return RewriteFunctionImp(szRewriteModuleName, szRewriteFunctionName, pRewriteFunctionPointer);
}

void PrintFunctions(const char* szRewriteModuleName)
{
	Print(_T("----"));
	RewriteFunctionImp(szRewriteModuleName, NULL, NULL);
	Print(_T("----"));
}