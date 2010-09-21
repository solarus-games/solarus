/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SOLARUS_KHORNETH_H
#define SOLARUS_KHORNETH_H

#include "Common.h"
#include "entities/Enemy.h"

/**
 * @brief The boss Khorneth.
 *
 * Khorneth has two blades that must be destroyed first.
 */
class Khorneth: public Enemy {

  private:

    // blades
    int left_blade_life;
    uint32_t end_left_blade_hurt_date;
    int right_blade_life;
    uint32_t end_right_blade_hurt_date;
    uint32_t next_blade_attack_date;
    bool blade_attack;
    bool blades_destroyed;

    void start_final_phase();

    Sprite * get_main_sprite();
    Sprite * get_left_blade_sprite();
    Sprite * get_right_blade_sprite();
    bool has_left_blade();
    bool has_right_blade();
    bool has_blade();
    bool is_left_blade_being_hurt();
    bool is_right_blade_being_hurt();

  protected:

    void initialize();
    void restart();
    int get_attack_consequence(EnemyAttack attack, Sprite *this_sprite);
    int custom_attack(EnemyAttack attack, Sprite *this_sprite);

  public:

    Khorneth(const ConstructionParameters &params);
    ~Khorneth();

    void update();
};

#endif

