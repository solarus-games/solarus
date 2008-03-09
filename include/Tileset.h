/**
 * This module defines the class Tileset.
 */

#ifndef ZSDX_TILESET_H
#define ZSDX_TILESET_H

#include "Common.h"
#include "Color.h"

/**
 * A tileset is an image with a set of elements (tiles)
 * one can use to compose a map.
 * See the directory images/tilesets.
 */
class Tileset {

 private:
  /**
   * Id of the tileset;
   */
  TilesetId id;

  /**
   * Tiles of the tileset.
   * The maximum id is 1024.
   */
  Tile *tiles[1024];
  
  /**
   * Number of tiles in the tileset.
   */
  int nb_tiles;
  
  /**
   * Background color of the tileset.
   */
  zsdx_color_t background_color;

  /**
   * Image from which the tiles are extracted.
   */
  SDL_Surface *tileset_image;
  
  /**
   * Creates a new tile in the tileset.
   * This function is called by load().
   * @param id id of this tile (1 to 1024)
   * @param tile the tile to add
   */
  void create_tile(int id, Tile *tile);

 public:

  Tileset(TilesetId id);
  ~Tileset(void);

  /**
   * Returns the background color of the tileset.
   * @return the background color
   */
  inline zsdx_color_t get_background_color(void) { return background_color; }

  void load(void);
  void unload(void);

  /**
   * Returns whether the tileset is loaded.
   * @return true if the tileset is loaded
   */
  inline bool is_loaded(void) { return tileset_image != NULL; }

  /**
   * Returns the tileset image.
   * @return the tileset image
   */
  inline SDL_Surface *get_image(void) { return tileset_image; }

  /**
   * Returns a tile from this tileset.
   * @param id id of the tile to get
   * @return the tile with this id
   */
  inline Tile *get_tile(int id) { return tiles[id - 1]; }
};

#endif
