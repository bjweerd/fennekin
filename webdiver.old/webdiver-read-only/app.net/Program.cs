using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace WebDiver
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            MainWindow mainWindow = new MainWindow();
            
            Application.Run(mainWindow);
        }
    }
}