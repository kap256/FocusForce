using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace FF64
{
    static class Program
    {
        [DllImport("FFDllpp64.dll")]
        static extern bool StartHook();

        [DllImport("FFDllpp64.dll")]
        static extern bool EndHook();
        
        const string mutexName = "FF64_3427bbd9-ae60-4f2b-b0d4";
  
        /// <summary>
        /// アプリケーションのメイン エントリ ポイントです。
        /// </summary>
        [STAThread]
        static void Main()
        {
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
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Application.Run();
            } catch (Exception e) {
                MessageBox.Show(e.Message);
            } finally {
                EndHook();

                if (hasHandle) {
                    mutex.ReleaseMutex();
                }
                mutex.Close();
            }

        }
    }
}
