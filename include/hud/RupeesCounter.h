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
#ifndef SOLARUS_RUPEES_VIEW_H
#define SOLARUS_RUPEES_VIEW_H

#include "Common.h"
#include "hud/HudElement.h"

/**
 * This class displays the rupee counter and its icon.
 */
class RupeesCounter: public HudElement {

  private:

    // rectangles of the rupee icons in the image
    static Rectangle rupee_icon_positions[3];

    Surface *img_rupee_icon;

    int nb_max_rupees_displayed;
    int icon_displayed;
    Counter *counter;

    uint32_t next_rupee_update_date;

  public:

    RupeesCounter(Game *game, int x, int y);
    ~RupeesCounter(void);

    void update(void);
    void rebuild(void);
};

#endif

