#include "FileTools.h"

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
  char file_name_in_datadir[MAX_FILE_NAME];
  
  sprintf(file_name_in_datadir, "%s/%s", DATADIR, file_name);
  f = fopen(file_name_in_datadir, "r");
  
  return f;
}
