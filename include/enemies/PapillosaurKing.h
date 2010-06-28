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
#ifndef SOLARUS_PAPILLAUSOR_KING_H
#define SOLARUS_PAPILLAUSOR_KING_H

#include "Common.h"
#include "entities/Enemy.h"

/**
 * @brief A big butterfly boss from Newlink.
 */
class PapillosaurKing: public Enemy {

  private:

    uint32_t next_egg_phase_date;  /**< date of the next egg creation phase */
    uint32_t next_egg_date;        /**< during an egg phase, date when a minillosaur egg is created */
    int nb_eggs_to_create;         /**< number of eggs to create in the current egg phase */

  protected:

    void initialize(void);
    void restart(void);

  public:

    PapillosaurKing(const ConstructionParameters &params);
    ~PapillosaurKing(void);

    bool is_destructible_item_obstacle(DestructibleItem *destructible_item);
    void set_suspended(bool suspended);
    void update(void);
    void just_hurt(MapEntity *source, EnemyAttack attack, int life_points);
};

#endif

