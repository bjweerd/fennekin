using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace WebDiver
{
    public partial class ConfigureEditorItem : Form
    {
        public ConfigureEditorItem()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.FileName = tbExeName.Text;
            dlg.Title = "Open XML Editor Programs";
            dlg.Filter = "Executable files (*.exe)|*.exe|All files (*.*)|*.*";
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                tbExeName.Text = dlg.FileName;
            }
            dlg.Dispose();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (tbMenuName.Text == "")
            {
                MessageBox.Show("Please specify a name for the editor.");
                return;
            }
            if (tbExeName.Text == "")
            {
                MessageBox.Show("Please specify an executable file for this editor.");
                return;
            }

            if (MainWindow.applicationData.tmpInAdd)
            {
                // okay, add it to the list
                ApplicationData.EditorItem item = new ApplicationData.EditorItem();
                item.menuName = tbMenuName.Text;
                item.exeFilename = tbExeName.Text;
                MainWindow.applicationData.editorList.Add(item);
                Close();
            }
            else
            {
                // okay, edit the current item instead of adding
                int i = 0;
                foreach (ApplicationData.EditorItem item in MainWindow.applicationData.editorList)
                {
                    if (i == MainWindow.applicationData.tmpListIndex)
                    {
                        item.menuName = tbMenuName.Text;
                        item.exeFilename = tbExeName.Text;
                    }
                    i++;
                }
                Close();
            }
        }

        private void ConfigureEditorItem_Load(object sender, EventArgs e)
        {
            if (MainWindow.applicationData.tmpInAdd == false)
            {
                tbExeName.Text = MainWindow.applicationData.tmpModifyExeName;
                tbMenuName.Text = MainWindow.applicationData.tmpModifyMenuName;
            }
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {

        }
    }
}