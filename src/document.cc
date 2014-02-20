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

