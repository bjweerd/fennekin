using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace WebDiver
{
    public partial class ConfigureEditorsForm : Form
    {
        public ConfigureEditorsForm()
        {
            InitializeComponent();
        }

        private void ConfigureEditorsForm_Load(object sender, EventArgs e)
        {
            DoReloadList();
        }

        private void DoReloadList()
        {
            listBox1.Items.Clear();

            foreach (ApplicationData.EditorItem item in MainWindow.applicationData.editorList)
            {
                listBox1.Items.Add(item.menuName);
            }
            listBox1.Refresh();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            ConfigureEditorItem frm = new ConfigureEditorItem();
            MainWindow.applicationData.tmpInAdd = true;
            frm.ShowDialog();
            DoReloadList();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (listBox1.SelectedItem == null)
            {
                MessageBox.Show("Please select an item from the list");
                return;
            }
            ConfigureEditorItem frm = new ConfigureEditorItem();
            MainWindow.applicationData.tmpInAdd = false;
            int i = 0;
            foreach (ApplicationData.EditorItem item in MainWindow.applicationData.editorList)
            {
                if (i == listBox1.SelectedIndex)
                {
                    MainWindow.applicationData.tmpListIndex = i;
                    MainWindow.applicationData.tmpModifyExeName = item.exeFilename;
                    MainWindow.applicationData.tmpModifyMenuName = item.menuName;
                }
                i++;
            }
            frm.ShowDialog();
            int tmp = listBox1.SelectedIndex;
            DoReloadList();
            listBox1.SelectedIndex = tmp;
        }

        private void button6_Click(object sender, EventArgs e)
        {
            Close();
        }

        // delete
        private void button3_Click(object sender, EventArgs e)
        {
            if (listBox1.SelectedItem == null)
            {
                MessageBox.Show("Please select an item from the list");
                return;
            }

            // actually delete the item
            MainWindow.applicationData.editorList.RemoveAt(listBox1.SelectedIndex);
            DoReloadList();
        }

        // move up
        private void button4_Click(object sender, EventArgs e)
        {
            if (listBox1.SelectedItem == null)
            {
                MessageBox.Show("Please select an item from the list");
                return;
            }

            // first item in list?
            if (listBox1.SelectedIndex == 0)
            {
                MessageBox.Show("Can't move first item further up.");
                return;
            }

            // todo - actually move item up

            // 1) get the item. 2) delete item from current pos. 3) insert item at new pos.
            int tmp = listBox1.SelectedIndex;
            ApplicationData.EditorItem item = MainWindow.applicationData.editorList[listBox1.SelectedIndex];
            MainWindow.applicationData.editorList.RemoveAt(listBox1.SelectedIndex);
            MainWindow.applicationData.editorList.Insert(listBox1.SelectedIndex - 1, item);
            
            DoReloadList();
            listBox1.SelectedIndex = tmp - 1;
        }

        // move down
        private void button5_Click(object sender, EventArgs e)
        {
            if (listBox1.SelectedItem == null)
            {
                MessageBox.Show("Please select an item from the list");
                return;
            }

            // last item in list?
            if (listBox1.SelectedIndex == listBox1.Items.Count-1)
            {
                MessageBox.Show("Can't move last item further down.");
                return;
            }

            // todo - actuall move item down
            // 1) get the item. 2) delete item from current pos. 3) insert item at new pos.
            int tmp = listBox1.SelectedIndex;
            ApplicationData.EditorItem item = MainWindow.applicationData.editorList[listBox1.SelectedIndex];
            MainWindow.applicationData.editorList.RemoveAt(listBox1.SelectedIndex);
            MainWindow.applicationData.editorList.Insert(listBox1.SelectedIndex + 1, item);

            DoReloadList();
            listBox1.SelectedIndex = tmp + 1;
        }

        private void ConfigureEditorsForm_FormClosed(object sender, FormClosedEventArgs e)
        {
        }
    }
}