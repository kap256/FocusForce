﻿using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace FocusForce
{
    static class Program
    {
        [DllImport("FFDllpp.dll")]
        static extern bool StartHook();

        [DllImport("FFDllpp.dll")]
        static extern bool EndHook();

        const string mutexName = "FF32_756e7162-a066-4e77-91c7";

        /// <summary>
        /// アプリケーションのメイン エントリ ポイントです。
        /// </summary>
        [STAThread]
        static void Main()
        {
            Process proc64 = null;
            Mutex mutex = new System.Threading.Mutex(false, mutexName);
            bool hasHandle = false;
            try {
                //多重起動の抑止
                try {
                    hasHandle = mutex.WaitOne(0, false);
                } catch (AbandonedMutexException) {
                    hasHandle = true;
                }

                if (hasHandle == false) {
                    return;
                }

                //フック開始
                StartHook();

                //64bitプロセスの起動
                try {
                    proc64 = Process.Start("FF64.exe");
                } catch {
                    //まあいいや。
                }

                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Application.Run(new Form1());
            } catch (Exception e) {
                MessageBox.Show(e.Message);
            } finally {
                try {
                    if (proc64 != null) {
                        proc64.Kill();
                    }
                } catch {
                    //まあいいや。
                }

                EndHook();

                if (hasHandle) {
                    mutex.ReleaseMutex();
                }
                mutex.Close();

            }
        }
    }
}
