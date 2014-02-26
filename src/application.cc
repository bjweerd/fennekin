#include "application.h"

namespace fennekin
{
  Application* Application::app = NULL; // instance pointer

  // constructor/destructor
  Application::Application(const char* progname, const char* datadir_)
    {
      doc = new Document();
      datadir = new app_util::datadir_t(progname,datadir_);

      const gchar* builder_filename = datadir->get("/Fennekin.ui");
      builder = gtk_builder_new();
      gtk_builder_add_from_file(builder, builder_filename, NULL);      
    }
  Application::~Application()
    {
      delete doc;
    }

}

