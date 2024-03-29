#pragma once
#define EXPORT extern "C" __declspec(dllexport)

EXPORT bool StartHook(void);
EXPORT bool EndHook(void);

void Log(const TCHAR* message);
void Print(const TCHAR* message);

bool CheckIsTargetProc();

void CancelINACTIVE(LPCWPSTRUCT cwp);
void CancelACTIVATEAPP(LPCWPSTRUCT cwp);
void FixWindowPos(LPCWPSTRUCT cwp);

void OnAttach();
void OnDetach();

void* RewriteFunction(const char* szRewriteModuleName, const char* szRewriteFunctionName, void* pRewriteFunctionPointer);
void PrintFunctions(const char* szRewriteModuleName);

const int buf_size = 1024;
extern TCHAR buf[buf_size];
extern HMODULE gModule;

//////////////////////////////////////////////////////////
//  デバッグ出力関連

#ifdef _DEBUG

#define DSPrintf _sntprintf_s

#else

void DSPrintf(TCHAR* arg, ...);

#endif