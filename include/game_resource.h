#ifndef ZSDX_GAME_RESOURCE_H
#define ZSDX_GAME_RESOURCE_H

#include "link.h"
#include "tileset.h"
#include "map.h"

// tilesets
static const int TILESET_HOUSE = 0;

// maps
static const int MAP_LINK_HOUSE = 0;

/* This class is a database of all game resources:
 * - the tileset list
 * - the map list
 * - the music list
 * - the sound list
 * - the colors
 * - ...
 */
class GameResource {

 private:
  /* Link
   */
  Link *link;

  /* The tilesets
   */
  Tileset *tilesets[1];

  /* The maps
   */
  Map *maps[1];

  // musics
  // sounds
  // items
  // saved data?
  // ...

 public:
  /* Constructor
   */
  GameResource(void);

  /* Destructor
   */
  ~GameResource(void);

  /* Create all resources
   */
  void create_resources(void);

  /* Return a color given its RGB values
   * TODO: find a better place for this function?
   */
  zsdx_color_t get_color(int r, int g, int b);

  /* Return link
   */
  inline Link *get_link() { return link; }

  /* Return a tileset given its id
   */
  Tileset *get_tileset(int tileset_id);

  /* Return a map given its id
   */
  Map *get_map(int map_id);

};

/* Global variable to get the game resources from anywhere
 */
extern GameResource *game_resource;

#endif
