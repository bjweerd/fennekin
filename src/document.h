#ifndef INCLUDED_DOCUMENT_H
#define INCLUDED_DOCUMENT_H

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <cstdlib>
#include <cstring>

namespace fennekin
{
  class Document
  {
    bool changed;
    char* filename;

  public:

    Document() : changed(false), filename(NULL) {}
    Document(const char* filename) : changed(false), filename(strdup(filename)) {}
    ~Document() { clear_data(); if (filename != NULL) { free(filename); filename = NULL; } }
    
    // Member access
    void set_filename(const char* filename);
    const char* get_filename() { return filename; }

    void change() { changed = true; }
    bool is_changed() { return changed; }

    // load and save the xml data (return false on error)
    void clear_data();
    bool load();
    bool save();

  private:
    // xml document data
  };
}

#endif /* INCLUDED_DOCUMENT_H */
