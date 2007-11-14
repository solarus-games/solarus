/**
 * Some definition of types.
 * This header is included by Common.h and every file should include Common.h.
 */

#ifndef ZSDX_TYPES_H
#define ZSDX_TYPES_H

/**
 * Obstacle property for the tiles or the active objects.
 */
enum Obstacle {
  OBSTACLE_NONE,         // the entity is not an obstacle
  OBSTACLE,              // the entity is entirely an obstacle
  OBSTACLE_TOP_RIGHT,    // the upper-right half of the entity is an obstacle
  OBSTACLE_TOP_LEFT,     // the upper-left half of the entity is an obstacle
  OBSTACLE_BOTTOM_LEFT,  // the lower-left half of the entity is an obstacle
  OBSTACLE_BOTTOM_RIGHT, // the lower-right half of the entity is an obstacle
};

/**
 * Layer of a tile or an active object.
 */
enum Layer {
  LAYER_LOW,             // the entity is always below Link (floor, walls, chests,
                         // enemies and 99% of the tiles and active objects)
  LAYER_INTERMEDIATE,    // Link can be below or above the entity (platforms or objects on a plaform)
  LAYER_HIGH,            // the entity is always above Link (trees, top of doors...)
  LAYER_NB               // number of layers
};

/**
 * Constants to identify each music.
 */
enum MusicID {
  MUSIC_NONE      = -2, // special value to stop the music
  MUSIC_UNCHANGED = -1, // special value to keep the current music
  MUSIC_VILLAGE,
  MUSIC_OVERWORLD,
  MUSIC_BOSS,
  MUSIC_CASTLE,
  MUSIC_CREDITS,
  MUSIC_TITLE_SCREEN,
  MUSIC_FANFARE,
  MUSIC_GAME_OVER,
  MUSIC_RABBIT,
  MUSIC_MENU,
  MUSIC_MINI_GAME,
  MUSIC_DARK_WORLD,
  MUSIC_SOLDIERS,
  NB_MUSICS,            // special value to get the number of musics
};

/**
 * Constants to identify each tileset.
 */
enum TilesetID {
#include "TilesetList.inc"
  NB_TILESETS
};

/**
 * Constants to identify each map.
 */
enum MapID {
#include "MapList.inc"
  NB_MAPS
};

/**
 * Declaration of some classes to break cycling dependencies between the header files.
 */

class Map;
class TileOnMap;
class MovingWithCollision;

#endif
