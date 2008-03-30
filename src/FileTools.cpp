#include "FileTools.h"

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

/**
 * @brief Returns the directory of the data files.
 * @return the directory of the data files
 */
const char *FileTools::data_file_get_prefix(void) {

  return DATADIR;
}

/**
 * @brief Returns the path of a data file.
 *
 * The \c file_name parameter is relative to the ZSDX data directory
 * (which could be for example /usr/local/share/zsdx/data or
 * C:\Program Files\zsdx\data).
 * This function returns the same file name, prefixed by the data directory. 
 *
 * @param file_name name of a data file
 */
const char *FileTools::data_file_add_prefix(const char *file_name) {

  static char prefixed_file_name[MAX_FILE_NAME];

  sprintf(prefixed_file_name, "%s/%s", DATADIR, file_name);
  return prefixed_file_name;
}

/**
 * @brief Opens in reading a file in the ZSDX data directory.
 * 
 * The file name is relative to the ZSDX data directory (which could be
 * for example /usr/local/share/zsdx/data or C:\Program Files\zsdx\data).
 *
 * @param file_name name of the file to open
 * @return the file, or NULL if it couldn't be open.
 */
FILE *FileTools::open_data_file(const char *file_name) {
  
  FILE *f;
  f = fopen(data_file_add_prefix(file_name), "r");
  
  return f;
}

/**
 * @brief Loads an image file.
 * 
 * The file name is relative to the ZSDX images directory (which could be
 * for example /usr/local/share/zsdx/data/images or C:\Program Files\zsdx\data\images).
 * The program is stopped with an error message if the image cannot be loaded.
 *
 * @param file_name name of the image file to open
 * @return the file
 */
SDL_Surface *FileTools::open_image(const char *file_name) {

  static char prefixed_file_name[MAX_FILE_NAME];

  sprintf(prefixed_file_name, "%s/images/%s", DATADIR, file_name);
  SDL_Surface *image = IMG_Load(prefixed_file_name);

  if (image == NULL) {
    cerr << "Cannot load image '" << prefixed_file_name << "'" << endl;
    exit(1);
  }

  return image;
}

/**
 * @brief Loads an image file.
 * 
 * The file name is relative to the ZSDX images directory (which could be
 * for example /usr/local/share/zsdx/data/images or C:\Program Files\zsdx\data\images).
 * The program is stopped with an error message if the image cannot be loaded.
 *
 * @param file_name name of the image file to open
 * @return the file
 */
SDL_Surface *FileTools::open_image(string file_name) {

  return open_image(file_name.c_str());
}
