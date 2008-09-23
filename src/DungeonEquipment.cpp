#include "DungeonEquipment.h"
#include "ZSDX.h"
#include "Game.h"
#include "Map.h"
#include "Savegame.h"

/**
 * Constructor.
 * @param savegame the savegame
 */
DungeonEquipment::DungeonEquipment(Savegame *savegame):
  savegame(savegame) {

}

/**
 * Destructor.
 */
DungeonEquipment::~DungeonEquipment(void) {

}

/**
 * Returns the current dungeon number.
 * The dungeon number returned is between 1 and 20.
 * The programs stops with an error message if the player is not in a dungeon.
 * @return the number of the current dungeon between 1 and 20
 */
int DungeonEquipment::get_current_dungeon(void) {

  Map *current_map = zsdx->game->get_current_map();

  if (!current_map->is_in_dungeon()) {
    DIE("The player is not in a dungeon");
  }

  return current_map->get_world_number();
}

/**
 * Returns whether the player has found the map in the current dungeon.
 * @return true if the player has found the map in the current dungeon
 */
bool DungeonEquipment::has_map(void) {

  int dungeon = get_current_dungeon();
  int index = Savegame::FIRST_DUNGEON_MAP + 10 * (dungeon - 1);
  return savegame->get_integer(index) != 0;
}

/**
 * Give the current dungeon's map to the player.
 */
void DungeonEquipment::add_map(void) {

  if (has_map()) {
    DIE("The player already has the map");
  }

  int dungeon = get_current_dungeon();
  int index = Savegame::FIRST_DUNGEON_MAP + 10 * (dungeon - 1);
  savegame->set_integer(index, 1);
}

/**
 * Returns whether the player has found the compass in the current dungeon.
 * @return true if the player has found the compass in the current dungeon
 */
bool DungeonEquipment::has_compass(void) {

  int dungeon = get_current_dungeon();
  int index = Savegame::FIRST_DUNGEON_COMPASS + 10 * (dungeon - 1);
  return savegame->get_integer(index) != 0;
}

/**
 * Give the current dungeon's compass to the player.
 */
void DungeonEquipment::add_compass(void) {

  if (has_compass()) {
    DIE("The player already has the compass");
  }

  int dungeon = get_current_dungeon();
  int index = Savegame::FIRST_DUNGEON_COMPASS + 10 * (dungeon - 1);
  savegame->set_integer(index, 1);
}

/**
 * Returns whether the player has found the big key in the current dungeon.
 * @return true if the player has found the big key in the current dungeon
 */
bool DungeonEquipment::has_big_key(void) {

  int dungeon = get_current_dungeon();
  int index = Savegame::FIRST_DUNGEON_BIG_KEY + 10 * (dungeon - 1);
  return savegame->get_integer(index) != 0;
}

/**
 * Give the current dungeon's big key to the player.
 */
void DungeonEquipment::add_big_key(void) {

  if (has_big_key()) {
    DIE("The player already has the big key");
  }

  int dungeon = get_current_dungeon();
  int index = Savegame::FIRST_DUNGEON_BIG_KEY + 10 * (dungeon - 1);
  savegame->set_integer(index, 1);
}

/**
 * Returns whether the player has found the boss key in the current dungeon.
 * @return true if the player has found the boss key in the current dungeon
 */
bool DungeonEquipment::has_boss_key(void) {

  int dungeon = get_current_dungeon();
  int index = Savegame::FIRST_DUNGEON_BOSS_KEY + 10 * (dungeon - 1);
  return savegame->get_integer(index) != 0;
}

/**
 * Give the current dungeon's boss key to the player.
 */
void DungeonEquipment::add_boss_key(void) {

  if (has_boss_key()) {
    DIE("The player already has the boss key");
  }

  int dungeon = get_current_dungeon();
  int index = Savegame::FIRST_DUNGEON_BOSS_KEY + 10 * (dungeon - 1);
  savegame->set_integer(index, 1);
}

/**
 * Returns whether the player has got at least one small key in the current dungeon.
 * @return true if the player has got at least one small key
 */
bool DungeonEquipment::has_small_key(void) {

  return get_small_keys() > 0;
}

/**
 * Returns the current number of small keys in the dungeon.
 * @return the current number of small keys in the dungeon
 */
int DungeonEquipment::get_small_keys(void) {

  int dungeon = get_current_dungeon();
  int index = Savegame::FIRST_DUNGEON_SMALL_KEYS + 10 * (dungeon - 1);
  return savegame->get_integer(index);
}

/**
 * Adds a small key to the player in the current dungeon.
 */
void DungeonEquipment::add_small_key(void) {

  int dungeon = get_current_dungeon();
  int index = Savegame::FIRST_DUNGEON_SMALL_KEYS + 10 * (dungeon - 1);
  savegame->set_integer(index, get_small_keys() + 1);
}

/**
 * Removes a small key from the player in the current dungeon.
 */
void DungeonEquipment::remove_small_key(void) {

  if (!has_small_key()) {
    DIE("The player has no small keys");
  }

  int dungeon = get_current_dungeon();
  int index = Savegame::FIRST_DUNGEON_SMALL_KEYS + 10 * (dungeon - 1);
  savegame->set_integer(index, get_small_keys() - 1);
}
