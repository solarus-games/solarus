/**
 * This module defines the class TilesetEditor.
 */

#ifndef ZSDX_TILESET_EDITOR_H
#define ZSDX_TILESET_EDITOR_H

#include <SDL/SDL.h>
#include "tile_image.h"

#define MAX_FILE_NAME 256
#define MAX_TILESET_NAME 128

/**
 * This class provides fonctionnalities
 * to edit the properties of a tileset.
 * This properties are stored in a file.
 */
class TilesetEditor {

 private:

  char tileset_file_name[MAX_FILE_NAME];

  char image_file_name[MAX_FILE_NAME];

  char source_file_name[MAX_FILE_NAME];

  char header_file_name[MAX_FILE_NAME];

  /**
   * Number of tiles in the tileset.
   */
  int nb_tiles;

  /**
   * The tiles.
   */
  TileImage *tiles[1024];

 public:

  /**
   * Creates a tileset editor.
   * @param tileset_name name of the tileset to edit
   * (if the corresponding file doesn't exist, this will be a new tileset)
   */
  TilesetEditor(const char *tileset_name);

  /**
   * Destructor.
   */
  ~TilesetEditor(void);

  /**
   * Launches the editor and displays it on an SDL surface.
   * @param src_surface surface of the editor
   */
  void launch(SDL_Surface *src_surface);

  /**
   * Saves the tileset file.
   */
  void save(void);

  /**
   * Generates the C++ file.
   */
  void generate_code(void);

};

#endif
