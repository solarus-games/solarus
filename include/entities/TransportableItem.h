#ifndef ZSDX_TRANSPORTABLE_ITEM_H
#define ZSDX_TRANSPORTABLE_ITEM_H

#include "Common.h"
#include "EntityDetector.h"
#include "PickableItem.h"

/**
 * Represents an entity that Link can lift
 * (a pot, a bush, a stone, etc.).
 * When Link lifts the item, it is destroyed
 * and replaced by an instance of CarriedItem that is
 * attached to Link.
 */
class TransportableItem: public EntityDetector {

 public:

  /**
   * Types of transportable items.
   */
  enum ItemType {

    POT               = 0,
    SKULL             = 1,
    BUSH              = 2,
    STONE_SMALL_WHITE = 3,
    STONE_SMALL_BLACK = 4,

    // not implemented
    STONE_BIG_WHITE   = 5,
    STONE_BIG_BLACK   = 6,

    //  SIGN              = 7,
};

 private:

  Map *map;      /**< the map */

  ItemType type;                           /**< the type of transportable item */
  PickableItem::ItemType pickable_item;    /**< the pickable item that appears when the item is lifted */
  int pickable_item_savegame_variable;     /**< savegame index of the pickable item (if any) */

  /**
   * This structure defines the properties of a transportable item type.
   */
  struct ItemProperties {
    SpriteAnimationSetId animation_set_id; /**< animation set used for this type of transportable item */
    SoundId breaking_sound_id;             /**< sound played when the item is broken */
    int weight;                            /**< weight of the item (0: light, 1: iron glove required, 2: golden glove required */
  };

  static const ItemProperties properties[];

 public:
  
  // creation and destruction
  TransportableItem(Map *map, Layer layer, int x, int y, ItemType type,
		    PickableItem::ItemType pickable_item, int pickable_item_savegame_variable);
  ~TransportableItem(void);

  string get_animation_set_id(void);
  Sound *get_breaking_sound(void);

  void entity_collision(MapEntity *entity_overlapping);
  void action_key_pressed(void);
  void lift(void);
};


#endif
