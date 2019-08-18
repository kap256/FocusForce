#pragma once
#define EXPORT extern "C" __declspec(dllexport)

EXPORT bool StartHook(void);
EXPORT bool EndHook(void);

void Print(const TCHAR* message);

void OnAttach();
void OnDetach();

void* RewriteFunction(const char* szRewriteModuleName, const char* szRewriteFunctionName, void* pRewriteFunctionPointer);
void PrintFunctions(const char* szRewriteModuleName);

const int buf_size = 512;
extern TCHAR buf[buf_size];
extern HMODULE gModule;