#ifndef INCLUDED_XMLSTRING_HPP
#define INCLUDED_XMLSTRING_HPP

#include <iostream>
#include <string>
#include <libxml/xmlstring.h>

// http://www.xmlsoft.org/html/libxml-xmlstring.html
class xmlString
{
  xmlChar* data;		// naked pointer, allowed to be null

public:

  xmlString() : data(nullptr) {}
  xmlString(const xmlString& str) : data(str.data ? xmlStrdup(str.data) : nullptr) {}
  ~xmlString() { if (data) xmlFree(data); }

  // copy construction/assignment
  xmlString(xmlChar* str)           : data(nullptr) { set(str); }
  xmlString(const std::string& str) : data(nullptr) { set(str); }
  xmlString(const char* str)        : data(nullptr) { set(str); }

  void set(xmlChar* str)           { if (str) { if (data) xmlFree(data); data = xmlStrdup(str); } }
  void set(const std::string& str) {            if (data) xmlFree(data); data = xmlCharStrdup(str.c_str()); }
  void set(const char* str)        { if (str) { if (data) xmlFree(data); data = xmlCharStrdup(str); } }

  // accessors/conversion
  operator xmlChar*()     { return get(); }
  operator char*()        { return c_str(); }
  operator std::string()  { return str(); }

  xmlChar*    get(void)   { return data; }
  char*       c_str(void) { return reinterpret_cast<char*>(data); }
  std::string str(void)   { if (!c_str()) set(""); return std::string(c_str()); } // not 100% happy with this set()

  // output stream
  friend std::ostream& operator<< (std::ostream& os, xmlString& str) { 
    xmlChar* s = str.get(); 
    if (!s) s = BAD_CAST ""; 
    os << s; 
    return os; 
  }
};


#endif /* INCLUDED_XMLSTRING_HPP */
