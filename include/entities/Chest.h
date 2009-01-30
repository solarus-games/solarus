#ifndef ZSDX_CHEST_H
#define ZSDX_CHEST_H

#include "Common.h"
#include "Detector.h"

/**
 * Represents a chest on a map.
 * It can be a normal chest or a big chest.
 */
class Chest: public Detector {

 private:

  bool big_chest;       /**< true for a big chest, false for normal chests */
  Treasure *treasure;   /**< the treasure placed in this chest, or NULL if the chest contains nothing */
  bool open;            /**< true if the chest is open (but the treasure may not have
		         * been given yet because there is a delay of 500 ms) */
  bool treasure_given;  /**< true if the chest is open and the treasure has been given to the player */
  Uint32 treasure_date; /**< date when the treasure will be given to the player */

  void initialize_sprite(void);

 public:

  Chest(string name, Layer layer, int x, int y, bool big_chest, Treasure *treasure);
  ~Chest(void);

  EntityType get_type(void);

  bool is_open(void);

  bool is_obstacle_for(MapEntity *other);
  void collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
  void update(void);
  void action_key_pressed(void);
  void set_suspended(bool suspended);
};


#endif
