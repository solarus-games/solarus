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
#ifndef ZSDX_EXPLOSION_H
#define ZSDX_EXPLOSION_H

#include "Common.h"
#include "entities/Detector.h"
#include <list>

/**
 * Represents an explosion on the map.
 * An explosion can be caused by several entities such as bombs, bomb flowers and enemies.
 * An explosion can hurt the hero, the enemies and open weak walls.
 */
class Explosion: public Detector {

  private:

    std::list<Enemy*> victims; /**< list of enemies successfully hurt by this explosion */

  public:

    Explosion(Layer layer, const SDL_Rect &xy, bool with_damages);
    ~Explosion(void);

    EntityType get_type(void);

    // features
    bool can_be_obstacle(void);
    bool can_detect_entities(void);
    bool can_be_displayed(void);
    bool is_displayed_in_y_order(void);

    // state
    void update(void);

    // collisions
    void notify_collision(MapEntity *other_entity, Sprite *other_sprite, Sprite *this_sprite);
    void notify_collision_with_enemy(Enemy *enemy, Sprite *enemy_sprite, Sprite *this_sprite);
    void try_attack_enemy(Enemy *enemy, Sprite *enemy_sprite);
    void just_attacked_enemy(EnemyAttack attack, Enemy *victim, int result);
};

#endif

