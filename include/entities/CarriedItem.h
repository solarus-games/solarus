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
#ifndef ZSDX_CARRIED_ITEM_H
#define ZSDX_CARRIED_ITEM_H

#include "Common.h"
#include "MapEntity.h"

/**
 * Represents an item (pot, bush...) carried by the hero. He can throw it.
 * The item is attached to the hero, not to the map.
 */
class CarriedItem: public MapEntity {

 private:

  // game data
  Map *map;                 /**< the map */
  Hero *hero;               /**< the hero, who is carrying or throwing this item */

  // state
  bool is_lifting;          /**< indicates that the hero is lifting this item */
  bool is_throwing;         /**< indicates that the item is being thrown */
  bool is_breaking;         /**< indicates that the item is breaking */
  Sound *destruction_sound; /**< the sound played when the item breaks */
  int damage_on_enemies;    /**< damage for an enemy that receives the item */

  // throwing the item
  Sprite *shadow_sprite;    /**< sprite of the shadow when the item is being thrown */
  int throwing_direction;   /**< direction where the item is thrown (0 to 3) */
  Uint32 next_down_date;    /**< when the item is thrown, date when it move one pixel downwards next time */
  int item_height;          /**< current height where the item is displayed above its shadow */
  int y_increment;          /**< next y change for item_height */

 public:

  CarriedItem(Hero *hero, DestructibleItem *destructible_item);
  ~CarriedItem(void);

  EntityType get_type(void);

  int get_damage_on_enemies(void);

  void set_animation_stopped(void);
  void set_animation_walking(void);
  void throw_item(Map *map, int direction);
  bool is_being_thrown(void);
  void break_item(void);
  bool is_broken(void);

  void set_suspended(bool suspended);
  void update(void);
  void display_on_map(void);

  bool is_teletransporter_obstacle(Teletransporter *teletransporter);
  bool is_water_obstacle(void);
  bool is_hole_obstacle(void);
  void collision_with_enemy(Enemy *enemy);
};

#endif
