using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
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

        /// <summary>
        /// アプリケーションのメイン エントリ ポイントです。
        /// </summary>
        [STAThread]
        static void Main()
        {
            try {
                StartHook();
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Application.Run(new Form1());
            } catch (Exception e) {
                MessageBox.Show(e.Message);
            } finally {
                EndHook();

            }
        }
    }
}
