#ifndef ZSDX_FILE_TOOLS_H
#define ZSDX_FILE_TOOLS_H

#include "Common.h"

/**
 * This class provides some functions to open and analyse a data file.
 */
class FileTools {

 public:
  static string data_file_get_prefix(void);
  static string data_file_add_prefix(string file_name);
  static FILE *open_data_file(string ile_name);

  static SDL_Surface *open_image(string file_name);

};

#endif
