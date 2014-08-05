#ifndef INCLUDED_DOCUMENT_H
#define INCLUDED_DOCUMENT_H

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <cstdlib>
#include <cstring>
#include <string>

namespace fennekin
{

  struct TreeModelNode
  {
    struct TreeModelNode* parent;
    struct TreeModelnode* next;
    struct TreeModelNode* children;
    
    typedef enum { 
      search_term_type, 
      search_engine_type, 
      engine_set_type 
    } NodeType;
    NodeType node_type;

    /* each node holds 5 std::string objects, i think that's a bit overkill/too heavy especially with
     big trees, but let's worry about that later */
    std::string search_term_value;
    std::string search_term_engine_set;
    std::string engine_name;
    std::string engine_value;
    std::string engine_set_name;
  };

  class Document
  {
    bool changed;
    char* filename;

  public:

    Document() : changed(false), filename(NULL) {
      root = NULL;
      xml_root_version_major = xml_root_version_minor = 0;
    }
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

  public:
    TreeModelNode* root;
    std::string xml_root_element_string; /* the xml root element is either either 'fennekin' or 'webdiver',
					  and we remember this so we can write a webdiver file back into webdiver format. */
    int xml_root_version_major, xml_root_version_minor; /* data format version */
  };
} 

#endif /* INCLUDED_DOCUMENT_H */
