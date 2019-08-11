// FFDllpp.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"

HHOOK hHook = nullptr;

void Print(const TCHAR* message)
{
	OutputDebugString(message);
	OutputDebugString(_T("\n"));
}
void OnError(const TCHAR* message)
{
	Print(message);

	const int buf_size = 512;
	TCHAR buf[buf_size];

	FormatMessage(
		0x00001000,
		NULL,
		GetLastError(),
		0,
		buf,
		buf_size,
		NULL);

	Print(message);

}

EXPORT bool StartHook()
{
	Print(_T("StartHook"));
	if (hHook != nullptr) {
		return true;
	}

	hHook = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)MyHookProc, gModule, 0);
	if (hHook == NULL) {
		OnError(_T("フックに失敗しました"));
		return false;
	}
	return true;
}



EXPORT bool EndHook()
{
	Print(_T("EndHook"));
	if (hHook == nullptr) {
		return true;
	}

	if (UnhookWindowsHookEx(hHook) == 0) {
		OnError(_T("フックの解除に失敗しました"));
		return false;
	}
	hHook = nullptr;
	return true;
}


LRESULT CALLBACK MyHookProc(int nCode, WPARAM wp, LPARAM lp)
{
	Print(_T("myhook"));
	LPCWPSTRUCT cwp = (LPCWPSTRUCT)(lp);

	if (cwp->message == WM_KILLFOCUS) {
		Print(_T("WM_KILLFOCUS"));
		Beep(440, 200);
		return 0;
	}

	return CallNextHookEx(hHook, nCode, wp, lp);
}