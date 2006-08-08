#include "main.h"
#include "game.h"
#include "game_resource.h"

int main(int argc, char **argv) {
  zsdx_init();

  Map *map = game_resource->get_map(MAP_LINK_HOUSE);
  for (int i = 0; i < 1; i++) {
    map->load();
    map->launch();
    map->unload();
  }

  zsdx_exit();
}
