#ifndef ZSDX_FILE_TOOLS_H
#define ZSDX_FILE_TOOLS_H

#include "Common.h"

#define MAX_FILE_NAME 512

/**
 * This class provides some functions to open and analyse a data file.
 */
class FileTools {

 public:
  static const char *data_file_get_prefix(void);
  static const char *data_file_add_prefix(const char *file_name);
  static FILE *open_data_file(const char *file_name);

  static SDL_Surface *open_image(const char *file_name);
  static SDL_Surface *open_image(string file_name);

};

#endif
