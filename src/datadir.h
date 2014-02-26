#ifndef INCLUDED_DATADIR_H
#define INCLUDED_DATADIR_H

#include <string>

namespace app_util
{

  class datadir_t
  {
    //
    // This class determines the location of the program 
    // resources at runtime when running under Microsoft Windows.
    //
    // class datadir_t usage:
    //
    // datadir_t datadir(argv[0], DATADIR);
    // ... your code ...
    // const char* icon_filename = datadir.get("/simba.png");
    // ... your code
    //

    const char* progname;
    const char* datadir;
    static char* resource_dir;
    static char* resource_path;

  public:

    datadir_t(const char* progname, const char* datadir)
      : progname(progname), datadir(datadir) {}
    ~datadir_t();

    const char* get(const char* resource_name);

  private:

    bool file_exists(const std::string& name) ;
    int set_resource_dir(const char* resource_name);
  };

}

#endif
