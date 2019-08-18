#pragma once
#define EXPORT extern "C" __declspec(dllexport)

EXPORT bool StartHook(void);
EXPORT bool EndHook(void);

extern HMODULE gModule;

void OnAttach();
void OnDetach();

void Print(const TCHAR* message);

void* RewriteFunction(const char* szRewriteModuleName, const char* szRewriteFunctionName, void* pRewriteFunctionPointer);
void PrintFunctions();

const int buf_size = 512;
extern TCHAR buf[buf_size];