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
#ifndef ZSDX_BOOMERANG_H
#define ZSDX_BOOMERANG_H

#include "Common.h"
#include "entities/MapEntity.h"

/**
 * A boomerang thrown on the map.
 */
class Boomerang: public MapEntity {

 private:

  Hero *hero;              /**< the hero */

  Uint32 next_sound_date;  /**< date when the boomerang sound will be played next time */

  bool has_to_go_back;     /**< true if the boomerang is about to go back */
  bool going_back;         /**< indicates that the boomerang is going back towards the hero */

  SDL_Rect initial_coords; /**< coordinates of the boomerang's initial position */

 public:

  Boomerang(Hero *hero, int direction);
  ~Boomerang(void);

  EntityType get_type(void);

  // features
  bool can_be_obstacle(void);
  bool can_detect_entities(void);
  bool can_be_displayed(void);
  bool is_displayed_in_y_order(void);

  bool is_teletransporter_obstacle(Teletransporter *teletransporter);
  bool is_conveyor_belt_obstacle(ConveyorBelt *conveyor_belt);
  bool is_water_obstacle(void);
  bool is_hole_obstacle(void);
  bool is_raised_block_obstacle(CrystalSwitchBlock *raised_block);
  bool is_crystal_switch_obstacle(CrystalSwitch *crystal_switch);
  bool is_npc_obstacle(InteractiveEntity *npc);

  // state
  bool is_going_back(void);
  void go_back(void);

  void update(void);

  // collisions
  void collision_with_enemy(Enemy *enemy);
  void just_attacked_enemy(EnemyAttack attack, Enemy *victim, int result);
};


#endif
 
