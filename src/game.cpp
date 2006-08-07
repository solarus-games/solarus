#include <SDL/SDL.h>
#include "game.h"
#include "game_resource.h"
#include "map.h"

Game::Game(void) {

}

Game::~Game(void) {

}

void Game::launch_map(int map_id) {
  // create_game_resource() must have been called
  Map *map = game_resource.maps[map_id];
  map->load();
  map->launch();
  map->unload();
}
