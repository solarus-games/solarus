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
#ifndef SOLARUS_TRANSITION_FADE_H
#define SOLARUS_TRANSITION_FADE_H

#include "Common.h"
#include "Transition.h"

/**
 * @brief Fade in or fade out transition effect.
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
    ~TransitionFade();

    void set_delay(uint32_t delay);

    void start();
    bool is_started();
    bool is_finished();
    void update();
    void display(Surface *surface);

};

#endif

