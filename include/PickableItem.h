#ifndef ZSDX_PICKABLE_ITEM_H
#define ZSDX_PICKABLE_ITEM_H

#include "Common.h"
#include "EntityDetector.h"
#include "Layer.h"
#include "PickableItemType.h"
#include "MovementFallingHeight.h"

/**
 * A pickable item on the map (rupee, heart, bomb, fairy...).
 * This is a concrete class, used for any kind of pickable item.
 */
class PickableItem: public EntityDetector {

 protected:

  /**
   * The map.
   */
  Map *map;

  /**
   * Type of pickable item.
   */
  PickableItemType type;

  /**
   * Unique id of the item, for certain kinds of items only:
   * a key, a piece of heart...
   */
  int unique_id;

  /**
   * Sprite of the shadow (except for a fairy).
   */
  Sprite *shadow_sprite;

  /**
   * Indicates whether the item is falling when it appears (except for a fairy).
   */
  MovementFallingHeight falling_height;

  /**
   * Indicates whether the item will disappear after an amount of time.
   */
  bool will_disappear;

  /**
   * Coordinates of the shadow (which does not move when the item is falling).
   */
  int shadow_x;
  int shadow_y;

  /**
   * Date when the item appears.
   */
  Uint16 appear_date;
  
  /**
   * Date when the item starts blinking.
   */
  Uint16 blink_date;
  
  /**
   * Date when the item disappears.
   */
  Uint16 disappear_date;

  // creation and initialization
  PickableItem(Map *map, Layer layer, int x, int y, PickableItemType type);

  static PickableItemType choose_random_type(void);

  virtual void initialize_sprites(void);
  virtual void initialize_movement(void);

  // falling
  bool is_falling(void);

  // item
  virtual void give_item_to_player(void);

 public:

  virtual ~PickableItem(void);
  static PickableItem * create(Map *map, Layer layer, int x, int y, PickableItemType type,
			       int unique_id,
			       MovementFallingHeight falling_height, bool will_disappear);

  virtual void set_suspended(bool suspended);
  void entity_collision(MapEntity *entity_overlapping);

  virtual void update(void);
  virtual void display_on_map(Map *map);
};

#endif
