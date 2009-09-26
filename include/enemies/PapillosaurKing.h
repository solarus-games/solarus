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
#ifndef ZSDX_PAPILLAUSOR_KING_H
#define ZSDX_PAPILLAUSOR_KING_H

#include "Common.h"
#include "entities/Enemy.h"

/**
 * A butterfly boss from Newlink.
 */
class PapillosaurKing: public Enemy {

  private:

    Uint32 next_egg_phase_date;  /**< date of the next egg creation phase */
    Uint32 next_egg_date;        /**< during an egg phase, date when a minillosaur egg is created */
    int nb_eggs_to_create;       /**< number of eggs to create in the current egg phase */

  protected:

    void initialize(void);
    void restart(void);

  public:

    PapillosaurKing(const ConstructionParameters &params);
    ~PapillosaurKing(void);

    bool is_destructible_item_obstacle(DestructibleItem *destructible_item);
    void set_suspended(bool suspended);
    void update(void);
};

#endif

