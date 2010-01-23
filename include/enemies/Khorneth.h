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
 * The Khorneth boss.
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

    void start_final_phase(void);

    Sprite * get_main_sprite(void);
    Sprite * get_left_blade_sprite(void);
    Sprite * get_right_blade_sprite(void);
    bool has_left_blade(void);
    bool has_right_blade(void);
    bool has_blade(void);
    bool is_left_blade_being_hurt(void);
    bool is_right_blade_being_hurt(void);

  protected:

    void initialize(void);
    void restart(void);
    int get_attack_consequence(EnemyAttack attack, Sprite *this_sprite);
    int custom_attack(EnemyAttack attack, Sprite *this_sprite);

  public:

    Khorneth(const ConstructionParameters &params);
    ~Khorneth(void);

    void update(void);
};

#endif

