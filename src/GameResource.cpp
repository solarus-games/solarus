#include "GameResource.h"
#include "Map.h"
#include "entities/Link.h"
#include "entities/Tileset.h"
#include "Music.h"
#include "Sound.h"
#include "SpriteAnimations.h"

/**
 * Constructor.
 */
GameResource::GameResource(void) {

  link = NULL;
}

/**
 * Destructor.
 * Destroys all the game resources.
 */
GameResource::~GameResource(void) {

  // Link
  if (link != NULL) {
    delete link;
  }

  // tilesets
  {
    map<TilesetId, Tileset*>::const_iterator it;
    
    for (it = tilesets.begin(); it != tilesets.end(); it++) {
      delete it->second;
    }
    tilesets.clear();
  }

  // maps
  {
    map<MapId, Map*>::const_iterator it;
    
    for (it = maps.begin(); it != maps.end(); it++) {
      delete it->second;
    }
    maps.clear();
  }

  // musics
  {
    map<MusicId, Music*>::const_iterator it;
    
    for (it = musics.begin(); it != musics.end(); it++) {
      delete it->second;
    }
    musics.clear();
  }

  // sounds
  {
    map<SoundId, Sound*>::const_iterator it;
    
    for (it = sounds.begin(); it != sounds.end(); it++) {
      delete it->second;
    }
    sounds.clear();
  }

  // sprite animations
  {
    map<SpriteAnimationsId, SpriteAnimations*>::const_iterator it;
    
    for (it = sprite_animations.begin(); it != sprite_animations.end(); it++) {
      delete it->second;
    }
    sprite_animations.clear();
  }
}

/**
 * Returns Link.
 * @return link
 */
Link* GameResource::get_link(void) {
  
  if (link == NULL) {
    link = new Link();
  }
  
  return link;
}

/**
 * Returns a tileset.
 * @param id id of the tileset to get
 * @return the tileset
 */
Tileset* GameResource::get_tileset(TilesetId id) {
  
  if (tilesets[id] == NULL) {
    tilesets[id] = new Tileset(id);
  }
  
  return tilesets[id];
}

/**
 * Returns a map.
 * @param id id of the map to get
 * @return the map
 */
Map* GameResource::get_map(MapId id) {
  
  if (maps[id] == NULL) {
    maps[id] = new Map(id);
  }
  
  return maps[id];
}

/**
 * Returns a music.
 * @param id id of the music to get
 * @return the music
 */
Music* GameResource::get_music(MusicId id) {
  
  if (musics[id] == NULL) {
    musics[id] = new Music(id);
  }
  
  return musics[id];
}

/**
 * Returns a sound.
 * @param id id of the sound to get
 * @return the sound
 */
Sound* GameResource::get_sound(SoundId id) {
  
  if (sounds[id] == NULL) {
    sounds[id] = new Sound(id);
  }
  
  return sounds[id];
}

/**
 * Returns the animations of a sprite.
 * @param id id of the sprite animations to get
 * @return the sprite animations required
 */
SpriteAnimations* GameResource::get_sprite_animations(SpriteAnimationsId id) {
  
  if (sprite_animations[id] == NULL) {
    sprite_animations[id] = new SpriteAnimations(id);
  }
  
  return sprite_animations[id];
}
