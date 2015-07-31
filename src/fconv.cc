#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include <libxml/xmlreader.h>

#include <boost/algorithm/string.hpp>

enum in_type { in_fennekin, in_webdiver, in_text, in_freemind };
enum out_type { out_fennekin, out_freemind };


struct fennekin_tree
{
  void read_fennekin  (const std::string& filename) {}
  void read_webdiver  (const std::string& filename) {}
  void read_text      (const std::string& filename) {}
  void read_freemind  (const std::string& filename) {}
  void write_fennekin (const std::string& filename) {}
  void write_freemind (const std::string& filename) {}
};

int
main(int argc,char* argv[])
{
  [](){}; // I want c++11

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

  std::cout << "Converting... " << std::flush;
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
  std::cout << "done\n" << std::flush;

  return 0;
}

