// FFDllpp.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"

HHOOK hHook = nullptr;

const int buf_size = 512;
TCHAR buf[buf_size];

void Print(const TCHAR* message)
{
	OutputDebugString(message);
	OutputDebugString(_T("\n"));
}
void OnError(const TCHAR* message)
{
	Print(message);


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



LRESULT CALLBACK GetMessageProc(int nCode, WPARAM wp, LPARAM lp)
{

	if (nCode < 0) {// do not process message 
		return CallNextHookEx(hHook, nCode, wp, lp);
	}

	LPMSG msg = (LPMSG)(lp);
	_sntprintf_s(buf, buf_size, _T("message:%x"), msg->message);
	Print(buf);

	if (msg->message == WM_ACTIVATE) {
		Print(_T("WM_ACTIVATE"));
		Beep(440, 200);

		msg->wParam = (msg->wParam & 0xFFFF0000) + 0x01;
		//return 0;
	}

	return CallNextHookEx(hHook, nCode, wp, lp);
}

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wp, LPARAM lp)
{

	if (nCode < 0) {// do not process message 
		return CallNextHookEx(hHook, nCode, wp, lp);
	}

	LPCWPSTRUCT cwp = (LPCWPSTRUCT)(lp);
	_sntprintf_s(buf, buf_size, _T("message:%x"), cwp->message);
	Print(buf);

	if (cwp->message == WM_ACTIVATE) {
		Print(_T("WM_ACTIVATE"));
		Beep(440, 200);

		cwp->wParam = (cwp->wParam & 0xFFFF0000) + 0x01;
		//return 0;
	}

	return CallNextHookEx(hHook, nCode, wp, lp);
}



EXPORT bool StartHook()
{
	Print(_T("StartHook"));
	if (hHook != nullptr) {
		return true;
	}

	//hHook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)GetMessageProc, gModule, 0);
	hHook = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)CallWndProc, gModule, 0);
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