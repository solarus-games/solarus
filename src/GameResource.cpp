#include "GameResource.h"
#include "Link.h"

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

  delete link;

  // TODO: delete the other resources
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
 * @param tileset_id id of the tileset to get
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
