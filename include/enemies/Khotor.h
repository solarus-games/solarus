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
#ifndef SOLARUS_KHOTOR_H
#define SOLARUS_KHOTOR_H

#include "Common.h"
#include "entities/Enemy.h"

/**
 * @brief The boss Khotor.
 *
 * Khotor controls a chain and ball.
 */
class Khotor: public Enemy {

  private:

    ChainAndBall *chain;
    void start_chain(void);

  protected:

    void initialize(void);
    void restart(void);

  public:

    Khotor(const ConstructionParameters &params);
    ~Khotor(void);

    bool is_displayed_in_y_order(void);
    void just_hurt(MapEntity *source, EnemyAttack attack, int life_points);
};

#endif

