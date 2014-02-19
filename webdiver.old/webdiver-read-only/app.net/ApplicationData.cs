using System;
using System.Collections.Generic;
using System.Text;

// registry usage help : http://www.devhood.com/tutorials/tutorial_details.aspx?tutorial_id=264
using Microsoft.Win32;

namespace WebDiver
{
    public class ApplicationData
    {
        //
        // Configurable Persistent list of editors
        //

        public class EditorItem
        {
            public string menuName;
            public string exeFilename;
            public bool isDefaultForXSLTEdit;

            public EditorItem()
            {
                menuName = "";
                exeFilename = "";
                isDefaultForXSLTEdit = false;
            }
        }

        public List<EditorItem> editorList;

        // for XML Editor stuff...
        public bool tmpInAdd;
        public string tmpModifyExeName, tmpModifyMenuName;
        public int tmpListIndex;

        //
        // Recent File history
        //

        public List<string> recentFiles;

        // Constructor
        public ApplicationData()
        {
            editorList = new List<EditorItem>();
            recentFiles = new List<string>();
        }

        public void PersistentSave()
        {
            RegistryKey key = Registry.CurrentUser.OpenSubKey("Software\\bsod\\WebDiver", true);
            
            //////////////////////
            // Editors:
            ///
            
            // write count
            key.SetValue("EditorListCount", editorList.Count);

            // write list
            int i;
            for (i = 0; i < editorList.Count; i++)
            {
                string s1 = String.Format("EditorList_MenuName_{0}", i);
                string s2 = String.Format("EditorList_ExecName_{0}", i);

                key.SetValue(s1, editorList[i].menuName);
                key.SetValue(s2, editorList[i].exeFilename);
            }

            //////////////////////
            // RecentFiles:
            ///

            // write count
            key.SetValue("RecentFileCount", recentFiles.Count);

            // write list
            for (i = 0; i < recentFiles.Count; i++)
            {
                string s1 = String.Format("RecentFile_{0}", i);

                key.SetValue(s1, recentFiles[i]);
            }
        }
        public void PersistentLoad()
        {
            RegistryKey key = Registry.CurrentUser.OpenSubKey("Software\\bsod\\WebDiver");
            
            if (key == null)
            {
                key = Registry.CurrentUser.CreateSubKey("Software\\bsod\\WebDiver");
            }

            /////////////////////////////////////////////////////////////////////

            // get EditorList
            if (key.GetValue("EditorListCount") != null)
            {
                editorList.Clear();
                int editorListCount = (int)key.GetValue("EditorListCount"), i;

                for (i = 0; i < editorListCount; i++)
                {
                    string s1 = String.Format("EditorList_MenuName_{0}", i);
                    string s2 = String.Format("EditorList_ExecName_{0}", i);

                    string t1 = key.GetValue(s1).ToString();
                    string t2 = key.GetValue(s2).ToString();

                    EditorItem item = new EditorItem();
                    item.menuName = t1;
                    item.exeFilename = t2;

                    editorList.Add(item);
                }
            }
            /////////////////////////////////////////////////////////////////////

            // get recent file list
            if (key.GetValue("RecentFileCount") != null)
            {
                recentFiles.Clear();
                int editorListCount = (int)key.GetValue("RecentFileCount"), i;

                for (i = 0; i < editorListCount; i++)
                {
                    string s1 = String.Format("RecentFile_{0}", i);

                    string t1 = key.GetValue(s1).ToString();

                    recentFiles.Add(t1);
                }
            }
        }
    }
}
