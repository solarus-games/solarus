/**
 * This module handles the game resources.
 */

#ifndef ZSDX_GAME_RESOURCE_H
#define ZSDX_GAME_RESOURCE_H

#include "Common.h"
#include "Link.h"
#include "Tileset.h"
#include "Map.h"
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

  // musics
  // sounds
  // items
  // saved data?
  // ...

 public:

  /**
   * Constructor.
   */
  inline GameResource(void) { }

  /**
   * Destructor.
   * Destroys all the game resources.
   */
  ~GameResource(void);

  /**
   * Returns Link.
   * @return Link
   */
  inline Link *get_link() { return link; }

  /**
   * Returns a tileset.
   * @param id id of the tileset to get
   */
  Tileset *get_tileset(TilesetId id);

  /**
   * Returns a map.
   * @param id id of the map to get
   */
  Map *get_map(MapId id);

  /**
   * Returns a music.
   * @param id id of the music to get
   */
  Music *get_music(MusicId id);
};

#endif
