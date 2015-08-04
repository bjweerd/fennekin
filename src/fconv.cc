#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include <libxml/xmlreader.h>

#include <boost/algorithm/string.hpp>

enum in_type { in_fennekin, in_webdiver, in_text, in_freemind };
enum out_type { out_fennekin, out_freemind };

struct engine_t
{
  xmlChar* name;
  xmlChar* query;

  engine_t(xmlChar* name, xmlChar* query) : name((name) ? xmlStrdup(name) : nullptr), query((query) ? xmlStrdup(query) : nullptr) {}
  ~engine_t() { if (name) xmlFree(name); if (query) xmlFree(query); }
};

struct node_t
{
  node_t* parent;
  std::vector<node_t*> directChildren;
  xmlChar* name;

  std::vector<engine_t> engines; // not filled in .mm
  std::vector<std::string> engineSets; // not filled in .mm
};

struct fennekin_tree
{
  node_t* root;

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

  void read_webdiver  (const std::string& filename) {}
  void read_text      (const std::string& filename) {}

  void read_fennekin  (const std::string& filename) {}
  void write_fennekin (const std::string& filename) {}

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
	      if (root) {
		// error: two map nodes in .xml file
		return;
	      }
	      else
		root = freemind_io.parse_map(reader);

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
    ofs << "<map version=\"0.9.0\">\n";
    
    for (auto node : root->directChildren)
      freemind_io.write_node(ofs, 1, node);

    ofs << "</map>\n";
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

  std::cout << "Input file:  " << in_filename << " (extension = " << in_extension << ")\n";
  std::cout << "Output file: " << out_filename << " (extension = " << out_extension << ")\n\n";

  //std::cout << "Converting... " << std::flush;
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
  //std::cout << "done\n" << std::flush;

  return 0;
}










#if 0
    void xxx_read_freemind_process_file(xmlTextReaderPtr reader) {
      xmlChar* name = nullptr;
      xmlChar* value = nullptr;
      
      if (xmlTextReaderNodeType(reader) == 14) return;
      
      std::cout << "node info: ";
      name = xmlTextReaderName(reader);
      value = xmlTextReaderValue(reader); // not used 
      std::cout << "name = " << name << ". ";
      std::cout << "depth = " << xmlTextReaderDepth(reader) << ". ";
      std::cout << "nodetype = " << xmlTextReaderNodeType(reader) << ". ";
      std::cout << "is_empty = " << xmlTextReaderIsEmptyElement(reader) << ". ";
      std::cout << std::endl;
      
      if (xmlTextReaderNodeType(reader) == 1)
	{
	  std::cout << "  - attributes: ";
	  while (xmlTextReaderMoveToNextAttribute(reader))
	    {
	      std::cout << "[" << xmlTextReaderName(reader) << "=\"" << xmlTextReaderValue(reader) << "\"] ";
	    }
	}
      
      std::cout << std::endl;

      if (name) xmlFree(name);
      if (value) xmlFree(value);
    }
    
    void xxx_read_freemind  (const std::string& filename) {
      xmlTextReaderPtr reader;
      int ret;
      reader = xmlNewTextReaderFilename(filename.c_str());
      if (reader != nullptr) 
	{
	  ret = xmlTextReaderRead(reader);
	  while (ret == 1) {
	    //read_freemind_process_file(reader);
	    ret = xmlTextReaderRead(reader);
	  }
	  xmlFreeTextReader(reader);
	  if (ret != 0) { /* failed to parse xml */ }
	}
      else
	{
	  /* unable to open file */
	}
    }

#endif // 0
