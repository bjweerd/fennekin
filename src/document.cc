/* document.cc - implement the document part of fennekin
 *
 * The basic strategy is as follows.:
 *
 * 1) data is stored in a GTK+ Custom Tree Model (so the data occurs only once in memory)
 * 2) data is read using the SAX interface of libxml2 (because it's the fastest way to read xml)
 *
TODO: 

1) create the CustomTreeModel and verify it works using some hardcoded testdata
2) read xml using sax into the working CustomTreeModel
3) enjoy and release :)

 * Resources:
 *
 * libxml2 sax example === https://gist.github.com/cooldaemon/106870
 * Using the SAX Interface of LibXML === http://www.jamesh.id.au/articles/libxml-sax/libxml-sax.html
 *
 */

#include "document.h"

#include <iostream>
#include <fstream>

namespace fennekin
{

  void Document::set_filename(const char* filename)  
  {
    if (Document::filename != NULL) free(Document::filename);
    Document::filename = strdup(filename);
    clear_data();
  }

  void Document::clear_data() 
  { 
    changed = false; 
  }

  bool Document::load() 
  { 
    std::ifstream ifs(get_filename());

    if (ifs.is_open())
      {
	ifs.close();
	// exit success
	changed = false; 
	return true; 
      }
    else
	return false; 
  }

  bool Document::save() { 
    changed = false; 
    return true; 
  }

}

