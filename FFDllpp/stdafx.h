// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "targetver.h"
#include <stdio.h>
#include <windows.h>
#include <dbghelp.h>
#include <shlobj_core.h>

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーから使用されていない部分を除外します。
// Windows ヘッダー ファイル:
#include <windows.h>
#include <tchar.h>
#include <string>

#ifdef _UNICODE
typedef std::wstring TStr;
#else
typedef std::string TStr;
#endif


// TODO: プログラムに必要な追加ヘッダーをここで参照してください
#include "FFDll.h"
