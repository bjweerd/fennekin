#ifndef INCLUDED_MAIN_WINDOW_H
#define INCLUDED_MAIN_WINDOW_H

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "application.h"

#include <gtk/gtk.h>
#include <webkit/webkit.h>

namespace fennekin
{
  // Main Window class

  struct MainWindow
  {
    static MainWindow* main_window; // instance pointer

    Application* app;

    GtkWidget* window;
    GtkWidget* scrolled_window;
    GtkWidget* vbox_left;
    WebKitWebView* web_view;
    // toolbar
    GtkWidget* entry_url;
    GtkWidget* toolbutton_go;
    GtkWidget* toolbutton_back;
    GtkWidget* toolbutton_forward;
    GtkWidget* toolbutton_reload;
    GtkWidget* toolbutton_stop_loading;
    GtkWidget* toolbutton_home;
    GtkWidget* toolbutton_file_new;
    GtkWidget* toolbutton_file_open;
    GtkWidget* toolbutton_file_save;
    // status bar
    GtkWidget* statusbar;
    // menu items
    GtkAction* menuitem_file_new;
    GtkAction* menuitem_file_open;
    GtkAction* menuitem_file_save;
    GtkAction* menuitem_file_save_as;
    GtkAction* menuitem_file_reload;
    GtkAction* menuitem_quit;
    GtkToggleAction* menuitem_view_show_sidebar;
    GtkAction* menuitem_help_issues;
    GtkAction* menuitem_help_wiki;
    GtkAction* menuitem_help_releases; 
    GtkToggleAction* menuitem_help_external;
    GtkAction* menuitem_example_universal;
    GtkAction* menuitem_example_languages;
    GtkAction* menuitem_about;

    MainWindow(Application* app, const char* filename);

    // member functions
    const char* question_save_changes() const {
      return "Data has changed. Save changes?";
    }

    void navigate(const gchar* url);
    void navigate_home();

    void set_statusbar_info_message(const gchar* msg, bool do_pop = true);
    
    //
    // show_info(), show_error(), show_warning() gtk dialog functions
    //
    // See: http://zetcode.com/tutorials/gtktutorial/gtkdialogs/
    //
    
    void show_message(const gchar* message, const gchar* title = "Information");
    void show_error(const gchar* error_message);
    void show_warning(const gchar* warning_message);

    // returns: GTK_RESPONSE_NO
    // returns: GTK_RESPONSE_YES
    gint ask_yes_no(const gchar* question);

    // returns: GTK_RESPONSE_NO
    // returns: GTK_RESPONSE_YES
    // returns: GTK_RESPONSE_CANCEL
    gint ask_yes_no_cancel(const gchar* question);

    // returns: GTK_RESPONSE_OK
    // returns: GTK_RESPONSE_CANCEL
    gint ask_ok_cancel(const gchar* question);

    //
    // File opration related members
    //

    void do_open(const char* filename_param = NULL);
    bool do_save();
    const char* get_home_folder(); // for use in do_save_as() 
    bool do_save_as();
    void do_reload();
    void do_new();
    gboolean do_quit();		// retval: FALSE = quit program, TRUE = continue program

    // interface utility funcions
    bool _do_ask_for_save(); 	// retval: false = user pressed cancel
    void _status_loaded();
    void _status_saved();
    void _title_changed();
    void _title_reset();

    void browse_external_url(const gchar* url);

    //
    // callbacks
    //

    static gboolean on_quit_event(GtkWidget* widget, gpointer data);
    static void on_file_new_event(GtkWidget* widget, gpointer data);
    static void on_file_open_event(GtkWidget* widget, gpointer data);
    static void on_file_save_event(GtkWidget* widget, gpointer data);
    static void on_file_save_as_event(GtkWidget* widget, gpointer data);
    static void on_file_reload_event(GtkWidget* widget, gpointer data);
    static void on_view_show_sidebar_event(GtkWidget* widget, gpointer data);
    static void on_ctrl_l_event(GtkWidget* widget, gpointer data);
    static void on_about_event(GtkWidget* widget, gpointer data);
    static void on_help_issues_event(GtkWidget* widget, gpointer data);
    static void on_help_wiki_event(GtkWidget* widget, gpointer data);
    static void on_help_releases_event(GtkWidget* widget, gpointer data);
    static void on_help_external_event(GtkWidget* widget, gpointer data);
    static void on_example_universal_event(GtkWidget* widget, gpointer data);
    static void on_example_languages_event(GtkWidget* widget, gpointer data);
    static void on_web_view_notify_load_status_event(WebKitWebView *web_view, GParamSpec *pspec, void* context);
    static gboolean on_entry_url_key_press_event(GtkWidget* widget, GdkEventKey* event, gpointer data);
    static void on_do_navigate_event(GtkWidget* widget, gpointer data);
    static void on_navigate_back_event(GtkWidget* widget, gpointer data);
    static void on_navigate_forward_event(GtkWidget* widget, gpointer data);
    static void on_navigate_reload_event(GtkWidget* widget, gpointer data);
    static void on_navigate_stop_loading_event(GtkWidget* widget, gpointer data);
    static void on_navigate_home_event(GtkWidget* widget, gpointer data);
  };
}

#endif // INCLUDED_MAIN_WINDOW_H
