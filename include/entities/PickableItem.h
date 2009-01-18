#ifndef ZSDX_PICKABLE_ITEM_H
#define ZSDX_PICKABLE_ITEM_H

#include "Common.h"
#include "entities/Detector.h"
#include "movements/MovementFalling.h"

/**
 * A pickable item on the map (rupee, heart, bomb, fairy...).
 * This is a concrete class, used for any kind of pickable item.
 */
class PickableItem: public Detector {

 public:

  /**
   * Types of pickable items.
   */
  enum ItemType {

    // special values
    RANDOM          = -1,   /**< special value to indicate to choose another value randomly (including NONE) */
    NONE            = 0,    /**< special value to indicate that there is no pickable item */

    // items not saved
    RUPEE_1         = 1,
    RUPEE_5         = 2,
    RUPEE_20        = 3,
    HEART           = 4,
    SMALL_MAGIC     = 5,
    BIG_MAGIC       = 6,
    FAIRY           = 7,
    BOMB_1          = 8,
    BOMB_5          = 9,
    BOMB_10         = 10,
    ARROW_1         = 11,
    ARROW_5         = 12,
    ARROW_10        = 13,

    // items saved
    SMALL_KEY       = 14,
    BIG_KEY         = 15,
    BOSS_KEY        = 16,
    PIECE_OF_HEART  = 17,
    HEART_CONTAINER = 18,
  };

 protected:

  /**
   * This structure defines the properties of a pickable item type.
   */
  struct Properties {
    SpriteAnimationSetId animation_set_id; /**< animation set used for this type of pickable item */
    string animation_name;                 /**< name of the animation */
    bool big_shadow;                       /**< true if the pickable item has a big shadow, false for a small shadow */
    SoundId sound;                         /**< the sound played when the player gets the item */
  };
  
  static const Properties properties[];

  Map *map;

  ItemType type;          // type of pickable item
  int savegame_variable;  // savegame index of the possession state of this item,
		          // for certain kinds of items only: a key, a piece of heart...

  Sprite *shadow_sprite;                // sprite of the shadow (except for a fairy).
  MovementFalling::FallingHeight falling_height; // indicates whether the item is falling when it appears (except for a fairy)
  bool will_disappear;                  // indicates whether the item will disappear after an amount of time
                                        // (only possible for items not saved)

  // current state
  int shadow_x;           // coordinates of the shadow (which does not move when the item does by falling)
  int shadow_y;

  Uint32 appear_date;     // date when the item is created
  Uint32 allow_pick_date; // date when the item can be picked
  bool can_be_picked;     // indicates that the item can be picked now (i.e. allow_picked_date is past)
  Uint32 blink_date;      // date when the item starts blinking
  Uint32 disappear_date;  // date when the item disappears

  // creation and initialization
  PickableItem(Map *map, Layer layer, int x, int y, ItemType type, int savegame_variable);

  static ItemType choose_random_type(void);
  virtual void initialize_sprites(void);
  virtual void initialize_movement(void);

  // falling
  bool is_falling(void);

  // item
  virtual void give_item_to_player(void);
  void set_blinking(bool blinking);

 public:

  // creation and destruction
  static PickableItem * create(Map *map, Layer layer, int x, int y, ItemType type, int savegame_variable,
			       MovementFalling::FallingHeight falling_height, bool will_disappear);

  virtual ~PickableItem(void);

  // item state
  virtual void set_suspended(bool suspended);
  void collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
  virtual void update(void);
  virtual void display_on_map(void);
};

#endif
