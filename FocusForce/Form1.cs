using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace FocusForce
{
    public partial class Form1 : Form
    {
        const String FILE_NAME = "FocusForce_list.conf";
        Encoding ENCODE = Encoding.GetEncoding("shift_jis");
        //少なくともウチの環境では、GetModuleFileNameの結果がSJisで帰ってきたため。


        #region 初期化-----------------------------------------
        public Form1()
        {
            ENCODE = Encoding.GetEncoding("shift_jis");

            InitializeComponent();
            pictureBox1.Image = Properties.Resources.icon.ToBitmap();
            label_build.Text = "Build : " + GetBuildDateStr();
            LoadTargetList();
#if !DEBUG
            HideForm();
#endif
        }



        public String GetBuildDateStr()
        {
            var asm = GetType().Assembly;
            var ver = asm.GetName().Version;
            
            var build = ver.Build;
            var revision = ver.Revision;
            var baseDate = new DateTime(2000, 1, 1);

            return baseDate.AddDays(build).AddSeconds(revision * 2).ToString("yyyy/MM/dd HH:mm");
        }

        #endregion

        #region 起動・終了-----------------------------------------
        private void Notify_Click(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left) {
                ShowForm();
            }
        }
        private void button_Exit_Click(object sender, EventArgs e)
        {
            SaveTargetList();
            this.Close();
        }
        private void button_ok_Click(object sender, EventArgs e)
        {
            SaveTargetList();
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
        #endregion

        #region ファイル操作-----------------------------------------
        private string GetFileName()
        {
            var app_data = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
            
            return app_data+Path.DirectorySeparatorChar+ FILE_NAME;
        }
        private void SaveTargetList()
        {
            var file = GetFileName();
            using (var writer = new StreamWriter(file,false, ENCODE)) {
                foreach (var target in listBox_target.Items) {
                    writer.WriteLine(target.ToString());
                }
            }
        }

        private void LoadTargetList()
        {
            var file = GetFileName();
            if (File.Exists(file)) {
                using (var reader = new StreamReader(file, ENCODE)) {
                    while (true) {
                        var line = reader.ReadLine();
                        if (line == null) {
                            break;
                        }
                        listBox_target.Items.Add(line);
                    }
                }
            }
        }
        #endregion

        #region ドラッグドロップ-----------------------------------------
        private void Form1_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop)) {
                e.Effect = DragDropEffects.Link;
            } else {
                e.Effect = DragDropEffects.None;
            }
        }

        private void Form1_DragDrop(object sender, DragEventArgs e)
        {
            string[] fileName =(string[])e.Data.GetData(DataFormats.FileDrop, false);
            textBox_Add.Text = fileName[0];
        }
        #endregion

        #region 対象リスト管理-----------------------------------------
        private void textBox_Add_TextChanged(object sender, EventArgs e)
        {
            button_Add.Enabled = !String.IsNullOrEmpty(textBox_Add.Text);
        }
        private void button_Add_Click(object sender, EventArgs e)
        {
            listBox_target.Items.Add(textBox_Add.Text);
            textBox_Add.Text = "";
        }
        private void listBox_target_SelectedIndexChanged(object sender, EventArgs e)
        {
            button_Delete.Enabled = listBox_target.SelectedIndex >= 0;
        }
        private void button_Delete_Click(object sender, EventArgs e)
        {
            var target_file = listBox_target.SelectedItem.ToString();
            textBox_Add.Text = target_file;
            listBox_target.Items.RemoveAt(listBox_target.SelectedIndex);
        }
        #endregion
        
    }
}
