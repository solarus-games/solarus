#ifndef ZSDX_FILE_TOOLS_H
#define ZSDX_FILE_TOOLS_H

#include "Common.h"

/**
 * This class provides some functions to open and analyse a data file.
 * It encapsulates how the access to data files is done.
 *
 * The most important function here is data_file_add_prefix, which
 * takes as paramater a relative data file path and returns its real path
 * depending on how the data files are stored: as normal files, in an archive,
 * in the memory itself... This class is the only one which knows how to access
 * a data file.
 */
class FileTools {

 public:
  static std::string data_file_get_prefix(void);
  static std::string data_file_add_prefix(std::string file_name);
  static FILE *open_data_file(std::string file_name);

  static SDL_Surface *open_image(std::string file_name);

};

#endif
