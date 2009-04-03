#ifndef ZSDX_DESTRUCTIBLE_ITEM_H
#define ZSDX_DESTRUCTIBLE_ITEM_H

#include "Common.h"
#include "Detector.h"
#include "PickableItem.h"

/**
 * Represents an entity that the hero can destroy and that contains a pickable item.
 * Some destructible items can be lifted and thrown (a pot, a stone, etc.),
 * some of them can be cut with the sword (for example some grass)
 * and others have both behaviors (for example a bush).
 * When the hero lifts an item, it is destroyed and replaced by an instance of CarriedItem
 * that is attached to the hero.
 */
class DestructibleItem: public Detector {

 public:

  /**
   * Types of destructible items.
   */
  enum ItemType {

    POT               = 0,
    SKULL             = 1,
    BUSH              = 2,
    STONE_SMALL_WHITE = 3,
    STONE_SMALL_BLACK = 4,
    GRASS             = 5,

    // not implemented
    /*
    STONE_BIG_WHITE   = ,
    STONE_BIG_BLACK   = ,
    SIGN              = ,
    */
  };

 private:

  ItemType type;                           /**< the type of destructible item */
  PickableItem::ItemType pickable_item;    /**< the pickable item that appears when the item is lifted or cut */
  int pickable_item_savegame_variable;     /**< savegame variable of the pickable item (if any) */

  bool is_being_cut;                       /**< indicates that the item is being cut */

  /**
   * This structure defines the properties of a destructible item type.
   */
  struct ItemProperties {
    SpriteAnimationSetId animation_set_id; /**< animation set used for this type of destructible item */
    SoundId destruction_sound_id;          /**< sound played when the item is destroyed */
    bool can_be_lifted;                    /**< indicates that this item can be lifted */
    bool can_be_cut;                       /**< indicates that this item can be cut with the sword */
    int weight;                            /**< for liftable items: weight of the item (0: light,
					    * 1: iron glove required, 2: golden glove required */
    int damage_on_enemies;                 /**< damage the item can cause to enemies (1: few, 2: normal, 3: a lot) */
  };

  static const ItemProperties properties[];

 public:

  // creation and destruction
  DestructibleItem(Layer layer, int x, int y, ItemType type,
		   PickableItem::ItemType pickable_item, int pickable_item_savegame_variable);
  ~DestructibleItem(void);

  EntityType get_type(void);

  std::string get_animation_set_id(void);
  Sound *get_destruction_sound(void);
  int get_damage_on_enemies(void);

  bool is_obstacle_for(MapEntity *other);
  void collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
  void collision(MapEntity *entity, Sprite *sprite_overlapping);
  void action_key_pressed(void);
  void lift(void);

  void update(void);
};

#endif
