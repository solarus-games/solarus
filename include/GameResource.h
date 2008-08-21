#ifndef ZSDX_GAME_RESOURCE_H
#define ZSDX_GAME_RESOURCE_H

#include "Common.h"
#include <map>

/* This class is a database of all game resources:
 * - the tileset list
 * - the map list
 * - the music list
 * - the sound list
 * - ...
 */
class GameResource {

 private:

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
   * The sounds.
   */
  std::map<SoundId, Sound*> sounds;

  /**
   * The sprite animations.
   */
  std::map<SpriteAnimationsId, SpriteAnimations*> sprite_animations;

  // items
  // ...

 public:
  GameResource(void);
  ~GameResource(void);

  Tileset* get_tileset(TilesetId id);
  Map* get_map(MapId id);
  Music* get_music(MusicId id);
  Sound* get_sound(SoundId id);
  SpriteAnimations* get_sprite_animations(SpriteAnimationsId id);
};

#endif
