#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "document.h"

#include <cstdlib>
#include <sstream>

#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include <gdk/gdkkeysyms.h>

namespace fennekin
{

  struct Application
  {
    static Application* app;    // instance pointer

    Document* doc;		// the document and it's data
    GtkBuilder* builder;

    Application()
    {
      doc = new Document();

      const gchar* builder_filename = datadir("/Fennekin.ui");
      builder = gtk_builder_new();
      gtk_builder_add_from_file(builder, builder_filename, nullptr);      
    }
    ~Application()
    {
      delete doc;
    }

    const char* datadir(const char* filename)
    {
      static char* buffer = nullptr;

      if (buffer != nullptr) { 
	free(buffer); buffer = nullptr; 
      }

      std::ostringstream tmp;
      tmp << DATADIR << filename;
      buffer = strdup(tmp.str().c_str());

      return buffer;
    }

  };
  Application* Application::app = nullptr; // instance pointer

  // Main Window class

  struct MainWindow
  {
    static MainWindow* main_window; // instance pointer

    Application* app;

    GtkWidget* window;
    GtkWidget* scrolled_window;
    WebKitWebView* web_view;
    // toolbar
    GtkWidget* entry_url;
    GtkWidget* toolbutton_go;
    GtkWidget* toolbutton_back;
    GtkWidget* toolbutton_forward;
    GtkWidget* toolbutton_reload;
    GtkWidget* toolbutton_stop_loading;
    GtkWidget* toolbutton_file_new;
    GtkWidget* toolbutton_file_open;
    GtkWidget* toolbutton_file_save;
    // status bar
    GtkWidget* statusbar;

    // Menu items
    GtkAction* menuitem_quit;
    GtkAction* menuitem_about;
    GtkAction* menuitem_help_issues;
    GtkAction* menuitem_help_wiki;
    GtkAction* menuitem_file_new;
    GtkAction* menuitem_file_open;
    GtkAction* menuitem_file_save;
    GtkAction* menuitem_file_save_as;
    GtkAction* menuitem_file_reload;

    MainWindow(Application* app) : app(app)
    {
      window = GTK_WIDGET(gtk_builder_get_object(app->builder,"MainWindow"));

      scrolled_window = GTK_WIDGET(gtk_builder_get_object(app->builder, "scrolled_window"));
      // toolbar
      entry_url = GTK_WIDGET(gtk_builder_get_object(app->builder, "entry_url"));
      toolbutton_go = GTK_WIDGET(gtk_builder_get_object(app->builder, "toolbutton_go"));
      toolbutton_back = GTK_WIDGET(gtk_builder_get_object(app->builder,"toolbutton_back"));
      toolbutton_forward = GTK_WIDGET(gtk_builder_get_object(app->builder,"toolbutton_forward"));
      toolbutton_reload = GTK_WIDGET(gtk_builder_get_object(app->builder,"toolbutton_reload"));
      toolbutton_stop_loading = GTK_WIDGET(gtk_builder_get_object(app->builder,"toolbutton_stop_loading"));
      toolbutton_file_new = GTK_WIDGET(gtk_builder_get_object(app->builder,"toolbutton_file_new"));
      toolbutton_file_open = GTK_WIDGET(gtk_builder_get_object(app->builder,"toolbutton_file_open"));
      toolbutton_file_save = GTK_WIDGET(gtk_builder_get_object(app->builder,"toolbutton_file_save"));
      // menu items
      menuitem_quit = GTK_ACTION(gtk_builder_get_object(app->builder, "menuitem_quit"));
      menuitem_about = GTK_ACTION(gtk_builder_get_object(app->builder, "menuitem_about"));
      menuitem_help_issues = GTK_ACTION(gtk_builder_get_object(app->builder, "menuitem_help_issues"));
      menuitem_help_wiki = GTK_ACTION(gtk_builder_get_object(app->builder, "menuitem_help_wiki"));
      menuitem_file_new = GTK_ACTION(gtk_builder_get_object(app->builder, "menuitem_file_new"));
      menuitem_file_open = GTK_ACTION(gtk_builder_get_object(app->builder, "menuitem_file_open"));
      menuitem_file_save = GTK_ACTION(gtk_builder_get_object(app->builder, "menuitem_file_save"));
      menuitem_file_save_as = GTK_ACTION(gtk_builder_get_object(app->builder, "menuitem_file_save_as"));
      menuitem_file_reload = GTK_ACTION(gtk_builder_get_object(app->builder, "menuitem_file_reload"));
      // status bar
      statusbar = GTK_WIDGET(gtk_builder_get_object(app->builder, "status_bar"));
      // web view
      web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
      gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(web_view));

      // events

      // MainWindow events
      g_signal_connect(window, "delete_event", G_CALLBACK(on_quit_event), nullptr);
      // toolbar events
      g_signal_connect(entry_url, "key-press-event", G_CALLBACK(on_entry_url_key_press_event), nullptr);
      g_signal_connect(toolbutton_go, "clicked", G_CALLBACK(on_do_navigate_event), nullptr);
      g_signal_connect(toolbutton_back, "clicked", G_CALLBACK(on_navigate_back_event), nullptr);
      g_signal_connect(toolbutton_forward, "clicked", G_CALLBACK(on_navigate_forward_event), nullptr);
      g_signal_connect(toolbutton_reload, "clicked", G_CALLBACK(on_navigate_reload_event), nullptr);
      g_signal_connect(toolbutton_stop_loading, "clicked", G_CALLBACK(on_navigate_stop_loading_event), nullptr);
      g_signal_connect(toolbutton_file_new, "clicked", G_CALLBACK(on_file_new_event), nullptr);
      g_signal_connect(toolbutton_file_open, "clicked", G_CALLBACK(on_file_open_event), nullptr);
      g_signal_connect(toolbutton_file_save, "clicked", G_CALLBACK(on_file_save_event), nullptr);
      // menu events
      g_signal_connect(menuitem_quit, "activate", G_CALLBACK(on_quit_event), nullptr);
      g_signal_connect(menuitem_about, "activate", G_CALLBACK(on_about_event), nullptr);
      g_signal_connect(menuitem_help_issues, "activate", G_CALLBACK(on_help_issues_event), nullptr);
      g_signal_connect(menuitem_help_wiki, "activate", G_CALLBACK(on_help_wiki_event), nullptr);
      g_signal_connect(menuitem_file_new, "activate", G_CALLBACK(on_file_new_event), nullptr);
      g_signal_connect(menuitem_file_open, "activate", G_CALLBACK(on_file_open_event), nullptr);
      g_signal_connect(menuitem_file_save, "activate", G_CALLBACK(on_file_save_event), nullptr);
      g_signal_connect(menuitem_file_save_as, "activate", G_CALLBACK(on_file_save_as_event), nullptr);
      g_signal_connect(menuitem_file_reload, "activate", G_CALLBACK(on_file_reload_event), nullptr);
      // web_view events
      g_signal_connect(web_view, "notify::load-status", G_CALLBACK(on_web_view_notify_load_status_event), nullptr);

      // initualize statusbar
      set_statusbar_info_message("Ready", false);
      _title_reset();
    }

    // member functions
    const char* question_save_changes() {
      return "Data has changed. Save changes?";
    }

    void navigate(const gchar* url)
    {
      if (url != nullptr)
	{
	  webkit_web_view_open(web_view, url);
	  gtk_entry_set_text(GTK_ENTRY(entry_url), url);
	}
    }

    void set_statusbar_info_message(const gchar* msg, bool do_pop = true)
    {
      guint id = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "info");

      if (do_pop) 
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar), id);

      gtk_statusbar_push(GTK_STATUSBAR(statusbar), id, msg);
    }
    
    //
    // show_info(), show_error(), show_warning() gtk dialog functions
    //
    // See: http://zetcode.com/tutorials/gtktutorial/gtkdialogs/
    //
    
    void show_message(const gchar* message, const gchar* title = "Information")
    {
      GtkWidget *dialog = gtk_message_dialog_new(
						 GTK_WINDOW(window),
						 GTK_DIALOG_DESTROY_WITH_PARENT,
						 GTK_MESSAGE_INFO,
						 GTK_BUTTONS_OK,
						 message,
						 "title");

      gtk_window_set_title(GTK_WINDOW(dialog), title);
      gtk_dialog_run(GTK_DIALOG(dialog));
      gtk_widget_destroy(dialog);
    }

    void show_error(const gchar* error_message)
    {
      GtkWidget *dialog = gtk_message_dialog_new(
						 GTK_WINDOW(window),
						 GTK_DIALOG_DESTROY_WITH_PARENT,
						 GTK_MESSAGE_ERROR,
						 GTK_BUTTONS_OK,
						 "%s",
						 error_message);
      
      gtk_window_set_title(GTK_WINDOW(dialog), "Error");
      gtk_dialog_run(GTK_DIALOG(dialog));
      gtk_widget_destroy(dialog);
    }

    void show_warning(const gchar* warning_message)
    {
      GtkWidget *dialog = gtk_message_dialog_new(
						 GTK_WINDOW(window),
						 GTK_DIALOG_DESTROY_WITH_PARENT,
						 GTK_MESSAGE_WARNING,
						 GTK_BUTTONS_OK,
						 "%s",
						 warning_message);

      gtk_window_set_title(GTK_WINDOW(dialog), "Warning");
      gtk_dialog_run(GTK_DIALOG(dialog));
      gtk_widget_destroy(dialog);
    }

    // returns: GTK_RESPONSE_NO
    // returns: GTK_RESPONSE_YES
    gint ask_yes_no(const gchar* question)
    {
      GtkWidget *dialog = gtk_message_dialog_new(
						 GTK_WINDOW(window),
						 GTK_DIALOG_DESTROY_WITH_PARENT,
						 GTK_MESSAGE_QUESTION,
						 GTK_BUTTONS_YES_NO,
						 "%s",
						 question);
      gtk_window_set_title(GTK_WINDOW(dialog), "Question");
      gint response = gtk_dialog_run(GTK_DIALOG(dialog));
      gtk_widget_destroy(dialog);
      return response;
    }

    // returns: GTK_RESPONSE_NO
    // returns: GTK_RESPONSE_YES
    // returns: GTK_RESPONSE_CANCEL
    gint ask_yes_no_cancel(const gchar* question)
    {
      GtkWidget *dialog = gtk_message_dialog_new(
						 GTK_WINDOW(window),
						 GTK_DIALOG_DESTROY_WITH_PARENT,
						 GTK_MESSAGE_QUESTION,
						 GTK_BUTTONS_NONE,
						 "%s",
						 question);
      gtk_window_set_title(GTK_WINDOW(dialog), "Question");
      gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_YES, GTK_RESPONSE_YES);
      gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_NO, GTK_RESPONSE_NO);
      gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
      gint response = gtk_dialog_run(GTK_DIALOG(dialog));
      gtk_widget_destroy(dialog);
      return response;
    }

    // returns: GTK_RESPONSE_OK
    // returns: GTK_RESPONSE_CANCEL
    gint ask_ok_cancel(const gchar* question)
    {
      GtkWidget *dialog = gtk_message_dialog_new(
						 GTK_WINDOW(window),
						 GTK_DIALOG_DESTROY_WITH_PARENT,
						 GTK_MESSAGE_QUESTION,
						 GTK_BUTTONS_OK_CANCEL,
						 "%s",
						 question);
      gtk_window_set_title(GTK_WINDOW(dialog), "Question");
      gint response = gtk_dialog_run(GTK_DIALOG(dialog));
      gtk_widget_destroy(dialog);
      return response;
    }



    //
    // File opration related members
    //


    void do_open()
    {
      if (_do_ask_for_save() == false) return;

      // now do the 'file open' dialog
      {
	GtkWidget *dialog = gtk_file_chooser_dialog_new ("Open File",
							 GTK_WINDOW(window),
							 GTK_FILE_CHOOSER_ACTION_OPEN,
							 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
							 GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
							 nullptr);
	
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	  {
	    char *filename;
	    
	    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
	    app->doc->set_filename(filename);
	    if (app->doc->load() == false)
	      {
		show_error("error loading data from file");
	      }
	    else
	      _status_loaded();

	    g_free (filename);
	  }
	gtk_widget_destroy (dialog);
      }	// done with 'file open' dialog

    }

    void do_save()
    {
      if (app->doc->get_filename() == nullptr)
	do_save_as();
      else
	{
	  if (app->doc->save() == false)
	    {
	      show_error("error saving data to file");
	    }
	  else
	    _status_saved();
	}
    }

    const char* get_home_folder() // for use in do_save_as()
    {
      const char* home = getenv("HOME");
      // under windows we need to query other environment variables, but we'll see for now
      return home;
    }

    void do_save_as()
    {
      
      // now do the 'file save as' dialog
      {
	GtkWidget *dialog = gtk_file_chooser_dialog_new ("Save File",
							 GTK_WINDOW(window),
							 GTK_FILE_CHOOSER_ACTION_SAVE,
							 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
							 GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
							 nullptr);
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);

	if (app->doc->get_filename() == nullptr)
	  {
	    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), get_home_folder());
	    gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), "Untitled document.fennekin");
	  }
	else
	  gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), app->doc->get_filename());
	
	
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	  {
	    char *filename;
	    
	    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

	    app->doc->set_filename(filename);

	    if (app->doc->save() == false)
	      {
		show_error("error saving data to file");	 
	      }
	    else
	      _status_saved();

	    g_free (filename);
	  }
	
	gtk_widget_destroy (dialog);
      }	// done with 'file save as' dialog

    }

    void do_reload()
    {
      if (app->doc->get_filename() == nullptr) 
	do_open();
      else 
	{
	  if (_do_ask_for_save() == false) return;
	  
	  app->doc->clear_data();
	  if (app->doc->load() == false)
	    {
	      show_error("error loading data from file");
	    }
	  else
	    _status_loaded();
	}
    }

    void do_new()
    {
      if (_do_ask_for_save() == false) return;

      delete app->doc;
      app->doc = new Document;

      // now run the 'new file wizard'
      {	
	show_message("-- NEW FILE WIZARD MOCKUP --");
      }

      app->doc->change(), _title_changed();
    }

    gboolean do_quit()		// retval: FALSE = quit program, TRUE = continue program
    {
      if (app->doc->is_changed() == false) {
	gtk_main_quit();
	return FALSE;
      }

      switch (ask_yes_no_cancel(question_save_changes()))
	{
	case GTK_RESPONSE_YES: 
	  do_save();
	case GTK_RESPONSE_NO:
	  gtk_main_quit();
	  return FALSE;
	case GTK_RESPONSE_CANCEL:
	default:
	  return TRUE;
	}
    }

    bool _do_ask_for_save() 	// retval: false = user pressed cancel
    {
      if (app->doc->is_changed() == true) {
	switch (ask_yes_no_cancel(question_save_changes())) 
	  {
	  case GTK_RESPONSE_YES:
	    do_save();
	    break;
	  case GTK_RESPONSE_NO:
	    break;
	  case GTK_RESPONSE_CANCEL:
	  default:
	    return false;
	  }
      }
      return true;
    }

    void _status_loaded()
    {
      std::ostringstream tmp;
      tmp << "Loaded file '" << app->doc->get_filename() << "'";
      set_statusbar_info_message(tmp.str().c_str());
      _title_reset();
    }
    void _status_saved()
    {
      std::ostringstream tmp;
      tmp << "Saved file '" << app->doc->get_filename() << "'";
      set_statusbar_info_message(tmp.str().c_str());
      _title_reset();
    }
    void _title_changed()
    {
      const char* filename = "<Untitled document>";

      if (app->doc->get_filename() != nullptr) 
	filename = app->doc->get_filename();

      std::ostringstream tmp;

      tmp << "Fennekin - * " << filename << "";

      gtk_window_set_title(GTK_WINDOW(window), tmp.str().c_str());
    }
    void _title_reset()
    {
      const char* filename = "<Untitled document>";

      if (app->doc->get_filename() != nullptr) 
	filename = app->doc->get_filename();

      std::ostringstream tmp;

      tmp << "Fennekin - " << filename << "";

      gtk_window_set_title(GTK_WINDOW(window), tmp.str().c_str());
    }


    //
    // callbacks
    //


    static gboolean on_quit_event(GtkWidget* widget, gpointer data)  {
      return main_window->do_quit();
    }  
    static void on_file_new_event(GtkWidget* widget, gpointer data) {
      main_window->do_new();
    }  
    static void on_file_open_event(GtkWidget* widget, gpointer data) {
      main_window->do_open();
    }  
    static void on_file_save_event(GtkWidget* widget, gpointer data) {
      main_window->do_save();
    }  
    static void on_file_save_as_event(GtkWidget* widget, gpointer data) {
      main_window->do_save_as();
    }  
    static void on_file_reload_event(GtkWidget* widget, gpointer data) {
      main_window->do_reload();
    }  


    static void on_about_event(GtkWidget* widget, gpointer data) 
    {
      GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(main_window->app->datadir("/Fennekin.png"), nullptr);
      GtkWidget *dialog = gtk_about_dialog_new();

      gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "Fennekin");
      gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), PACKAGE_VERSION); 
      gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), 
				     "(c) Bert van der Weerd <" PACKAGE_BUGREPORT ">");
      gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), 
				    "Fennekin is a simple program that allows"
				    " searching with a hierarchical tree of terms. The search"
				    " engine to use can be selected from a drop-down list."
				    );
      gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), PACKAGE_URL);
      gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);

      g_object_unref(pixbuf), pixbuf = nullptr;
      gtk_dialog_run(GTK_DIALOG (dialog));
      gtk_widget_destroy(dialog);
    }  

    void browse_url(const gchar* url)
    {
      GError* error = nullptr;
      gtk_show_uri(gtk_widget_get_screen(GTK_WIDGET(window)), url, GDK_CURRENT_TIME, &error);
    }

    static void on_help_issues_event(GtkWidget* widget, gpointer data) {
      main_window->browse_url(PACKAGE_URL "issues");
    }
    static void on_help_wiki_event(GtkWidget* widget, gpointer data) {
      main_window->browse_url(PACKAGE_URL "wiki");
    }

    static void on_web_view_notify_load_status_event(WebKitWebView *web_view, GParamSpec *pspec, void* context) 
    {
      WebKitLoadStatus status = webkit_web_view_get_load_status (web_view);
      GObject *object = G_OBJECT (web_view);
        
      g_object_freeze_notify (object);

      const gchar* url = webkit_web_view_get_uri(web_view);
      if (url != nullptr)
	gtk_entry_set_text(GTK_ENTRY(main_window->entry_url), url);
        
      switch (status)
        {
        case WEBKIT_LOAD_FINISHED:
	  main_window->set_statusbar_info_message("Webpage loading finished");
	  break;
                
        case WEBKIT_LOAD_PROVISIONAL:
	  main_window->set_statusbar_info_message("Webpage load provisional");
	  break;
          
        case WEBKIT_LOAD_COMMITTED:
	  main_window->set_statusbar_info_message("Webpage load commited");
	  break;
          
        case WEBKIT_LOAD_FIRST_VISUALLY_NON_EMPTY_LAYOUT:
	  // main_window->set_statusbar_info_message("Webpage first layout");
	  break;
          
        case WEBKIT_LOAD_FAILED:
	  main_window->set_statusbar_info_message("Webpage loading failed");
	  break;
          
        default:
	  break;
        }
      
      g_object_thaw_notify (object);
    }

    static gboolean on_entry_url_key_press_event(GtkWidget* widget, GdkEventKey* event, gpointer data) {
      if (event->keyval == GDK_KEY_Return)
	{
	  on_do_navigate_event(widget,data);
	}
      return FALSE;
    }

    static void on_do_navigate_event(GtkWidget* widget, gpointer data) {
      main_window->navigate(gtk_entry_get_text(GTK_ENTRY(main_window->entry_url)));
    }

    static void on_navigate_back_event(GtkWidget* widget, gpointer data) {
      webkit_web_view_go_back(main_window->web_view);
    }

    static void on_navigate_forward_event(GtkWidget* widget, gpointer data) {
      webkit_web_view_go_forward(main_window->web_view);
    }

    static void on_navigate_reload_event(GtkWidget* widget, gpointer data) {
      webkit_web_view_reload(main_window->web_view);
    }

    static void on_navigate_stop_loading_event(GtkWidget* widget, gpointer data) {

      main_window->show_message("The data is now considered CHANGED");
      main_window->app->doc->change(), main_window->_title_changed();

      //webkit_web_view_stop_loading(main_window->web_view);
    }

  };
  MainWindow* MainWindow::main_window = nullptr; // instance pointer


  // program entry point
  int main (int argc, char **argv)
  {
    gtk_init(&argc, &argv);

    Application::app = new Application();
    MainWindow::main_window = new MainWindow(Application::app);
    MainWindow::main_window->navigate("https://encrypted.google.com/");

    gtk_widget_show_all(MainWindow::main_window->window);
    gtk_main ();

    // bow out
    return 0;
  }

} // end namespace fennekin

int
main(int argc,char* argv[]) 
{
  return fennekin::main(argc,argv);
}
