#ifndef INCLUDED_APPLICATION_H
#define INCLUDED_APPLICATION_H

#include "document.h"
#include "datadir.h"

#include <gtk/gtk.h>

namespace fennekin
{
  struct Application
  {
    static Application* app;    // instance pointer

    Document* doc;		// the document and it's data
    GtkBuilder* builder;
    app_util::datadir_t* datadir;

    Application(const char* progname, const char* datadir_);
    ~Application();
  };
}

#endif // INCLUDED_APPLICATION_H
