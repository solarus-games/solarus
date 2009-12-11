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
#ifndef ZSDX_TRANSITION_FADE_H
#define ZSDX_TRANSITION_FADE_H

#include "Common.h"
#include "Transition.h"

/**
 * Fade in or fade out transition effect.
 */
class TransitionFade: public Transition {

  private:

    int alpha_start;
    int alpha_limit;
    int alpha_increment;
    int alpha; // current alpha value of the surface

    uint32_t next_frame_date;
    uint32_t delay;

  public:

    TransitionFade(Direction direction);
    ~TransitionFade(void);

    void set_delay(uint32_t delay);

    void start(void);
    bool is_started(void);
    bool is_over(void);
    void display(Surface *surface);

};

#endif

