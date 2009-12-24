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
#ifndef ZSDX_SCREEN_H
#define ZSDX_SCREEN_H

#include "Common.h"
#include <SDL/SDL.h>

/**
 * A screen is an execution phase of the program, like the title
 * screen, the selection menu screens or the game itself.
 * There is always a current screen and it is managed by the main loop.
 *
 * To create a screen, you have to make a subclass of Screen.
 * The main loop will call three methods that you must implement:
 * - display: to draw your screen on the screen surface. This function
 * is called a certain number of times per second (FPS).
 * - update: to update your screen's internal data (this function is
 * called repeatedly). This is useful to update the
 * state of objects that depend on time (e.g. they are animated or moving).
 * - handle_event: to take into account events that occur
 * (such as a key that was just pressed).
 * TODO: replace handle_event by some higher level functions: handle_key_pressed,
 * handle_key_released, handle_joypad_... This will encapsulate SDL event structures
 * and allow using the joypad in the menus. Also rename Controls into GameControls.
 *
 * Basically, the SDL main loop calls these three functions. If it has enough
 * time (depending on how long are the update() and display() calls), it also
 * sleeps between two loops.
 *
 * When your screen is finished and you want to go to another screen,
 * you have to call set_next_screen() with the new screen as parameter.
 */
class Screen {

  private:

    bool screen_finished;
    Screen *next_screen;

  protected:

    Screen(void);

    void set_next_screen(Screen *next_screen);

  public:

    virtual ~Screen(void);

    bool is_screen_finished(void);
    Screen *get_next_screen(void);

    // functions to implement and that will be called by the main loop
    virtual void update(void) = 0;
    virtual void display(Surface *screen_surface) = 0;
    virtual void handle_event(const SDL_Event &event) = 0;
};

#endif

