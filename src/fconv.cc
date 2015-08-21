#include <algorithm>
#include <cstdarg>
#include <fstream>
#include <iostream>
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
  int read_text(const std::string& filename, std::string& errmsg)      { return -1; }

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
// fennekin_tree static data members
std::string fennekin_tree::xml_error_msg;
xmlGenericErrorFunc fennekin_tree::xml_error_handler_ptr;



int
main(int argc,char* argv[])
{
  if (argc != 3)
    {

      std::cout << "The purpose of this tool is to have a command line version of the"
	" Fennekin import/export functionality and use the internal memory representation"
	" of the Fennekin tree data structure.\n\n"
	"Usage: fconv <input file> <output file>\n\n"
	"Input can be .fennekin .webdiver .xml .txt .mm\n"
	"Output is in .fennekin or .mm format\n\n"
	"The .mm format is the format used by the Freemind mindmapping software.\n"
	;

      return 1;
    }

  std::string in_filename = argv[1];
  std::string out_filename = argv[2];

  // get file extensions
  size_t pos;
  pos = in_filename.find_last_of(".");
  if (pos == std::string::npos) { std::cout << "Input file has no extension: " << in_filename << std::endl; return 1; }
  auto in_extension = std::string(in_filename.begin()+pos, in_filename.end());
  pos = out_filename.find_last_of(".");
  if (pos == std::string::npos) { std::cout << "Output file has no extension: " << out_filename << std::endl; return 1; }
  auto out_extension = std::string(out_filename.begin()+pos, out_filename.end());


  fennekin_tree::in_type input_type;
  fennekin_tree::out_type output_type;

  if (in_extension == ".fennekin") input_type = fennekin_tree::in_fennekin;
  else if (in_extension == ".webdiver" || in_extension == ".xml") input_type = fennekin_tree::in_webdiver;
  else if (in_extension == ".txt") input_type = fennekin_tree::in_text;
  else if (in_extension == ".mm") input_type = fennekin_tree::in_freemind;
  else 
    {
      std::cout << "Input file extension is " << in_extension << " and can only be one of: .fennekin .webdiver .xml .mm .txt\n";
      return 1;
    }

  if (out_extension == ".fennekin") output_type = fennekin_tree::out_fennekin;
  else if (out_extension == ".mm") output_type = fennekin_tree::out_freemind;
  else {
    std::cout << "Output file extension is " << out_extension << " and can only be one of: .fennekin .mm\n";
    return 1;
  }

  // the actual conversion work
  {
    fennekin_tree tree;
    int ret;
    std::string errmsg;

    // read the data file into the internal tree
    switch (input_type)
      {
      case fennekin_tree::in_fennekin:
	ret = tree.read_fennekin(in_filename, errmsg);
      break;
      case fennekin_tree::in_webdiver:
	ret = tree.read_webdiver(in_filename, errmsg);
	break;
      case fennekin_tree::in_text:
	ret = tree.read_text(in_filename, errmsg);
	break;
      case fennekin_tree::in_freemind:
	ret = tree.read_freemind(in_filename, errmsg);
	break;
      }

    if (ret) 			// error reading input file
      {
	std::cout << "input error: " << errmsg << std::endl;
	return 1;
      }

    // write internal tree to data file
    switch (output_type)
      {
      case fennekin_tree::out_fennekin:
	ret = tree.write_fennekin(out_filename, errmsg);
	break;
      case fennekin_tree::out_freemind:
	ret = tree.write_freemind(out_filename, errmsg);
	break;
      }

    if (ret) 			// error writing output file
      {
	std::cout << "output error: " << errmsg << std::endl;
	return 1;
      }
  }

  return 0;
}


























struct fennekin_tree_XXX
{

struct engine_t
{
  xmlChar* name;
  xmlChar* query;

  engine_t() : name(nullptr), query(nullptr) {}
  engine_t(const engine_t& e) : engine_t(e.name,e.query) {}
  engine_t(xmlChar* name, xmlChar* query) : name((name) ? xmlStrdup(name) : nullptr), query((query) ? xmlStrdup(query) : nullptr) {}
  ~engine_t() { if (name) xmlFree(name); if (query) xmlFree(query); }
};

struct node_t
{
  node_t* parent;
  std::vector<node_t*> directChildren;
  xmlChar* name;

  std::vector<engine_t> engines; // not filled in .mm
  xmlChar* engineset; // not filled in .mm
};

struct engineset_t
{
  xmlChar* name;
  std::vector<engine_t> engines;

  engineset_t() : name(nullptr) {}
  ~engineset_t() { if (name) xmlFree(name); }
  void set_name(xmlChar* name) { engineset_t::name = xmlStrdup(name); }
};





  node_t* root;
  std::vector<engine_t> engines;
  std::vector<engineset_t> enginesets;

  static bool is_begin(xmlTextReaderPtr reader, const std::string& compare) {
    return std::string((const char*)xmlTextReaderName(reader)) == compare && xmlTextReaderNodeType(reader) == 1;
  }
  static bool is_end(xmlTextReaderPtr reader, const std::string& compare) {
    return std::string((const char*)xmlTextReaderName(reader)) == compare && xmlTextReaderNodeType(reader) == 15;
  }
  static void indent_spaces(std::ofstream& ofs, int level) {
    for (auto i = 0; i < level*2; ++i)
      ofs << ' ';
  }

  static void to_xml(std::ofstream& ofs, xmlChar* str) {
    for (xmlChar* s = str; *s; s++)
      switch (*s) 
	{
	case '&': ofs << "&amp;"; break;
	case '<': ofs << "&lt;"; break;
	case '>': ofs << "&gt;"; break;
	case '\'': ofs << "&apos;"; break;
	case '\"': ofs << "&quot;"; break;
	default:
	  ofs << *s;
	}
  }
  








  void read_text      (const std::string& filename) {}




  struct fennekin_io_t {
    fennekin_tree_XXX& tree;
    fennekin_io_t(fennekin_tree_XXX& tree) : tree(tree) {}
    
    void write_node(std::ofstream& ofs, int level, node_t* node)
    {
      indent_spaces(ofs,level);
      if (node->directChildren.empty())
	{
	  ofs << "<term ";
	  if (node->engineset) { ofs << "engineset=\""; to_xml(ofs, node->engineset); ofs << "\" "; }
	  ofs << "name=\""; to_xml(ofs,node->name); ofs << "\"/>\n";
	}
      else
	{
	  ofs << "<term ";
	  if (node->engineset) { ofs << "engineset=\""; to_xml(ofs,node->engineset); ofs << "\" "; }
	  ofs << "name=\""; to_xml(ofs,node->name); ofs << "\">\n";

	  for (auto engine : node->engines) {
	    indent_spaces(ofs,level+1);
	    ofs << "<engine name=\""; to_xml(ofs,engine.name); ofs << "\" query=\""; to_xml(ofs,engine.query); ofs << "\"/>\n";
	  }
	  
	  for (auto child : node->directChildren)
	    write_node(ofs, level+1, child);
	  
	  indent_spaces(ofs,level);
	  ofs << "</term>\n";
	}
    }
  };

  void write_fennekin (const std::string& filename) 
  {
    fennekin_io_t fennekin_io(*this);
    
    std::ofstream ofs(filename.c_str());
    // we always write an empty root node because .mm can't have more than one root <node> element
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
    
    for (auto node : root->directChildren)
      fennekin_io.write_node(ofs, 1, node);
    
    ofs << "</fennekin>\n";
    ofs.close();
  }



  // straight copy from read_webdiver() 
  void read_fennekin  (const std::string& filename) 
  {
    webdiver_io_t webdiver_io(*this);
    xmlTextReaderPtr reader;
    int ret;

    reader = xmlNewTextReaderFilename(filename.c_str());
    root = nullptr;

    if (reader != nullptr) 
      {
	ret = xmlTextReaderRead(reader);

	while (ret == 1) 
	  {
	    if (is_begin(reader, "fennekin"))
	      {
		if (root) {
		  // error: two map nodes in .xml file
		  return;
		}
		else {
		  root = webdiver_io.parse_webdiver(reader);
		  break;
		}
	      }

	    ret = xmlTextReaderRead(reader);
	  }

	xmlFreeTextReader(reader);
	if (ret != 0) { return; /* failed to parse xml */ }
      }
    else
      {
	return; /* unable to open file */
      }
  }







  struct webdiver_io_t
  {
    fennekin_tree_XXX& tree;
    webdiver_io_t(fennekin_tree_XXX& tree) : tree(tree) {}

    engine_t* parse_engine(xmlTextReaderPtr reader)
    {
      engine_t* retval = nullptr;

      if (is_begin(reader, "engine")) // global engine
	{
	  // <engine name="theName" query="theQuery"/>
	  xmlChar* name = nullptr;
	  xmlChar* query = nullptr;
	  
	  while (xmlTextReaderMoveToNextAttribute(reader))
	    {
	      if (xmlStrcasecmp(xmlTextReaderName(reader), BAD_CAST "name") == 0)
		name = xmlStrdup(xmlTextReaderValue(reader));
	      else if (xmlStrcasecmp(xmlTextReaderName(reader), BAD_CAST "query") == 0)
		query = xmlStrdup(xmlTextReaderValue(reader));
	    }

	  if (name && query)
	    retval = new engine_t(name, query);

	  if (name) xmlFree(name);
	  if (query) xmlFree(query);
	}

      return retval;
    }

    node_t* parse_webdiver(xmlTextReaderPtr reader)
    {
      node_t* retval = new node_t;
      retval->parent = nullptr;
      retval->name = xmlStrdup(xmlTextReaderName(reader)); // the root node is always called 'webdiver' (unless it's 'fennekin':)
      
      while (xmlTextReaderRead(reader) == 1)
	{
	  if (0) { // DEBUG
	    xmlChar* tmp = xmlStrdup(xmlTextReaderName(reader));
	    std::cout << "{" << tmp << "}" << std::flush;
	    xmlFree(tmp);
	  }

	  if (is_end(reader, (const char*)retval->name)) // this ends the root node (whatever it's called)
	    break;

	  if (is_begin(reader, "engineset")) // enginesets are always global
	    {
	      // create a new engine set and get a ref to it.
	      tree.enginesets.push_back(engineset_t());
	      engineset_t& back = tree.enginesets.back();

	      // <engineset name="theName">
	      xmlChar* name = nullptr;
	  
	      while (xmlTextReaderMoveToNextAttribute(reader))
		{
		  if (xmlStrcasecmp(xmlTextReaderName(reader), BAD_CAST "name") == 0)
		    name = xmlStrdup(xmlTextReaderValue(reader));
		}
	      
	      if (name) {
		back.set_name(name);
		xmlFree(name);
	      }

	      while (xmlTextReaderRead(reader) == 1)
		{
		  if (is_end(reader, "engineset")) break;

		  if (is_begin(reader, "engine")) {
		    auto engine = parse_engine(reader);
		    if (engine) {
		      back.engines.push_back(*engine);
		      delete engine;
		    }
		  }
		}
	    } // done with <engineset>...</engineset>

	  if (is_begin(reader, "term"))
	    retval->directChildren.push_back(parse_term(reader, retval));

	  if (is_begin(reader, "engine")) // global engine
	    {
	      engine_t* engine = parse_engine(reader);
	      if (engine) {
		tree.engines.push_back(*engine);
		delete engine;
	      }
	    }	  
	}
      
      return retval;
    }

    node_t* parse_term(xmlTextReaderPtr reader, node_t* parent) 
    {
      node_t* retval = new node_t;
      retval->parent = parent;
      retval->name = nullptr;
      retval->engineset = nullptr;
      int is_empty = xmlTextReaderIsEmptyElement(reader);

      // <term name="theName" engineset="theEngineSet">
      xmlChar* name = nullptr;
      xmlChar* engineset = nullptr;
	  
      while (xmlTextReaderMoveToNextAttribute(reader))
	{
	  if (xmlStrcasecmp(xmlTextReaderName(reader), BAD_CAST "name") == 0) {
	    if (name) xmlFree(name);
	    name = xmlStrdup(xmlTextReaderValue(reader));
	  }
	  else if (xmlStrcasecmp(xmlTextReaderName(reader), BAD_CAST "engineset") == 0) {
	    if (engineset) xmlFree(engineset);
	    engineset = xmlStrdup(xmlTextReaderValue(reader));
	  }
	}

      if (name) {
	retval->name = xmlStrdup(name);
	xmlFree(name);
      }
      if (engineset) {
	retval->engineset = xmlStrdup(engineset);
	xmlFree(engineset);
      }
      

      if (!is_empty)
	{
	  while (xmlTextReaderRead(reader) == 1)
	    {
	      if (is_end(reader, "term"))
		break;
	      if (is_begin(reader, "engine")) {
		auto engine = parse_engine(reader);
		if (engine) {
		  retval->engines.push_back(*engine);
		  delete engine;
		}
	      }

	      if (is_begin(reader, "term"))
		retval->directChildren.push_back(parse_term(reader,retval));
	    }
	}

      return retval;
    }
  };

  void read_webdiver  (const std::string& filename) 
  {
    webdiver_io_t webdiver_io(*this);
    xmlTextReaderPtr reader;
    int ret;

    reader = xmlNewTextReaderFilename(filename.c_str());
    root = nullptr;

    if (reader != nullptr) 
      {
	ret = xmlTextReaderRead(reader);

	while (ret == 1) 
	  {
	    if (is_begin(reader, "webdiver"))
	      {
		if (root) {
		  // error: two map nodes in .xml file
		  return;
		}
		else {
		  root = webdiver_io.parse_webdiver(reader);
		  break;
		}
	      }

	    ret = xmlTextReaderRead(reader);
	  }

	xmlFreeTextReader(reader);
	if (ret != 0) { return; /* failed to parse xml */ }
      }
    else
      {
	return; /* unable to open file */
      }
  }




  //
  // BELOW THIS LINE IS ALREADY DONE
  //




  struct freemind_io_t
  {
    node_t* parse_map(xmlTextReaderPtr reader)
    {
      node_t* retval = new node_t;
      retval->parent = nullptr;
      retval->name = xmlStrdup(xmlTextReaderName(reader));
      
      while (xmlTextReaderRead(reader) == 1)
	{
	  if (is_begin(reader, "node"))
	    retval->directChildren.push_back(parse_node(reader, retval));
	  if (is_end(reader, "map")) 
	    break;
	}
      
      return retval;
    }
    node_t* parse_node(xmlTextReaderPtr reader, node_t* parent) 
    {
      node_t* retval = new node_t;
      retval->parent = parent;
      retval->name = nullptr;
      retval->engineset = nullptr;
      int is_empty = xmlTextReaderIsEmptyElement(reader);

      while (xmlTextReaderMoveToNextAttribute(reader))
	{
	  std::string attr_name{(const char*)xmlTextReaderName(reader)};
	  std::transform(attr_name.begin(), attr_name.end(), attr_name.begin(), ::tolower);

	  if (attr_name == "text")
	    {
	      retval->name = xmlStrdup(xmlTextReaderValue(reader));
	    }
	}
      
      if (!is_empty)
	{
	  while (xmlTextReaderRead(reader) == 1)
	    {
	      if (is_end(reader, "node"))
		break;

	      if (is_begin(reader, "node"))
		retval->directChildren.push_back(parse_node(reader,retval));
	    }
	}

      return retval;
    }

    void write_node(std::ofstream& ofs, int level, node_t* node)
    {
      indent_spaces(ofs,level);
      if (node->directChildren.empty())
	{
	  ofs << "<node text=\""; to_xml(ofs,node->name); ofs << "\"/>\n";
	}
      else
	{
	  ofs << "<node text=\""; to_xml(ofs,node->name); ofs << "\">\n";

	  for (auto child : node->directChildren)
	    write_node(ofs, level+1, child);

	  indent_spaces(ofs,level);
	  ofs << "</node>\n";
	}
    }
  }; // freemind_io_t


  void read_freemind  (const std::string& filename) 
  {
    freemind_io_t freemind_io;
    xmlTextReaderPtr reader;
    int ret;

    reader = xmlNewTextReaderFilename(filename.c_str());
    root = nullptr;

    if (reader != nullptr) 
      {
	ret = xmlTextReaderRead(reader);

	while (ret == 1) 
	  {
	    if (is_begin(reader, "map"))
	      {
		if (root) {
		  // error: two map nodes in .xml file
		  return;
		}
		else {
		  root = freemind_io.parse_map(reader);
		  break;
		}
	      }
	    ret = xmlTextReaderRead(reader);
	  }

	xmlFreeTextReader(reader);
	if (ret != 0) { return; /* failed to parse xml */ }
      }
    else
      {
	return; /* unable to open file */
      }
  }

  void write_freemind (const std::string& filename) 
  {
    freemind_io_t freemind_io;

    std::ofstream ofs(filename.c_str());
    // we always write an empty root node because .mm can't have more than one root <node> element
    ofs << "<map version=\"0.9.0\"><node text=\"\">\n";
    
    for (auto node : root->directChildren)
      freemind_io.write_node(ofs, 1, node);

    ofs << "</node></map>\n";
    ofs.close();
  }
};







