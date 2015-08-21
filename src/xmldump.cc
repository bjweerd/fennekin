#include "xmlstring.hpp"

#include <cstdarg>
#include <boost/algorithm/string.hpp>


int xmldump(const std::string& filename);
void xml_error_handler(void* ctx, const char* msg, ...);
xmlGenericErrorFunc xml_error_handler_ptr = &xml_error_handler;
std::string xml_error_msg;

int
main(int argc,char* argv[])
{

  for (int i = 1; i < argc; i++)
    {
      if (xmldump(argv[i]) != 0) 
	return 1;
    }

  return 0;
}

int xmldump(const std::string& filename)
{
  initGenericErrorDefaultFunc(&xml_error_handler_ptr);

  xmlTextReaderPtr reader = xmlNewTextReaderFilename(filename.c_str());

  if (reader)
    {
      int ret;

      while ((ret = xmlTextReaderRead(reader)) == 1)
	{
	  std::string name = xmlString(xmlTextReaderName(reader));
	  std::string value = xmlString(xmlTextReaderValue(reader));

	  boost::algorithm::trim(value); // frigging whitespaces everywhere
	  
	  int depth = xmlTextReaderDepth(reader);
	  int node_type = xmlTextReaderNodeType(reader);
	  int is_empty_element = xmlTextReaderIsEmptyElement(reader);

	  std::cout << "{ name=" << name << "; value=" << value << "; depth=" 
		    << depth << "; node_type=" << node_type << "; is_empty="
		    << is_empty_element << " --- ";

	  while (xmlTextReaderMoveToNextAttribute(reader))
	    {
	      std::string name = xmlString(xmlTextReaderName(reader));
	      std::string value = xmlString(xmlTextReaderValue(reader));

	      std::cout << name << "=\"" << value << "\" ";
	    }

	  std::cout << "}\n";
	}

      xmlFreeTextReader(reader);

      if (ret != 0) 
	{
	  std::cerr << xml_error_msg << std::flush;
	  return 2;
	}
    }
  else 
    {
      std::cerr << "XML Reader: Unable to open `" << filename << "' for reading.\n";
      return 1;
    }

  return 0;
}

void xml_error_handler(void* ctx, const char* msg, ...)
{
  const int tmp_buf_size = 256;
  char buf[tmp_buf_size];
  va_list arg_ptr;

  va_start(arg_ptr, msg);
  vsnprintf(buf, tmp_buf_size, msg, arg_ptr);
  va_end(arg_ptr);

  xml_error_msg += buf;
}

