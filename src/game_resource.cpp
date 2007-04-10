#include <SDL/SDL.h>
#include "game_resource.h"
#include "link.h"
#include "tilesets/house.h"
#include "maps/link_house.h"

void GameResource::create_resources(void) {
  // we cannot create the resources in the constructor
  // because some resources need other resources

  // link
  link = new Link();

  // tilesets
  tilesets[TILESET_HOUSE] = new TilesetHouse();

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
  maps[MAP_LINK_HOUSE] = new MapLinkHouse();
}

GameResource::~GameResource(void) {
  int i;

  delete link;
  delete tilesets[TILESET_HOUSE];
  delete maps[MAP_LINK_HOUSE];

  for (i = 0; i < NB_MUSICS; i++) {
    delete musics[i];
  }
}

Tileset *GameResource::get_tileset(int tileset_id) {
  return tilesets[tileset_id];
}

Map *GameResource::get_map(int map_id) {
  return maps[map_id];
}

Music *GameResource::get_music(int music_id) {
  return musics[music_id];
}
