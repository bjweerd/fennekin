#include "fennekin_tree.h"
#include <iostream>

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
      std::cout << "Input file extension is " 
		<< in_extension << " and can only be one of: .fennekin .webdiver .xml .mm .txt\n";
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
