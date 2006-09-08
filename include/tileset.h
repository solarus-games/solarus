#ifndef ZSDX_TILESET_H
#define ZSDX_TILESET_H

#include <SDL/SDL.h>
#include "extensible_tile.h"
#include "dynamic_array.h"

/* Abstract class for a tileset.
 * A tileset is an image with a set of graphics to compose a map.
 * See the directory images/tilesets.
 */
class Tileset {

 private:
  /* Extensible tiles created by get_new_extensible_tiles()
   * This field is just here to keep references to these tiles
   * in order to delete them in unload().
   */
  DynamicArray<ExtensibleTile*> *extensible_tiles;
  
 protected:
  /* Tiles of the tileset
   */
  TileData *tiles[1024];
  
  /* Number of tiles in the tileset
   */
  int tile_number;
  
  /* Image from which the tiles are extracted
   */
  SDL_Surface *tileset_image;
  
 public:
  /* Constructor
   */
  Tileset(void);

  /* Destructor
   */
  virtual ~Tileset(void);

  /* Load the given tileset image
   */
  void load_tileset_image(const char *file);

  /* Load the tileset by creating all tiles.
   * This function is abstract so that each tileset can load
   * its own tiles.
   */
  virtual void load(void) = 0;

  /* Unload the tileset to free some memory
   * Destroy all tiles
   */
  void unload(void);

  /* Return whether the tileset is loaded
   */
  inline bool is_loaded(void) { return tile_number != 0; }

  /* Return the tileset image
   */
  inline SDL_Surface *get_image(void) { return tileset_image; }

  /* Return a tile
   */
  inline TileData *get_tile(int index) { return tiles[index]; }

  /* Create a new extensible tile given the index of an extensible tile
   * and how many times you want its pattern to be repeated 
   */
  TileData *get_new_extensible_tile(int index, int repeat_x, int repeat_y);
};

#endif
