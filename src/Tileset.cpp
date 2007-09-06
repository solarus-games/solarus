/**
 * Abstract class for a tileset.
 * A tileset is an image with a set of elements (tiles)
 * one can use to compose a map.
 * See the directory images/tilesets.
 */

using namespace std;
#include <iostream>
#include <cstdlib>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "Tileset.h"
#include "Tile.h"

/**
 * Constructor.
 * Creates an empty tileset.
 */
Tileset::Tileset(void):
nb_tiles(0) {
  
}

/**
 * Destructor.
 */
Tileset::~Tileset(void) {
  if (is_loaded()) {
    unload(); // destroy the tiles
  }
}

/**
 * Creates a new tile in the tileset.
 * This function is called by load().
 * @param id id of this tile (1 to 1024)
 * @param tile the tile to add
 */
void Tileset::create_tile(int id, Tile *tile) {
  tiles[id - 1] = tile;
}

/**
 * Loads the tileset image.
 * This function should be called by load().
 * @param file_name name of the tileset image file
 */
void Tileset::load_tileset_image(const char *file_name) {
  tileset_image = IMG_Load(file_name);
  
  if (tileset_image == NULL) {
    cerr << "Unable to load file '" << file_name << "'" << endl;
  }
}

/**
 * Destroys the tiles and frees the memory used
 * by the tileset image.
 */
void Tileset::unload(void) {
  int i;

  for (i = 0; i < nb_tiles; i++) {
    delete tiles[i];
  }

  SDL_FreeSurface(tileset_image);
  nb_tiles = 0;
}
