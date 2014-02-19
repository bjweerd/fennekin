using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace WebDiver
{
    public partial class NewFileWizard : Form
    {
        public NewFileWizard()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            // i think i swapped the meaning of the two variables
            string engineSetName = this.comboBox1.Text;
            string termName = this.textBox1.Text;

            if (termName == "")
            {
                MessageBox.Show("Please give a concept or term to search for.");
                return;
            }

            if (engineSetName == "")
            {
                MessageBox.Show("Please select one of the named set of search engines.");
                return;
            }

            MainWindow.mainWindow.NewFile_Generate(engineSetName,termName);

            // Done, thank you.
            Close();
        }

        private void NewFileWizard_Load(object sender, EventArgs e)
        {
            this.comboBox1.Text = "default";
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void NewFileWizard_Load_1(object sender, EventArgs e)
        {

        }
    }
}