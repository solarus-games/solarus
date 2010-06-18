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
#ifndef SOLARUS_CHAIN_AND_BALL_H
#define SOLARUS_CHAIN_AND_BALL_H

#include "Common.h"
#include "entities/Enemy.h"

/**
 * A ball and its chain, usually controlled by another enemy.
 */
class ChainAndBall: public Enemy {

  private:



  protected:

    void initialize(void);

  public:

    ChainAndBall(const ConstructionParameters &params);
    ~ChainAndBall(void);

    void attach_to(MapEntity *entity, int x = 0, int y = 0);
    void twirl(void);
    void update(void);
};

#endif

