#include <SDL/SDL_config_lib.h>
#include "Dungeon.h"

/**
 * Creates the specified dungeon.
 * @param dungeon_number a dungeon number between 1 and 20
 */
Dungeon::Dungeon(int dungeon_number) {
  this->dungeon_number = dungeon_number;
  load();
}

/**
 * Destructor.
 */
Dungeon::~Dungeon(void) {
  delete[] chests;
  delete[] minibosses;
}

/**
 * Loads the dungeon properties from file dungeons.zsd.
 */
void Dungeon::load(void) {

}
