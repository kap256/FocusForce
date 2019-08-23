using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace FocusForce
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            pictureBox1.Image = Properties.Resources.icon.ToBitmap();
#if !DEBUG
            HideForm();
#endif
        }


        private void Notify_Click(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right) {
                ShowForm();
            } else {
                this.Close();
            }
        }
        private void button_Exit_Click(object sender, EventArgs e)
        {
            this.Close();
        }
        private void button_ok_Click(object sender, EventArgs e)
        {
            HideForm();
        }


        private void ShowForm()
        {
            this.ShowInTaskbar = true;
            this.WindowState = FormWindowState.Normal;
        }

        private void HideForm()
        {
            this.ShowInTaskbar = false;
            this.WindowState = FormWindowState.Minimized;
        }
    }
}
