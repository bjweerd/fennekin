#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "application.h"
#include "main_window.h"
#include <gtk/gtk.h>


#include "document.h"
#include <iostream>
#include "fennekin_treemodel.h"



namespace fennekin
{
  // gdb: break fennekin::test
  void test()
  {
    std::cout << "\n\n*** STARTING test()\n\n";

    TreeModelNode* root = Application::app->doc->root;
    Application::app->doc->set_filename("/dev/null");
    Application::app->doc->load();
    

    // testing member functions
    FennekinTreemodel* treemodel = fennekin_treemodel_new();


    // delete our treemodel
    g_object_unref(treemodel);
    std::cout << "\n\n*** DONE WITH test()\n\n";
  }

  // program entry point
  int main (int argc, char **argv)
  {
    gtk_init(&argc, &argv);

    char* filename = NULL;
    if (argc >= 2)
      filename = argv[1];

    Application::app = new Application(argv[0], DATADIR);
    MainWindow::main_window = new MainWindow(Application::app, filename);

    // testing
    test();

    MainWindow::main_window->navigate_home();
    gtk_widget_show_all(MainWindow::main_window->window);
    gtk_main ();

    // bow out
    return 0;
  }
} // end namespace fennekin

int
main(int argc,char* argv[]) 
{
  // enter 'fennekin' namespace
  return fennekin::main(argc,argv);
}
