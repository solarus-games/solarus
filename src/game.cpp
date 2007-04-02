#include <SDL/SDL.h>
#include "game.h"
#include "zsdx.h"

Game::Game(void) {

}

Game::~Game(void) {

}

void Game::launch_map(int map_id) {
  Map *map = game_resource->get_map(map_id);
  map->load();
  map->start();
  map->exit();
  map->unload();
}
