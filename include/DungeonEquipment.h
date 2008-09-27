#ifndef ZSDX_DUNGEON_EQUIPMENT_H
#define ZSDX_DUNGEON_EQUIPMENT_H

#include "Common.h"

/**
 * This class represents Link's equipment in a dungeon
 * (the map, the compass, the boss keys...).
 * Like class Equipment, it makes the interface between
 * the savegame storage and other engine classes.
 * This class can be used only when the player is in a dungeon.
 * Note that the small keys are not handled here because small keys
 * can exist outside a dungeon. They are handled in class Equipment.
 */
class DungeonEquipment {

 private:

  Savegame *savegame;
  int get_current_dungeon(void);

 public:

  DungeonEquipment(Savegame *savegame);
  ~DungeonEquipment(void);

  bool has_map(void);
  void add_map(void);

  bool has_compass(void);
  void add_compass(void);

  bool has_big_key(void);
  void add_big_key(void);

  bool has_boss_key(void);
  void add_boss_key(void);
};

#endif
