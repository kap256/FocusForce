using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace FFDll
{
    public class Hook
    {

        /// <summary>
        /// <param name="wParam">新しくフォーカスを取得したウインドウハンドル(NULL有)</param>
        /// <param name="lParam">未使用</param>
        /// </summary>
        const UInt32 WM_KILLFOCUS = 0x0008;

        /// <summary>
        /// メッセージフィルタのフックID
        /// <param name="wParam">未使用</param>
        /// <param name="lParam">MSG構造体へのポインタ</param>
        /// </summary>
        const Int32 WH_MSGFILTER = -1;        /// <summary>

        /// メッセージフィルタのフックID
        /// <param name="wParam">スレッド判断用の値</param>
        /// <param name="lParam">CWPSTRUCT構造体へのポインタ</param>
        /// </summary>
        const Int32 WH_CALLWNDPROC = 4;


        /// <summary>
        /// CWPSTRUCT構造体　https://docs.microsoft.com/ja-jp/windows/win32/api/winuser/ns-winuser-cwpstruct
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        public struct CWPSTRUCT
        {
            public UInt32 lParam;
            public UInt32 wParam;
            public UInt32 message;
            public IntPtr hwnd;
        }

        /// <summary>
        /// MSG構造体　https://docs.microsoft.com/ja-jp/windows/win32/api/winuser/ns-winuser-msg
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        public struct MSG
        {
            public IntPtr hwnd;
            public UInt32 message;
            public UInt32 wParam;
            public UInt32 lParam;
            public UInt32 time;
            public POINT pt;
            public UInt32 lPrivate;
        }

        /// <summary>
        /// POINT構造体　https://docs.microsoft.com/en-us/previous-versions/dd162805(v=vs.85)
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        public struct POINT
        {
            public Int32 x;
            public Int32 y;
        }

        /// <summary>
        /// フックプロシージャのデリゲート
        /// </summary>
        public delegate IntPtr HookCallback(Int32 nCode, UInt32 wParam, ref CWPSTRUCT lParam);


        /// <summary>
        /// アプリケーション定義のフックプロシージャをフックチェーン内にインストールします。
        /// フックプロシージャをインストールすると、特定のイベントタイプを監視できます。
        /// 監視の対象になるイベントは、特定のスレッド、または呼び出し側スレッドと同じデスクトップ内のすべてのスレッドに関連付けられているものです。
        /// </summary>
        [DllImport("user32.dll",SetLastError =true)]
        public static extern IntPtr SetWindowsHookEx(Int32 idHook, HookCallback lpfn, IntPtr hMod, UInt32 dwThreadId);

        /// <summary>
        /// 現在のフックチェーン内の次のフックプロシージャに、フック情報を渡します。
        /// フックプロシージャは、フック情報を処理する前でも、フック情報を処理した後でも、この関数を呼び出せます。
        /// </summary>
        [DllImport("user32.dll", SetLastError = true)]
        public static extern IntPtr CallNextHookEx(IntPtr hhk, Int32 hwnd, UInt32 wParam, ref CWPSTRUCT lParam);

        /// <summary>
        /// SetWindowsHookEx 関数を使ってフックチェーン内にインストールされたフックプロシージャを削除します。
        /// </summary>
        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool UnhookWindowsHookEx(IntPtr hhk);


        /// <summary>
        /// LastErrorを読みやすい形にします。
        /// </summary>
        [DllImport("kernel32.dll")]
        static extern uint FormatMessage(uint dwFlags, IntPtr lpSource, uint dwMessageId, uint dwLanguageId, StringBuilder lpBuffer, int nSize, IntPtr Arguments);

        /// <summary>
        /// LastErrorを読みやすい形にします。
        /// </summary>
        [DllImport("kernel32.dll")]
        static extern IntPtr LoadLibrary(string dll_name);


        /// <summary>
        /// フックハンドル
        /// </summary>
        static IntPtr HHook = IntPtr.Zero;


        /// <summary>
        /// フックを開始します。
        /// </summary>
        public static bool HookStart()
        {
            if (HHook != IntPtr.Zero) {
                return true;
            }

            IntPtr h = Marshal.GetHINSTANCE(typeof(Hook).Assembly.GetModules()[0]);
            IntPtr mar = LoadLibrary("user32.dll");
            HHook = SetWindowsHookEx(WH_CALLWNDPROC, OnHook,mar, 0);

            if (HHook == IntPtr.Zero) {

                ThrowError();
                return false;
            }
            return true;

        }

        /// <summary>
        /// フックを終了します。
        /// </summary>
        public static void HookStop()
        {
            if (HHook != IntPtr.Zero) {
                var result = UnhookWindowsHookEx(HHook);
                if (result) {
                    HHook = IntPtr.Zero;
                } else {
                    ThrowError();
                }
            }
        }


        /// <summary>
        /// フックプロシージャ本体
        /// </summary>
        protected static IntPtr OnHook(int nCode, uint wParam, ref CWPSTRUCT lParam)
        {
            /*
            if (lParam.message == WM_KILLFOCUS) {
                return IntPtr.Zero;
            }
            */

            return CallNextHookEx(HHook, nCode, wParam, ref lParam);
        }

        /// <summary>
        /// エラーが発生したため、LastErrorを取得して例外にします。
        /// </summary>
        private static void ThrowError()
        {
            int errCode = Marshal.GetLastWin32Error();

            StringBuilder message = new StringBuilder(255);

            FormatMessage(
              0x00001000,
              IntPtr.Zero,
              (uint)errCode,
              0,
              message,
              message.Capacity,
              IntPtr.Zero);

            throw new Exception($"{message.ToString()}({errCode:X8})");
        }
    }
}
