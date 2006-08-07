#include "main.h"
#include "game.h"
#include "game_resource.h"

int main(int argc, char **argv) {
  zsdx_init();

//   Game *game = new Game();
//   game->launch_map(MAP_LINK_HOUSE);

  Map *map = game_resource.maps[MAP_LINK_HOUSE];
  for (int i = 0; i < 10; i++) {
    map->load();
    //    map->launch();
    map->unload();
  }

  zsdx_exit();
}
