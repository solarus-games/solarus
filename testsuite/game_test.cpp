#include "ZSDX.h"
#include "Game.h"
#include "GameResource.h"
#include <iostream>

int main(int argc, char **argv) {

  ZSDX::initialize();

//   Map *map = ZSDX::game_resource->get_map(MAP_LINKHOUSE);
//   Map *map = ZSDX::game_resource->get_map(MAP_LINKHOUSESECRETROOM);
  Map *map = ZSDX::game_resource->get_map(MAP_RUPEEHOUSE);
  for (int i = 0; i < 1; i++) {
    map->load();
    map->start();
    map->exit();
    map->unload();
  }

  ZSDX::exit();
  
  return 0;
}
