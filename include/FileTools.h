/**
 * This module provides some functions to open and analyse a data file.
 */

#ifndef ZSDX_FILE_TOOLS_H
#define ZSDX_FILE_TOOLS_H

#include "Common.h"

#define MAX_FILE_NAME 512

class FileTools {

 public:
  static const char *data_file_get_prefix(void);
  static const char *data_file_add_prefix(const char *file_name);
  static FILE *open_data_file(const char *file_name);

};

#endif
