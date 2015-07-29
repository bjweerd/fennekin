#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include <libxml/xmlreader.h>

#include <boost/algorithm/string.hpp>

int
main(int argc,char* argv[])
{
  [](){};

  if (argc != 1+2)
    {

      std::cout << "The purpose of this tool is to have a command line version of the"
	" fennekin import/export functionality and an internal memory representation"
	" of the resulting Fennekin data structure.\n\n"
	"Usage: fconv <input file> <output file>\n\n"
	"Input can be .fennekin .webdiver .xml .txt .mm\n"
	"Output is in .fennekin or .mm format\n\n"
	;

      return 1;
    }

  std::string in_filename = argv[1];
  std::string out_filename = argv[2];

  std::cout << "input file:  " << in_filename << "\n";
  std::cout << "output file: " << out_filename << "\n";
  return 0;
}

