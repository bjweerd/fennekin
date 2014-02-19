using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Xml.Xsl; // for: XSL Processor

namespace WebDiver
{
    public partial class XSLTImportWindow : Form
    {
        public XSLTImportWindow()
        {
            InitializeComponent();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            XSLTOptionsWindow frmOptionsWindow = new XSLTOptionsWindow();

            // We do ShowDialog() here because we dont want them both running at the same time
            frmOptionsWindow.ShowDialog();
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            Hide();
        }

        private void button4_Click(object sender, EventArgs e)
        {
            MessageBox.Show("starting import.");

            // Run Transform!!!

            // 1) specify input file and transform file
            // 2) specify output file

            string filenameInputXML = MainWindow.fileName;
            string filenameInputXSLT = comboXSLT.Text;
            string filenameOutput = comboOutput.Text;

            if (filenameInputXSLT == "")
            {
                MessageBox.Show("Input XSLT Transformation filename cannot be empty");
                return;
            }
            if (filenameOutput == "")
            {
                MessageBox.Show("Output file name cannot be empty");
                return;
            }

            // 3) run the transform
            XslCompiledTransform transform = new XslCompiledTransform();
            try
            {

                transform.Load(filenameInputXSLT);

            } catch (Exception)
            {
                MessageBox.Show("XSLT input file has errors.");
                return;
            }

            string source = filenameInputXML;
            string destination = filenameOutput;
            
            try
            {
                transform.Transform(source, destination);
            }
            catch (Exception)
            {
                MessageBox.Show("XSL transform has failed.");
                return;
            }


            MessageBox.Show("Done, without errors.\nDisplaying tree in new document.\n");
            Close();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            // browse for file dialog code
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Title = "Open XSLT Transform file";
            dlg.Filter = "XSLT files (*.xsl)|*.xsl|All files (*.*)|*.*";
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                comboXSLT.Text =  dlg.FileName;
            }
            dlg.Dispose();

        }

        private void XSLTWindow_Load(object sender, EventArgs e)
        {
//            if (MainWindow.fileName == "")
//            {
//                MessageBox.Show("Sorry, but you must have a WebDiver document open to use this.");
//                Close();
//                return;
//            }
        }

        private void button6_Click(object sender, EventArgs e)
        {
            if (comboXSLT.Text == "")
            {
                MessageBox.Show("Please select XSLT file first.");
                return;
            }

            System.Diagnostics.Process.Start("notepad.exe", "\"" + comboXSLT.Text + "\"");
        }


        private void button5_Click(object sender, EventArgs e)
        {
            if (comboOutput.Text == "")
            {
                MessageBox.Show("Please select XSLT file first.");
                return;
            }

            try
            {
                System.Diagnostics.Process.Start("\"" + comboOutput.Text + "\"");
            }
            catch (Exception)
            {
                MessageBox.Show("Cannot view this file.");
                return;
            }

        }

        private void button7_Click(object sender, EventArgs e)
        {
            // Show 'save as' dialog box
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Title = "Select Input file";
            dlg.DefaultExt = "xml";
            dlg.Filter = "XML files (*.xml)|*.xml|"
                + "All files|*.*";
            dlg.InitialDirectory = "";
            dlg.RestoreDirectory = true;

            if (dlg.ShowDialog() == DialogResult.OK)
            {
                comboOutput.Text = dlg.FileName;
            }

            dlg.Dispose();
        }

        private void comboOutput_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void label1_Click_1(object sender, EventArgs e)
        {

        }

        private void comboXSLT_SelectedIndexChanged(object sender, EventArgs e)
        {

        }
    }
}