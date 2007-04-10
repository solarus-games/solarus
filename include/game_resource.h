#ifndef ZSDX_GAME_RESOURCE_H
#define ZSDX_GAME_RESOURCE_H

#include "link.h"
#include "tileset.h"
#include "map.h"

// tilesets
static const int TILESET_HOUSE = 0;

// maps
static const int MAP_LINK_HOUSE = 0;

// musics
static const int NB_MUSICS = 13;
static const int MUSIC_VILLAGE = 0;
static const int MUSIC_OVERWORLD = 1;
static const int MUSIC_BOSS = 2;
static const int MUSIC_CASTLE = 3;
static const int MUSIC_CREDITS = 4;
static const int MUSIC_TITLE_SCREEN = 5;
static const int MUSIC_FANFARE = 6;
static const int MUSIC_GAME_OVER = 7;
static const int MUSIC_RABBIT = 8;
static const int MUSIC_MENU = 9;
static const int MUSIC_MINI_GAME = 10;
static const int MUSIC_DARK_WORLD = 11;
static const int MUSIC_SOLDIERS = 12;

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

  /* The musics
   */
  Music *musics[NB_MUSICS];

  // musics
  // sounds
  // items
  // saved data?
  // ...

 public:
  /* Constructor
   */
  inline GameResource(void) { }

  /* Destructor
   */
  ~GameResource(void);

  /* Create all resources
   */
  void create_resources(void);

  /* Return link
   */
  inline Link *get_link() { return link; }

  /* Return a tileset given its id
   */
  Tileset *get_tileset(int tileset_id);

  /* Return a map given its id
   */
  Map *get_map(int map_id);

  /**
   * Returns a music given its id
   */
  Music *get_music(int music_id);
};

#endif
