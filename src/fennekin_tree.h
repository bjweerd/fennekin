#ifndef INCLUDED_FENNEKIN_TREE_H
#define INCLUDED_FENNEKIN_TREE_H

/* 
 * This header defines the main program data structure, a tree of search terms,
 * engines and engine sets. It implements reading and writing of fennekin files, importing
 * of older 'webdiver' xml files, and it supports reading/writing Freemind .mm data files.
 */

#include <algorithm>
#include <cstdarg>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

#include <libxml/xmlreader.h>
#include "xmlstring.hpp"

struct fennekin_tree 
{
  // supported input/output formats
  enum in_type { in_fennekin, in_webdiver, in_text, in_freemind };
  enum out_type { out_fennekin, out_freemind };

  //
  // data types
  //

  struct engine
  {
    std::string name, query;	// query is a specially formed URL. 

    // SECURITY NOTE: because the file format contains URL's, there can be ATTACKS with the file format
    // <engine name="Google" query="http://123.321.23.34/evil/javascript/kill-the-user.html"/>
    // (see the 2015 CCCen 'Bad File Formats' talk on youtube)
  };

  struct engineset
  {
    std::string name;
    std::vector<engine> engines;
  };

  struct node
  {
    node* parent;
    std::vector<std::unique_ptr<node>> direct_children;
    std::string name;

    std::vector<engine> engines; // not filled in .mm
    std::string engineset;	 // not filled in .mm
  };

  //
  // data members
  // 

  node* root;

  std::vector<engine> engines;	// engines and enginesets are not used in .mm
  std::vector<engineset> enginesets;

  // 
  // constructor / destructor 
  //

  fennekin_tree() : root(nullptr) {}
  ~fennekin_tree() { if (root) delete root; }

  //
  // error handling for libxml2
  //

  static std::string xml_error_msg;
  static xmlGenericErrorFunc xml_error_handler_ptr;
  static void xml_error_handler(void* ctx, const char* msg, ...) {
    const int tmp_buf_size = 256;
    char buf[tmp_buf_size];
    va_list arg_ptr;
    
    va_start(arg_ptr, msg);
    vsnprintf(buf, tmp_buf_size, msg, arg_ptr);
    va_end(arg_ptr);
    
    xml_error_msg += buf;
  }
  // call this function before starting the xml parser
  void init_error_handler() {
    xml_error_msg = "";		// reset error message
    xml_error_handler_ptr = &xml_error_handler;
    initGenericErrorDefaultFunc(&xml_error_handler_ptr); 
  }

  //
  // format specific xml parsing
  //

  struct internal_parser
  {
    // this internal parser exists because the .fennekin and .webdiver file formats are identical, except
    // for the root node name.

    xmlTextReaderPtr reader;
    fennekin_tree& tree;
    std::string rootnode_name;

    internal_parser(xmlTextReaderPtr reader, fennekin_tree& tree, const std::string& rootnode_name) 
      : reader(reader), tree(tree), rootnode_name(rootnode_name) {}

    engine parse_engine() 
    {
      engine retval;
      
      std::string element_name = xmlString(xmlTextReaderName(reader));
      int node_type = xmlTextReaderNodeType(reader);
      int is_empty_element = xmlTextReaderIsEmptyElement(reader);

      if (element_name == "engine" && node_type == 1 && is_empty_element)
	{
	  std::string name;
	  std::string query;

	  while (xmlTextReaderMoveToNextAttribute(reader))
	    {
	      std::string n = xmlString(xmlTextReaderName(reader)).str();
	      if (n == "name")
		name = xmlString(xmlTextReaderValue(reader)).str();
	      else if (n == "query")
		query = xmlString(xmlTextReaderValue(reader)).str();
	    }

	  retval.name = name;
	  retval.query = query;
	}

      return retval;
    }

    node* parse_term(node* parent)
    {
      node* retval = new node;

      retval->parent = parent;
      int is_empty = xmlTextReaderIsEmptyElement(reader);

      // <term name="theName" engineset="theEngineSet">...</term>
      while (xmlTextReaderMoveToNextAttribute(reader))
	{
	  std::string s = xmlString(xmlTextReaderName(reader)).str();

	  if (s == "name")
	    retval->name = xmlString(xmlTextReaderValue(reader)).str();
	  else if (s == "engineset")
	    retval->engineset = xmlString(xmlTextReaderValue(reader)).str();
	}

      if (!is_empty)
	{
	  while (xmlTextReaderRead(reader) == 1)
	    {
	      std::string name = xmlString(xmlTextReaderName(reader));
	      int node_type = xmlTextReaderNodeType(reader);
	      int is_empty_element = xmlTextReaderIsEmptyElement(reader);

	      if (name == "term" && node_type == 15)
		break;
	      else if (name == "engine" && node_type == 1)
		retval->engines.push_back(parse_engine());
	      else if (name == "term" && node_type == 1)
		{
		  node* p = parse_term(retval);

		  if (p)
		    retval->direct_children.push_back(std::unique_ptr<node>(p));
		  else {
		    // error: something went wrong in the xml child parse
		    delete retval;
		    return nullptr;
		  }
		}
	    }
	}
      
      return retval;
    }

    node* parse()
    { 
      node* retval = new node;
      
      retval->parent = nullptr;
      retval->name = xmlString(xmlTextReaderName(reader)).str();

      int ret;

      while ((ret = xmlTextReaderRead(reader)) == 1)
	{
	  std::string name = xmlString(xmlTextReaderName(reader));
	  int node_type = xmlTextReaderNodeType(reader);
	  int is_empty_element = xmlTextReaderIsEmptyElement(reader);


	  if (name == rootnode_name && node_type == 15)
	    break;
	  else if (name == "engineset" && node_type == 1)
	    {
	      // parse an engineset
	      tree.enginesets.push_back(engineset());
	      engineset& back = tree.enginesets.back();
	      
	      // <engineset name="theName">
	      while (xmlTextReaderMoveToNextAttribute(reader))
		{
		  std::string s = xmlString(xmlTextReaderName(reader)).str();
		  if (s == "name")
		    back.name = xmlString(xmlTextReaderValue(reader)).str();
		}

	      // now parse them engines in this set
	      while (xmlTextReaderRead(reader) == 1)
		{
		  std::string name = xmlString(xmlTextReaderName(reader)).str();
		  int node_type = xmlTextReaderNodeType(reader);
		  
		  if (name == "engineset" && node_type == 15)
		    break;
		  else if (name == "engine" && node_type == 1)
		    back.engines.push_back(parse_engine());
		}
	    }
	  else if (name == "engine" && node_type == 1)
	    {
	      tree.engines.push_back(parse_engine());
	    }
	  else if (name == "term" && node_type == 1)
	    {
	      node* p = parse_term(retval);

	      if (p)
		retval->direct_children.push_back(std::unique_ptr<node>(p));
	      else {
		// error: something went wrong in the xml child parse
		delete retval;
		return nullptr;
	      }
	    }
	}

      return retval; 
    }
  };

  struct fennekin_parser : public internal_parser {
    fennekin_parser(xmlTextReaderPtr reader, fennekin_tree& tree) : internal_parser(reader, tree, "fennekin") {}
  };
  
  struct webdiver_parser : public internal_parser {
    webdiver_parser(xmlTextReaderPtr reader, fennekin_tree& tree) : internal_parser(reader, tree, "webdiver") {}
  };

  struct freemind_parser
  {
    xmlTextReaderPtr reader;
    freemind_parser(xmlTextReaderPtr reader) : reader(reader) {}
    
    node* parse_node(node* parent)
    {
      node* retval = new node;
      retval->parent = parent;
      int is_empty = xmlTextReaderIsEmptyElement(reader);

      while (xmlTextReaderMoveToNextAttribute(reader))
	{
	  std::string attr_name = xmlString(xmlTextReaderName(reader)).str();
	  std::transform(attr_name.begin(), attr_name.end(), attr_name.begin(), ::tolower);

	  if (attr_name == "text")
	    {
	      retval->name = xmlString(xmlTextReaderValue(reader)).str();
	    }
	}
      
      if (!is_empty)
	{
	  while (xmlTextReaderRead(reader) == 1)
	    {
	      std::string name = xmlString(xmlTextReaderName(reader)).str();
	      int node_type = xmlTextReaderNodeType(reader);

	      if (name == "node" && node_type == 15)
		break;
	      else if (name == "node" && node_type == 1) {
		node* p = parse_node(retval);

		if (p)
		  retval->direct_children.push_back(std::unique_ptr<node>(p));
		else {
		  // error: something went wrong in the xml child parse
		  delete retval;
		  return nullptr;
		}
	      }
	    }
	}

      return retval;
    }

    node* parse() 
    { 
      node* retval = new node;
      retval->parent = nullptr;
      retval->name = xmlString(xmlTextReaderName(reader)).str();

      int ret;
      while ((ret = xmlTextReaderRead(reader)) == 1)
	{
	  std::string name = xmlString(xmlTextReaderName(reader));
	  int node_type = xmlTextReaderNodeType(reader);

	  if (name == "node" && node_type == 1)
	    {
	      node* p = parse_node(retval);

	      if (p)
		retval->direct_children.push_back(std::unique_ptr<node>(p));
	      else {
		// error: somewhere in the xml children of me an error occured.
		delete retval;
		return nullptr;
	      }
	    }
	  else if (name == retval->name && node_type == 15)
	    break;
	}

      return retval; 
    }
  };

  int parse_xml(const std::string& filename, std::string& error_msg, in_type in)
  {
    init_error_handler();

    if (root) {
      error_msg = "parse_xml() root pointer not null. reader used twice?";
      return 4;
    }

    if (in == in_text) {
      error_msg = "in_type::in_text is not an xml format"; 
      return 3; 
    }

    xmlTextReaderPtr reader = xmlNewTextReaderFilename(filename.c_str());

    if (reader)
      {
	int ret;

	while ((ret = xmlTextReaderRead(reader)) == 1)
	  {
	    std::string name = xmlString(xmlTextReaderName(reader));
	    int node_type = xmlTextReaderNodeType(reader);
	    int is_empty_element = xmlTextReaderIsEmptyElement(reader);

	    switch (in)
	      {
	      case in_fennekin:
		if (name == "fennekin" && node_type == 1) {
		  if (!is_empty_element) {
		    root = fennekin_parser(reader,*this).parse();
		    if (!root) { error_msg = "error: parse() encountered an error"; return 1; }
		  }
		  else { error_msg = "error: root node is empty"; return 1; }
		}
		break;
	      case in_webdiver:
		if (name == "webdiver" && node_type == 1) {
		  if (!is_empty_element) {
		    root = webdiver_parser(reader,*this).parse();
		    if (!root) { error_msg = "error: parse() encountered an error"; return 1; }
		  }
		  else { error_msg = "error: root node is empty"; return 1; }
		}
		break;
	      case in_freemind:
		if (name == "map" && node_type == 1) {
		  if (!is_empty_element) {
		    root = freemind_parser(reader).parse();
		    if (!root) { error_msg = "error: parse() encountered an error"; return 1; }
		  }
		  else { error_msg = "error: root node is empty"; return 1; }
		}
		break;
	      }
	  }

	xmlFreeTextReader(reader);

	if (ret != 0) 
	  {
	    error_msg = xml_error_msg;
	    return 2;
	  }
      }
    else 
      {
	error_msg = std::string("XML Reader: Unable to open `") + filename + std::string("' for reading.\n");
	return 1;
      }

    return 0;
  }

  int read_fennekin(const std::string& filename, std::string& errmsg)  { return parse_xml(filename, errmsg, in_fennekin); }
  int read_webdiver(const std::string& filename, std::string& errmsg)  { return parse_xml(filename, errmsg, in_webdiver); }
  int read_freemind(const std::string& filename, std::string& errmsg)  { return parse_xml(filename, errmsg, in_freemind); }

  int read_text(const std::string& filename, std::string& errmsg)
  { 
    return -1; 
  }

  //
  // writing various output formats
  //

  // writing utility functions
  static void indent_spaces(std::ofstream& ofs, int level) {
    for (auto i = 0; i < level*2; ++i)
      ofs << ' ';
  }
  
  static void to_xml(std::ofstream& ofs, const std::string& str) 
  {
    for (auto c : str)
      switch (c) 
	{
	case '&': ofs << "&amp;"; break;
	case '<': ofs << "&lt;"; break;
	case '>': ofs << "&gt;"; break;
	case '\'': ofs << "&apos;"; break;
	case '\"': ofs << "&quot;"; break;
	default:
	  ofs << c;
	}
  }

  int write_fennekin_node(std::ofstream& ofs, int level, const std::unique_ptr<node>& n)
  {
    indent_spaces(ofs,level);

    if (n->direct_children.empty())
      {
	ofs << "<term ";
	if (!n->engineset.empty()) { ofs << "engineset=\""; to_xml(ofs, n->engineset); ofs << "\" "; }
	ofs << "name=\""; to_xml(ofs,n->name); ofs << "\"/>\n";
      }
    else
      {
	ofs << "<term ";
	if (!n->engineset.empty()) { ofs << "engineset=\""; to_xml(ofs,n->engineset); ofs << "\" "; }
	ofs << "name=\""; to_xml(ofs,n->name); ofs << "\">\n";
	
	for (auto e : n->engines) {
	  indent_spaces(ofs,level+1);
	  ofs << "<engine name=\""; to_xml(ofs,e.name); ofs << "\" query=\""; to_xml(ofs,e.query); ofs << "\"/>\n";
	}
	  
	for (auto& child : n->direct_children)
	  write_fennekin_node(ofs, level+1, child);
	
	indent_spaces(ofs,level);
	ofs << "</term>\n";
      }

    return 0;
  }

  int write_fennekin(const std::string& filename, std::string& errmsg) 
  { 
    std::ofstream ofs(filename.c_str());

    if (ofs.is_open())
      {
	ofs << "<fennekin version=\"1.0\">\n";

	for (auto engine : engines) {
	  indent_spaces(ofs,1);
	  ofs << "<engine name=\"";to_xml(ofs,engine.name);ofs<<"\" query=\"";to_xml(ofs,engine.query);ofs<<"\"/>\n";
	}
	
	for (auto engineset : enginesets) {
	  indent_spaces(ofs,1);
	  ofs << "<engineset name=\""; to_xml(ofs,engineset.name); ofs << "\">\n";
	  
	  for (auto engine : engineset.engines) {
	    indent_spaces(ofs,2);
	    ofs << "<engine name=\""; to_xml(ofs,engine.name); ofs << "\" query=\""; to_xml(ofs,engine.query); ofs<<"\"/>\n";
	  }

	  indent_spaces(ofs,1);
	  ofs << "</engineset>\n";
	}
    
	for (auto& node : root->direct_children)
	  write_fennekin_node(ofs, 1, node);
    
	ofs << "</fennekin>\n";
	ofs.close();
      }
    else {
      errmsg = "Can't open " + filename + " for writing";
      return 1;
    }
      
    return 0; 
  }

  int write_freemind_node(std::ofstream& ofs, int level, const std::unique_ptr<node>& n)
  {
    indent_spaces(ofs,level);

    if (n->direct_children.empty())
      {
	ofs << "<node text=\""; to_xml(ofs,n->name); ofs << "\"/>\n";
      }
    else
      {
	ofs << "<node text=\""; to_xml(ofs,n->name); ofs << "\">\n";
	
	for (auto& child : n->direct_children)
	  write_freemind_node(ofs, level+1, child);
	
	indent_spaces(ofs,level);
	ofs << "</node>\n";
      }

    return 0;
  }

  int write_freemind(const std::string& filename, std::string& errmsg) 
  { 
    std::ofstream ofs(filename.c_str());

    if (ofs.is_open()) 
      {
	// we always write an empty root node because .mm can't have more than one root <node> element (xml forbids it)
	ofs << "<map version=\"0.9.0\">\n<node text=\"\">\n";
	
	for (auto& child : root->direct_children)
	  write_freemind_node(ofs, 1, child);
	
	ofs << "</node>\n</map>\n";
	ofs.close();
      }
    else
      {
	errmsg = "Can't open " + filename + " for writing";
	return 1;
      }

    return 0;
  }
};



#endif
