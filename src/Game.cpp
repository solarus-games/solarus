/**
 * This module defines the class Game.
 */

#include <SDL/SDL.h>
#include "Game.h"
#include "ZSDX.h"

/**
 * Launches a map.
 * @param map_id id of the map to get
 */
void Game::launch_map(MapID map_id) {
  Map *map = ZSDX::game_resource->get_map(map_id);

  map->load();
  map->start();
  map->exit();
  map->unload();
}
