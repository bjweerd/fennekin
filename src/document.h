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
    struct TreeModelNode* next;
    struct TreeModelNode* children;
    
    typedef enum {
      search_term, 
      search_engine, 
      engine_set 
    } NodeType;
    NodeType node_type;

    inline TreeModelNode(struct TreeModelNode* parent, NodeType node_type);

    /* each node holds 5 std::string objects, i think that's a bit overkill/too heavy especially with
     big trees, but let's worry about that later */
    std::string search_term_value;
    std::string search_term_engine_set;
    std::string engine_name;
    std::string engine_value;
    std::string engine_set_name;
  };



  inline TreeModelNode::TreeModelNode(struct TreeModelNode* parent, NodeType node_type)
    {
      next = children = NULL;
      this->parent = parent;
      this->node_type = node_type;
    }

  // the two functions below are mainly for testing purposes!

  inline void delete_node(TreeModelNode* node)
  {
    if (node)
      {
	TreeModelNode *p = node, *q;
	while (p->next)
	  {
	    if (p->children) 
	      delete_node(p->children);

	    q = p;
	    p = p->next;
	    delete q;
	  }
	delete p;
      }
  }
  inline TreeModelNode* create_test_tree_model()
  {
    TreeModelNode* root = new TreeModelNode(NULL, TreeModelNode::search_engine);

    TreeModelNode* q;
    root->engine_name = "Google";
    root->engine_value = "https://https://encrypted.google.com/#q=$encoded_term";
    q = root;

    q->next = new TreeModelNode(NULL,TreeModelNode::search_engine);
    q->next = q;
    q->engine_name = "Wikipedia";
    q->engine_value = "https://en.wikipedia.org/wiki/$encoded_term";

    q->next = new TreeModelNode(NULL, TreeModelNode::search_term);
    q->next = q;
    q->search_term_value = "Everyday life";

    q->next = new TreeModelNode(NULL, TreeModelNode::search_term);
    q->next = q;
    q->search_term_value = "Personal life";

    q->next = new TreeModelNode(NULL, TreeModelNode::search_term);
    q->next = q;
    q->search_term_value = "Computers";
    {
      TreeModelNode* r;
      r->children = new TreeModelNode(q, TreeModelNode::search_term);
      r->children->search_term_value = "Personal computer";
      r = q->children;

      r->next = new TreeModelNode(q, TreeModelNode::search_term);
      r->next = r;
      r->search_term_value = "Smartphone";

      r->next = new TreeModelNode(q, TreeModelNode::search_term);
      r->next = r;
      r->search_term_value = "Tablet computer";

      r->next = new TreeModelNode(q, TreeModelNode::search_term);
      r->next = r;
      r->search_term_value = "Laser printing";

      r->next = new TreeModelNode(q, TreeModelNode::search_term);
      r->next = r;
      r->search_term_value = "Smart TV";

      r->next = new TreeModelNode(q, TreeModelNode::search_term);
      r->next = r;
      r->search_term_value = "PlayStation 4";

      r->next = new TreeModelNode(q, TreeModelNode::search_term);
      r->next = r;
      r->search_term_value = "Raspberry Pi";

      r->next = new TreeModelNode(q, TreeModelNode::search_term);
      r->next = r;
      r->search_term_value = "Arduino";
    }

    q->next = new TreeModelNode(NULL, TreeModelNode::search_term);
    q->next = q;
    q->search_term_value = "Internet";

    q->next = new TreeModelNode(NULL, TreeModelNode::search_term);
    q->next = q;
    q->search_term_value = "Philosophy";

    q->next = new TreeModelNode(NULL, TreeModelNode::search_term);
    q->next = q;
    q->search_term_value = "Physics";

    q->next = new TreeModelNode(NULL, TreeModelNode::search_term);
    q->next = q;
    q->search_term_value = "Biology";

    return root;
  }




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
