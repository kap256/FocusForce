#pragma once
#define EXPORT extern "C" __declspec(dllexport)

EXPORT bool StartHook(void);
EXPORT bool EndHook(void);

extern HMODULE gModule;
