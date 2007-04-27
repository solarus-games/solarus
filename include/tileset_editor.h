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

  /**
   * Name of the tileset file, for example
   * src/tilesets/house.zsd
   */
  char tileset_file_name[MAX_FILE_NAME];

  /**
   * Name of the tileset image file, for example
   * images/tilesets/house.png
   */
  char image_file_name[MAX_FILE_NAME];

  /**
   * Name of the C++ source file generated, for example
   * src/tilesets/house.cpp 
   */
  char source_file_name[MAX_FILE_NAME];

  /**
   * Name of the C++ header file generated, for example
   * include/tilesets/house.h 
   */
  char header_file_name[MAX_FILE_NAME];

  /**
   * Number of tiles in the tileset.
   */
  int nb_tiles;

  /**
   * Displays the control panel.
   * @param dst the surface
   * @param dst_rect position of the control panel on this surface
   */
  static void display_control_panel(SDL_Surface *dst, SDL_Rect &dst_rect);

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
