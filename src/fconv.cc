#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include <libxml/xmlreader.h>

#include <boost/algorithm/string.hpp>

enum in_type { in_fennekin, in_webdiver, in_text, in_freemind };
enum out_type { out_fennekin, out_freemind };

class xmlstring_t
{
  xmlChar* data;
public:
  // http://www.xmlsoft.org/html/libxml-xmlstring.html
  xmlstring_t() : data(nullptr) {}
  xmlstring_t(xmlChar* str) : data(str?xmlStrdup(str):nullptr){}
  xmlstring_t(const xmlstring_t& u) : data(u.data?xmlStrdup(u.data):nullptr){}
  ~xmlstring_t() { if (data) xmlFree(data);}
  xmlChar* str(void) { return data; }
  void set(xmlChar* new_data) { if (new_data) { if (data) xmlFree(data); data = xmlStrdup(new_data); } }
  void set(const std::string& new_data) { if (data) xmlFree(data); data = xmlCharStrdup(new_data.c_str()); }
};

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

struct fennekin_tree
{
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




  void read_text      (const std::string& filename) {}




  struct fennekin_io_t {
  };

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

  void write_fennekin (const std::string& filename) 
  {
  }








  struct webdiver_io_t
  {
    fennekin_tree& tree;
    webdiver_io_t(fennekin_tree& tree) : tree(tree) {}

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
	  if (is_end(reader, (const char*)retval->name)) 
	    break;

	  if (is_begin(reader, "engine")) // global engine
	    {
	      engine_t* engine = parse_engine(reader);
	      if (engine) {
		tree.engines.push_back(*engine);
		delete engine;
	      }
	    }

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
	  ofs << "<node text=\"" << node->name << "\"/>\n";
	}
      else
	{
	  ofs << "<node text=\"" << node->name << "\">\n";

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










int
main(int argc,char* argv[])
{
  if (argc != 1+2)
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


  in_type input_type;
  out_type output_type;

  if (in_extension == ".fennekin") input_type = in_fennekin;
  else if (in_extension == ".webdiver" || in_extension == ".xml") input_type = in_webdiver;
  else if (in_extension == ".txt") input_type = in_text;
  else if (in_extension == ".mm") input_type = in_freemind;
  else 
    {
      std::cout << "Input file extension is " << in_extension << " and can only be one of: .fennekin .webdiver .xml .mm .txt\n";
      return 1;
    }

  if (out_extension == ".fennekin") output_type = out_fennekin;
  else if (out_extension == ".mm") output_type = out_freemind;
  else
    {
      std::cout << "Output file extension is " << out_extension << " and can only be one of: .fennekin .mm\n";
      return 1;
    }

  {
    fennekin_tree tree;

    // read the data file into the internal tree
    switch (input_type)
      {
      case in_fennekin:
	tree.read_fennekin(in_filename);
      break;
      case in_webdiver:
	tree.read_webdiver(in_filename);
	break;
      case in_text:
	tree.read_text(in_filename);
	break;
      case in_freemind:
	tree.read_freemind(in_filename);
	break;
      }

    // write internal tree to data file
    switch (output_type)
      {
      case out_fennekin:
	tree.write_fennekin(out_filename);
	break;
      case out_freemind:
	tree.write_freemind(out_filename);
	break;
      }
  }

  return 0;
}







