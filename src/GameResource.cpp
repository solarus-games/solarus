#include <SDL/SDL.h>
#include "GameResource.h"
#include "Link.h"
#include "tilesets/TilesetHouse.h"
#include "maps/MapLinkHouse.h"
#include "maps/MapLinkHouseSecretRoom.h"
#include "maps/MapRupeeHouse.h"

/**
 * Creates all resources.
 */
void GameResource::create_resources(void) {
  // we cannot create the resources in the constructor
  // because some resources need other resources

  // Link
  link = new Link();

  // tilesets
#include "TilesetsCreation.inc.cpp"
  
  // musics
  musics[MUSIC_VILLAGE] = new Music("village.it");
  musics[MUSIC_OVERWORLD] = new Music("overworld.it");
  musics[MUSIC_BOSS] = new Music("boss.it");
  musics[MUSIC_CASTLE] = new Music("castle.it");
  musics[MUSIC_CREDITS] = new Music("credits.it");
  musics[MUSIC_TITLE_SCREEN] = new Music("title_screen.it");
  musics[MUSIC_FANFARE] = new Music("fanfare.it");
  musics[MUSIC_GAME_OVER] = new Music("game_over.it");
  musics[MUSIC_RABBIT] = new Music("rabbit.it");
  musics[MUSIC_MENU] = new Music("menu.it");
  musics[MUSIC_MINI_GAME] = new Music("mini_game.it");
  musics[MUSIC_DARK_WORLD] = new Music("dark_world.it");
  musics[MUSIC_SOLDIERS] = new Music("soldiers.it");

  // maps
#include "MapsCreation.inc.cpp"
}

/**
 * Destructor.
 * Destroys all the game resources.
 */
GameResource::~GameResource(void) {
  int i;

  delete link;

  for (i = 0; i < NB_TILESETS; i++) {
    delete tilesets[i];
  }
  for (i = 0; i < NB_MAPS; i++) {
    delete maps[i];
  }
  for (i = 0; i < NB_MUSICS; i++) {
    delete musics[i];
  }
}

/**
 * Returns a tileset.
 * @param tileset_id id of the tileset to get
 */
Tileset *GameResource::get_tileset(TilesetID tileset_id) {
  return tilesets[tileset_id];
}

/**
 * Returns a map.
 * @param map_id id of the map to get
 */
Map *GameResource::get_map(MapID map_id) {
  return maps[map_id];
}

/**
 * Returns a music.
 * @param music_id id of the music to get
 */
Music *GameResource::get_music(MusicID music_id) {
  return musics[music_id];
}
