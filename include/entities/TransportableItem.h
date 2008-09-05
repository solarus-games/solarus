#ifndef ZSDX_TRANSPORTABLE_ITEM_H
#define ZSDX_TRANSPORTABLE_ITEM_H

#include "Common.h"
#include "EntityDetector.h"
#include "TransportableItemType.h"
#include "PickableItemType.h"

/**
 * Represents an entity that Link can lift
 * (a pot, a bush, a stone, etc.).
 * When Link lifts the item, it is destroyed
 * and replaced by an instance of CarriedItem that is
 * attached to Link.
 */
class TransportableItem: public EntityDetector {

 private:

  /**
   * The map.
   */
  Map *map;
  
  /**
   * The type of transportable item.
   */
  TransportableItemType type;

  /**
   * The type of pickable item that appears when the item is lifted.
   */
  PickableItemType pickable_item;

  /**
   * This structure defines the properties of a transportable item type.
   */
  struct ItemProperties {
    SpriteAnimationsId sprite_animations_id; /**< animation set used for this type of transportable item */
    SoundId breaking_sound_id;               /**< sound played when the item is broken */
    // TODO weight
  };

  static const ItemProperties properties[];

 public:
  
  // creation and destruction
  TransportableItem(Map *map, Layer layer, int x, int y,
		    TransportableItemType type,
		    PickableItemType pickable_item, int unique_id);
  ~TransportableItem(void);

  string get_sprite_animations_id(void);
  Sound *get_breaking_sound(void);

  void entity_collision(MapEntity *entity_overlapping);
  void action_key_pressed(void);
  void lift(void);
};


#endif
