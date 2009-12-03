/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ZSDX_DESTRUCTIBLE_ITEM_H
#define ZSDX_DESTRUCTIBLE_ITEM_H

#include "Common.h"
#include "Detector.h"
#include "PickableItem.h"
#include "Ground.h"

/**
 * Represents an entity that the hero can destroy and that may contain a pickable item.
 * Some destructible items can be lifted and thrown (a pot, a stone, etc.),
 * some of them can be cut with the sword (for example some grass)
 * and others have both behaviors (for example a bush).
 * Some others can explode when they are lifted (a bomb or a bomb flower).
 * When the hero lifts an item, it is removed and replaced by an instance of CarriedItem
 * that is attached to the hero.
 */
class DestructibleItem: public Detector {

 public:

  /**
   * Subtypes of destructible items.
   */
  enum Subtype {

    POT               = 0,
    SKULL             = 1,
    BUSH              = 2,
    STONE_SMALL_WHITE = 3,
    STONE_SMALL_BLACK = 4,
    GRASS             = 5,
    BOMB_FLOWER       = 6,

  };

 private:

  /**
   * This structure defines the properties of a destructible item type.
   */
  struct Features {
    SpriteAnimationSetId animation_set_id; /**< animation set used for this type of destructible item */
    SoundId destruction_sound_id;          /**< sound played when the item is destroyed */
    bool can_be_lifted;                    /**< indicates that this item is an obstacle and can be lifted */
    bool can_be_cut;                       /**< indicates that this item can be cut with the sword */
    bool can_explode;                      /**< indicates that this item explodes after a delay */
    bool can_regenerate;                   /**< indicates that this item regenerates once lifted */
    int weight;                            /**< for liftable items: weight of the item (0: light,
					    * 1: iron glove required, 2: golden glove required) */
    int damage_on_enemies;                 /**< damage the item can cause to enemies (1: few, 2: normal, 3: a lot) */
    Ground special_ground;                 /**< for a non-obstacle item, indicates a special ground to display */
  };

  Subtype subtype;                         /**< the subtype of destructible item */
  PickableItem::Subtype pickable_item;     /**< the pickable item that appears when the item is lifted or cut */
  int pickable_item_savegame_variable;     /**< savegame variable of the pickable item (if any) */

  bool is_being_cut;                       /**< indicates that the item is being cut */
  uint32_t regeneration_date;                /**< date when the item starts regenerating */
  bool is_regenerating;                    /**< indicates that the item is currently regenerating */

  static const Features features[];

  void play_destroy_animation(void);
  bool is_disabled(void);

 public:

  // creation and destruction
  DestructibleItem(Layer layer, int x, int y, Subtype subtype,
		   PickableItem::Subtype pickable_item, int pickable_item_savegame_variable);
  ~DestructibleItem(void);
  static CreationFunction parse;

  EntityType get_type(void);
  bool is_displayed_in_y_order(void);

  const std::string& get_animation_set_id(void);
  Sound *get_destruction_sound(void);
  int get_damage_on_enemies(void);
  bool has_special_ground(void);
  Ground get_special_ground(void);
  bool can_explode(void);

  bool is_obstacle_for(MapEntity *other);
  void notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
  void notify_collision(MapEntity *other_entity, Sprite *other_sprite, Sprite *this_sprite);
  void action_key_pressed(void);
  void lift(void);

  void set_suspended(bool suspended);
  void update(void);
};

#endif

