/**
 * This module provides some functions to open and analyse a data file.
 */

#ifndef ZSDX_FILE_TOOLS_H
#define ZSDX_FILE_TOOLS_H

#include "Common.h"

#define MAX_FILE_NAME 512

/**
 * @brief Converts the parameter as a <code>char*</code> constant string.
 * @param x the text to convert
 */
#define STRING(x) STRING2(x)
#define STRING2(x) #x

/*
 * If we are not under Windows, the data files will be picked
 * in the data directory (this is the normal behavior on Unix environments when the
 * user compiles the program). Note that the data directory is set at the
 * compilation time and hardcoded in the binary.
 * Otherwise, the data directory is relative to the current directory (which should
 * be the location of the executable file). This is the normal behavior on Windows
 * platforms, because the user downloads a binary version of the game.
 */
#ifdef WINDOWS
#define DATADIR "./data"
#else
#define DATADIR STRING(DATADIR_)
#endif

class FileTools {

 public:
  static FILE *open_data_file(const char *file_name);

};

#endif
