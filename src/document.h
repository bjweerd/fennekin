#ifndef INCLUDED_DOCUMENT_H
#define INCLUDED_DOCUMENT_H

#include <cstdlib>
#include <cstring>

namespace fennekin
{
  class Document
  {
    bool changed;
    char* filename;

  public:

    Document() : changed(false), filename(nullptr) {}
    Document(const char* filename) : changed(false), filename(strdup(filename)) {}
    ~Document() { clear_data(); if (filename != nullptr) { free(filename); filename = nullptr; } }
    
    // Member access
    void set_filename(const char* filename) 
    {
      if (Document::filename != nullptr) free(Document::filename);
      Document::filename = strdup(filename);

      clear_data();
    }
    const char* get_filename() { return filename; }

    bool is_changed() { return changed; }
    void change() { changed = true; }

    // load and save the xml data (return false on error)
    void clear_data() { changed = false; }
    bool load() { changed = false; return true; }
    bool save() { changed = false; return true; }
  };
}

#endif /* INCLUDED_DOCUMENT_H */
