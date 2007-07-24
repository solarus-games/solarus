/**
 * This module defines the class Tileset.
 */

#ifndef ZSDX_TILESET_H
#define ZSDX_TILESET_H

#include <SDL/SDL.h>

class Tile;

/**
 * Abstract class for a tileset.
 * A tileset is an image with a set of elements (tiles)
 * one can use to compose a map.
 * See the directory images/tilesets.
 */
class Tileset {

 protected:
  /**
   * Tiles of the tileset.
   * A tileset cannot have more than 1024 tiles.
   */
  Tile *tiles[1024];
  
  /**
   * Number of tiles in the tileset.
   */
  int nb_tiles;
  
  /**
   * Image from which the tiles are extracted.
   */
  SDL_Surface *tileset_image;
  
  /**
   * Constructor.
   */
  Tileset(void);

  /**
   * Loads the tileset image.
   * This function should be called by load().
   * @param file_name name of the tileset image file
   */
  void load_tileset_image(const char *file_name);

 public:

  /**
   * Destructor.
   */
  virtual ~Tileset(void);

  /**
   * Loads the tileset by creating all tiles.
   * This function is abstract so that each tileset can load its own tiles.
   * The tiles can be instances of SimpleTile, AnimatedTile and ExtensibleTiles.
   * Extensible tiles are handled
   */
  virtual void load(void) = 0;

  /**
   * Destroys the tiles and frees the memory used
   * by the tileset image.
   */
  void unload(void);

  /**
   * Returns whether the tileset is loaded.
   * @return true if the tileset is loaded
   */
  inline bool is_loaded(void) { return nb_tiles != 0; }

  /**
   * Returns the tileset image.
   * @return the tileset image
   */
  inline SDL_Surface *get_image(void) { return tileset_image; }

  /**
   * Returns a tile from this tileset.
   * @param index index of the tile to get
   * @return the tile at this index
   */
  inline Tile *get_tile(int index) { return tiles[index]; }
};

#endif
