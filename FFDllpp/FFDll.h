#pragma once
#define EXPORT extern "C" __declspec(dllexport)

EXPORT bool StartHook(void);
EXPORT bool EndHook(void);
EXPORT LRESULT CALLBACK MyHookProc(int, WPARAM, LPARAM);


extern HMODULE gModule;
