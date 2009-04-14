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

  Map *map;
  Hero *hero;
  bool is_lifting;
  bool is_throwing;
  bool is_breaking;
  Sound *destruction_sound;
  Sprite *shadow_sprite;
  int throwing_direction;
  int damage_on_enemies;

 public:

  CarriedItem(Hero *hero, DestructibleItem *destructible_item);
  ~CarriedItem(void);

  EntityType get_type(void);

  int get_damage_on_enemies(void);

  void set_animation_stopped(void);
  void set_animation_walking(void);
  void throw_item(Map *map, int direction);
  bool is_being_thrown(void);
  bool is_broken(void);

  void set_suspended(bool suspended);
  void update(void);
  void display_on_map(void);

  void collision_with_enemy(Enemy *enemy);
  bool is_teletransporter_obstacle(Teletransporter *teletransporter);
  bool is_water_obstacle(void);
  bool is_hole_obstacle(void);
};

#endif
