#include "ResourceManager.h"
#include "Map.h"
#include "entities/Tileset.h"
#include "Music.h"
#include "Sound.h"
#include "SpriteAnimationSet.h"
#include "FileTools.h"

/**
 * Only instance.
 */
ResourceManager * ResourceManager::instance;

/**
 * Constructor.
 */
ResourceManager::ResourceManager(void) {

}

/**
 * Destructor.
 * Destroys all the resources stored into memory.
 */
ResourceManager::~ResourceManager(void) {

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
    map<SpriteAnimationSetId, SpriteAnimationSet*>::const_iterator it;
    
    for (it = sprite_animations.begin(); it != sprite_animations.end(); it++) {
      delete it->second;
    }
    sprite_animations.clear();
  }
}

/**
 * Initializes the resource manager.
 */
void ResourceManager::initialize(void) {
  instance = new ResourceManager();
}

/**
 * Quits the resource manager.
 */
void ResourceManager::quit(void) {
  delete instance;
}

/**
 * Loads an image.
 * Note: unlike the other resource types, the resource manager
 * does not store the images previously loaded.
 * Consequently, if you call this function several times
 * with the same image file name, the image will be reloaded
 * every time. You also have to free the image yourself.
 * @param id name of the image file to get (relative to the images directory)
 * @return the image loaded
 */
SDL_Surface * ResourceManager::load_image(ImageId id) {
  
  return FileTools::open_image(id);
}

/**
 * Returns a tileset.
 * @param id id of the tileset to get
 * @return the tileset
 */
Tileset * ResourceManager::get_tileset(TilesetId id) {
  
  if (instance->tilesets[id] == NULL) {
    instance->tilesets[id] = new Tileset(id);
  }
  
  return instance->tilesets[id];
}

/**
 * Returns a map.
 * @param id id of the map to get
 * @return the map
 */
Map * ResourceManager::get_map(MapId id) {
  
  if (instance->maps[id] == NULL) {
    instance->maps[id] = new Map(id);
  }
  
  return instance->maps[id];
}

/**
 * Returns a music.
 * @param id id of the music to get
 * @return the music
 */
Music * ResourceManager::get_music(MusicId id) {
  
  if (instance->musics[id] == NULL) {
    instance->musics[id] = new Music(id);
  }
  
  return instance->musics[id];
}

/**
 * Returns a sound.
 * @param id id of the sound to get
 * @return the sound
 */
Sound * ResourceManager::get_sound(SoundId id) {
  
  if (instance->sounds[id] == NULL) {
    instance->sounds[id] = new Sound(id);
  }
  
  return instance->sounds[id];
}

/**
 * Returns the animation set of a sprite.
 * @param id id of the sprite animation set to get
 * @return the sprite animation set required
 */
SpriteAnimationSet * ResourceManager::get_sprite_animation_set(SpriteAnimationSetId id) {
  
  if (instance->sprite_animations[id] == NULL) {
    instance->sprite_animations[id] = new SpriteAnimationSet(id);
  }
  
  return instance->sprite_animations[id];
}
