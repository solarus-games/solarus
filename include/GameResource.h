/**
 * This module handles the game resources.
 */

#ifndef ZSDX_GAME_RESOURCE_H
#define ZSDX_GAME_RESOURCE_H

#include "Common.h"
#include <map>

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

  /**
   * Link.
   */
  Link *link;

  /**
   * The tilesets.
   */
  std::map<TilesetId, Tileset*> tilesets;

  /**
   * The maps.
   */
  std::map<MapId, Map*> maps;

  /**
   * The musics.
   */
  std::map<MusicId, Music*> musics;

  /**
   * The sprites.
   */
  std::map<SpriteId, SpriteAnimations*> sprites;

  // sprites
  // sounds
  // items
  // saved data?
  // ...

 public:
  GameResource(void);
  ~GameResource(void);

  Link* get_link(void); // TODO: is link a resource?

  Tileset* get_tileset(TilesetId id);
  Map* get_map(MapId id);
  Music* get_music(MusicId id);
  SpriteAnimations* get_sprite(SpriteId id);
};

#endif
