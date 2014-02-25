#include "datadir.h"

#include <algorithm>

#include <cstdio>
#include <cstdlib>
#include <cstring>


namespace app_util
{

  char* datadir_t::resource_dir = NULL;
  char* datadir_t::resource_path = NULL;
  datadir_t::~datadir_t()
  {
    if (resource_path != NULL) free(resource_path);
    if (resource_dir != NULL) free(resource_dir);
  }
  const char* datadir_t::get(const char* resource_name)
  {
    if (resource_dir == NULL) {
      if (set_resource_dir(resource_name) != 0) return NULL;
    }
    
    if (resource_path != NULL) {
      free(resource_path);
      resource_path = NULL;
    }
    
    std::string tmp = std::string(resource_dir);
    tmp += resource_name;
    resource_path = strdup(tmp.c_str());
    
    if (file_exists(resource_path) == true)
      return resource_path;
    else
      return NULL;
  }
  bool datadir_t::file_exists(const std::string& name) 
  {
    // using fopen() is the fastest portable way to check
    // if a file exists.
    
    if (FILE *file = fopen(name.c_str(), "r")) {
      fclose(file);
      return true;
    } else {
      return false;
    }   
  }
  int datadir_t::set_resource_dir(const char* resource_name)
  {
    std::string tmp;
		
#if 1	
    // try DATADIR
    tmp = std::string(datadir);
    tmp += resource_name;
    if (file_exists(tmp) == true)
      {
	resource_dir = strdup(datadir);
	return 0;
      }
#endif
    
#if 1
    // try current folder
    tmp = ".";
    tmp += resource_name;
    if (file_exists(tmp) == true)
      {
	resource_dir = strdup(".");
	return 0;
      }
#endif
    
#ifndef _WIN32
    
    return 1; // not windows? error out of here.
    
#else
    
    // Under windows we can try two things:
    //
    // 1) In the folder where the program resides.
    // 2) If the program is in $prefix/bin and
    //    the resources in $prefix/share/progname/
    //    figure out the current $prefix and take
    //		it from there.
    //
    // note: the code below will not work if the program
    // is located in the root folder of a drive.
		
    std::string prog_name = progname;
    std::replace(prog_name.begin(), prog_name.end(), '\\', '/');
    std::string data_dir = datadir;
    std::replace(data_dir.begin(), data_dir.end(), '\\', '/');
    
    // Try in the program folder
    size_t pos = prog_name.find_last_of("/");
    if (pos == std::string::npos)
      return 2; // give up right now, this should not happen under windwos.
    std::string prog_path = prog_name.substr(0, pos);
    if (file_exists(prog_path + resource_name) == true)
      {
	resource_dir = strdup(prog_path.c_str());
	return 0;
      }
    
    // Now, when we end up here, we have to do it the
    // hard way. we determine $prefix from program name,
    // and then we use that as new $prefix for the datadir,
    // assuming that datadir is of the form $prefix/share/$progname
    
    pos = prog_path.find_last_of("/");
    if (pos == std::string::npos)
      return 3; // this should not happen. maybe the program is located in the root folder.
    if (prog_path.substr(pos+1, std::string::npos) != "bin")
      return 4; // this does not look like a standard folder layout
			
    std::string prefix = prog_path.substr(0, pos); // save new prefix
    // now, investigate the data_dir string...
    pos = data_dir.find_last_of("/");;
    if (pos == std::string::npos)
      return 5; // should not happen
    
    // this next bit should capture the folder name as given
    // in configure.ac
    std::string datadir_tail = data_dir.substr(pos+1, std::string::npos);
    tmp = data_dir.substr(0, pos);
    pos = tmp.find_last_of("/");
    if (pos == std::string::npos)
      return 6; // huh? i want cookies
		
    // so now we can build a guess for the path.
    std::string path = prefix + "/" + 
      tmp.substr(pos+1,std::string::npos) + "/" + datadir_tail;
    // let's check it out...
    if (file_exists(path + resource_name) == true)
      {
	resource_dir = strdup(path.c_str());
	return 0;
      }
		
    return 7; // ok, we tried everything and failed.
#endif // _WIN32
  }

} // namespace app_util
