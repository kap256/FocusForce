// FFDllpp.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include <thread>
#include "mode.h"

//////////////////////////////////////////////////////////
//  ビルドオプション

#define _USE_WINDOW_POS_FIX_	//ウインドウが上に突き抜けた場合、位置修正する。

//////////////////////////////////////////////////////////
//  定数・変数

const TCHAR* FILE_NAME = _T("FocusForce_list.conf");	//定数共用したいなぁ、したい。

TCHAR buf[buf_size];

HHOOK hHook = nullptr;
HWND hWnd = nullptr;
CFFMode Mode;

//////////////////////////////////////////////////////////
//  デバッグ出力関連

#ifdef _DEBUG
	//ヘッダでマクロ指定済み
#else

void DSPrintf(TCHAR* arg, ...)
{
	//何もしない
}

#endif

void Log(const TCHAR* message)
{
#ifdef _DEBUG
	FILE* pFile;
	if (pFile = _tfopen(_T("E:\\fflog.log"), _T("at"))) {
		time_t now = time(NULL);
		struct tm *pnow = localtime(&now);

#ifdef _M_AMD64
		const TCHAR* CPU = _T("x64");
#else
		const TCHAR* CPU = _T("x86");
#endif

		_ftprintf(pFile, _T("%4d/%2d/%2d %2d:%2d:%2d (%6d) on %s : %s\n"),
			pnow->tm_year + 1900,
			pnow->tm_mon + 1,
			pnow->tm_mday,
			pnow->tm_hour,
			pnow->tm_min,
			pnow->tm_sec,
			GetCurrentProcessId(),
			CPU,
			message);

		fclose(pFile);
	} else {
		Log(_T("open log file failed..."));
	}
	Print(message);
#endif 
}

void Print(const TCHAR* message)
{
#ifdef _DEBUG
	OutputDebugString(message);
	OutputDebugString(_T("\n"));
#endif 
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

	Print(buf);
}

//////////////////////////////////////////////////////////
//  処理対象プロセスかどうかの判定

bool CheckIsTargetProc()
{
	Log(_T("CheckIsTargetProc----------------"));
	//まずプロセス名を取得
	TCHAR proc_name[MAX_PATH];
	if (MAX_PATH == GetModuleFileName(NULL, proc_name, MAX_PATH)) {
		//プロセス名取得失敗
		Log(_T("proc name is too long..."));
		return false;
	}

	DSPrintf(buf, buf_size, _T("proc:%s"), proc_name);
	Log(buf);


	//処理対象リストファイル名を取得
	TCHAR folder_path[MAX_PATH];
	if (!SHGetSpecialFolderPath(NULL, folder_path, CSIDL_APPDATA, 0)) {
		//roamingのパスが分からない。
		Log(_T("AppData/Roaming not found..."));
		return true;
	}
	TStr conf_file = folder_path;
	conf_file.append(_T("\\"));
	conf_file.append(FILE_NAME);

	DSPrintf(buf, buf_size, _T("conf_file:%s"), conf_file.c_str());
	Print(buf);

	//ファイルを開いて比較していく。
	FILE* pFile;

	bool is_default = true;
	if (pFile = _tfopen(conf_file.c_str(), _T("rt"))) {
		const int LINE_SIZE = 512;
		TCHAR line[LINE_SIZE];
		while (_fgetts(line, LINE_SIZE, pFile)) {

			size_t line_len;
			TCHAR* split =  _tcschr(line, DELIMITER);
			if (split) {
				//区切り文字があった場合
				line_len = split - line;
				split++;
			} else {
				//区切り文字が無かった場合
				line_len = _tcslen(line) - 1;
			}

			line[line_len] = _T('\0');	//手編集しなければこれでいいハズヨ。
			DSPrintf(buf, buf_size, _T("line(%d):%s"), line_len, line);
			Log(buf);

			if (line_len>0 && _tcsncmp(line, proc_name, line_len) == 0) {
				DSPrintf(buf, buf_size, _T("hit:%s"), line);
				Log(buf);
				is_default = false;
				Mode.FromString(split);
				break;
			}
		}

		fclose(pFile);
	} else {
		Log(_T("open conf_file failed..."));
	}

	if (is_default) {
		//デフォルト動作（CFFModeのコンストラクタで初期化するので対応不要）
		Log(_T("default mode."));
	}
	Log(_T("----------------"));

	return (Mode.mFixWinPos) || (Mode.mCancelInactive);
}

//////////////////////////////////////////////////////////
//  フック処理

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wp, LPARAM lp)
{

	if (nCode < 0) {// do not process message 
		return CallNextHookEx(hHook, nCode, wp, lp);
	}

	LPCWPSTRUCT cwp = (LPCWPSTRUCT)(lp);

	DSPrintf(buf, buf_size, _T("message:%x wp:%x"), cwp->message, (cwp->wParam & 0xFFFF));
	Print(buf);

	//メッセージを取得
	switch (cwp->message) {
		case  WM_ACTIVATE:
		{
			hWnd = cwp->hwnd;	//GetForegroundWindowの偽装に用いる。
			if ((cwp->wParam & 0xFFFF) == WA_INACTIVE) {
				//INACTIVEになったので、必要なら打ち消し処理を行う。
				CancelINACTIVE(cwp);
			} else {
				//ACTIVEになった。必要ならウインドウ位置修正を行う
				FixWindowPos(cwp);
			}
		}
		break;

		case  WM_ACTIVATEAPP:
		{
			if (cwp->wParam == FALSE) {
				//WM_ACTIVATEAPP = falseなので、必要なら打ち消し処理を行う。
				CancelACTIVATEAPP(cwp);
			} 
		}
		break;

		case  WM_SIZE:
		{
			//WM_ACTIVATEだけだと不十分みたいなのだが、どのメッセージをとればいいのかな……
			//　× - WM_MOVE
			//　× - WM_SHOWWINDOW
			FixWindowPos(cwp);
		}
		break;

	}

	return CallNextHookEx(hHook, nCode, wp, lp);
}


EXPORT bool StartHook()
{
	Print(_T("StartHook"));
	if (hHook != nullptr) {
		return true;
	}

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
//////////////////////////////////////////////////////////
//  ウインドウ位置の修正

int GetWidth(RECT* rc) { return rc->right - rc->left; }
int GetHeight(RECT* rc) { return rc->bottom - rc->top; }

bool IsFix(RECT* win, RECT* mon)
{
	//ウインドウのほうが大きい場合は何か事情があるかもしれないので無視。
	//	-> =も追加。どうもタスクバーもウインドウ扱いらしく、ときどき真ん中に来たりする。
	if (GetWidth(win) >= GetWidth(mon)) {
		return false;
	}
	if (GetHeight(win) >= GetHeight(mon)) {
		return false;
	}

	//ウインドウ上端がモニターに収まっていなければ位置修正する。
	if (win->top < mon->top || mon->bottom < win->top) {
		return true;
	}
	//とりあえずウインドウの上がつかめればいい（マウスで移動できる）ので、横は見なくていいか。
	//縦並びのマルチモニタだと問題になるかも。わが家の環境では考慮しなくていいけど……。

	//位置修正対象ではなかった。
	return false;
}

struct MyMonitorEnumParam
{
	LPCWPSTRUCT cwp;
	LPRECT win_rect;
};

BOOL CALLBACK MonitorEnumProc(HMONITOR hMon, HDC hDC, LPRECT dispRect, LPARAM lp);

void FixWindowPos(LPCWPSTRUCT cwp)
{
#ifndef _USE_WINDOW_POS_FIX_
	return;
#endif

	if (!Mode.mFixWinPos) {// 処理対象ではない
		return;	
	}
	Print(_T("FixWindowPos"));

	if (GetParent(cwp->hwnd) != NULL) {	//トップレベルウインドウではない（エラーの可能性もあるけど…）
		return;
	}

	//モニター情報を鑑みて、修正の必要な位置・サイズであるかどうか？
	RECT win_rect;
	GetWindowRect(cwp->hwnd, &win_rect);
	MyMonitorEnumParam param = { cwp,&win_rect };

	EnumDisplayMonitors(NULL, &win_rect, &MonitorEnumProc, (LPARAM)&param);
	//↓コールバックに続く……
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMon, HDC hDC, LPRECT dispRect, LPARAM lp)
{
	MyMonitorEnumParam* mp = (MyMonitorEnumParam*)(lp);
	DSPrintf(buf, buf_size, _T("window left:%d top:%d"), mp->win_rect->left, mp->win_rect->top);
	Print(buf);

	//モニター情報取得(MONITORINFO.rcWorkで、タスクバー抜きの領域が取れるため)
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(hMon, &info);
	DSPrintf(buf, buf_size, _T("mon_work left:%d top:%d"), info.rcWork.left, info.rcWork.top);
	Print(buf);

	//位置修正対象なら真ん中に持ってくる。
	if (IsFix(mp->win_rect, &info.rcWork)) {

		Log(_T("Fix Window position"));
		auto new_x = (GetWidth(&info.rcWork) - GetWidth(mp->win_rect)) / 2 + info.rcWork.left;
		auto new_y = (GetHeight(&info.rcWork) - GetHeight(mp->win_rect)) / 2 + info.rcWork.top;

		SetWindowPos(
			mp->cwp->hwnd, 0,
			new_x, new_y,
			0, 0,
			SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
	}

	return FALSE;	//まあ一つ見れば十分かな……。
}

//////////////////////////////////////////////////////////
//メッセージの打ち消し

//別スレッドで少し待機した後、再度メッセージを投げる。
void SendDelayMessage(CWPSTRUCT& cwp)
{
	auto th = new std::thread(
		[](CWPSTRUCT cwp) {
		Sleep(100);
		SendMessage(cwp.hwnd, cwp.message, cwp.wParam, cwp.lParam);

		DSPrintf(buf, buf_size, _T("Send %x to %x (wp:%x,lp:%x)"), cwp.message, cwp.hwnd, cwp.wParam, cwp.lParam);
		Log(buf);
		; },
		cwp);

	th->detach();
}

//  WA_INACTIVEの打ち消し処理
void CancelINACTIVE(LPCWPSTRUCT cwp)
{
	if (!Mode.mCancelInactive) {// 処理対象ではない
		return;
	}
	Print(_T("CancelINACTIVE"));


	CWPSTRUCT new_cwp = *cwp;
	new_cwp.wParam = (cwp->wParam & 0xFFFF0000) | WA_ACTIVE;
	SendDelayMessage(new_cwp);
}

//  ACTIVATEAPP = false の打ち消し処理
void CancelACTIVATEAPP(LPCWPSTRUCT cwp)
{
	if (!Mode.mCancelInactive) {// 処理対象ではない
		return;
	}
	Print(_T("CancelACTIVATEAPP"));


	CWPSTRUCT new_cwp = *cwp;
	new_cwp.wParam = TRUE;
	SendDelayMessage(new_cwp);
}



//////////////////////////////////////////////////////////
//  GetForegroundWindowの偽装処理

typedef HWND(WINAPI *GFW)();
typedef BOOL(WINAPI *SCP)(int X, int Y);
typedef BOOL(WINAPI *CC)(const RECT *lpRect);
GFW pGetForegroundWindow = nullptr;
SCP pSetCursorPos = nullptr;
CC pClipCursor = nullptr;

HWND WINAPI MyGetForegroundWindow(VOID)
{
	if (hWnd) {
		return hWnd;
	}
	return pGetForegroundWindow();
}

BOOL WINAPI MySetCursorPos(_In_ int X, _In_ int Y)
{
	//握りつぶす。
	return TRUE;
}

BOOL WINAPI MyClipCursor(const RECT *lpRect)
{
	//握りつぶす。
	return pClipCursor(NULL);
}


void OnAttach()
{
	if (!Mode.IsAttach()) {// 処理対象ではない(関数差し替えからしない。)
		return;
	}

	Log(_T("OnAttach."));

#ifdef _DEBUG
	PrintFunctions("User32.dll");
#endif

	pGetForegroundWindow = (GFW)(RewriteFunction("User32.dll", "NtUserGetForegroundWindow", MyGetForegroundWindow));
	//pSetCursorPos = (SCP)(RewriteFunction("User32.dll", "NtUserSetCursorPos", MySetCursorPos));
	pClipCursor = (CC)(RewriteFunction("User32.dll", "NtUserClipCursor", MyClipCursor));

	if (pGetForegroundWindow) {
		Log(_T("Rewrite success."));

	} else {
		OnError(_T("Rewrite failed..."));
	}
}

void OnDetach()
{
	if (!Mode.IsAttach()) {// 処理対象ではない
		return;
	}

	if (pGetForegroundWindow) {
		RewriteFunction("User32.dll", "NtUserGetForegroundWindow", pGetForegroundWindow); // 戻す
		RewriteFunction("User32.dll", "NtUserSetCursorPos", pSetCursorPos); // 戻す
		RewriteFunction("User32.dll", "NtUserClipCursor", pClipCursor); // 戻す
	}
}
