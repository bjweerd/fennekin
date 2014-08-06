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
    delete_node(root);
    root = NULL;
    std::cout << "\n\n*** DELETED HARDCODED TEST TREE\n\n";
        
    changed = false; 
  }

  bool Document::load() 
  { 
    
    /* SPEED CONSIDERATIONS

       Firstly, you should detach your list store or tree store from the tree view before doing your mass insertions, then do your insertions, and only connect your store to the tree view again when you are done with your insertions. Like this:

  ...

  model = gtk_tree_view_get_model(GTK_TREE_VIEW(view));

  g_object_ref(model); // Make sure the model stays with us after the tree view unrefs it 

  gtk_tree_view_set_model(GTK_TREE_VIEW(view), NULL); // Detach model from view 

  ... insert a couple of thousand rows ...

  gtk_tree_view_set_model(GTK_TREE_VIEW(view), model); // Re-attach model to view

  g_object_unref(model);

  ...


    Secondly, you should make sure that sorting is disabled while you are doing your mass insertions, otherwise your store might be resorted after each and every single row insertion, which is going to be everything but fast.

     Thirdly, you should not keep around a lot of tree row references if you have so many rows, because with each insertion (or removal) every single tree row reference will check whether its path needs to be updated or not.
       

    */

    std::ifstream ifs(get_filename());

    if (ifs.is_open())
      {
	ifs.close();



	//        TEST TEST TEST
	root = create_test_tree_model();

	std::cout << "\n\n*** CREATED HARDCODED TEST TREE\n\n";

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

