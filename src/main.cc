#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "application.h"
#include "main_window.h"

#include "fennekin_treemodel.h"

#include <gtk/gtk.h>


namespace fennekin
{
  // gdb: break fennekin::test
  void test()
  {
    FennekinTreemodel* treemodel = fennekin_treemodel_new();

    // testing member functions

    g_object_unref(treemodel);
  }

  // program entry point
  int main (int argc, char **argv)
  {
    gtk_init(&argc, &argv);

    char* filename = NULL;
    if (argc >= 2)
      filename = argv[1];

    // testing
    test();

    Application::app = new Application(argv[0], DATADIR);
    MainWindow::main_window = new MainWindow(Application::app, filename);

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
