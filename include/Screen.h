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
#ifndef SOLARUS_SCREEN_H
#define SOLARUS_SCREEN_H

#include "Common.h"

/**
 * @brief Represents an execution phase of the program.
 *
 * A screen is an execution phase of the program, like the title
 * screen, the selection menu or the game itself.
 * There is always a current screen and it is controlled by the main loop of the program.
 *
 * To create a screen, you have to implement a subclass of Screen.
 * The main loop will call three methods that you must implement:
 * - display(): to draw your screen on the main surface. This function
 * is called a certain number of times per second (which is the FPS number).
 * - update(): to update your screen's internal data (this function is
 * called repeatedly). This is useful to update the
 * state of objects that depend on time (e.g. the ones that are animated or moving).
 * - notify_event(): to take into account input events that occur
 * (such as a keyboard key or a joypad button that was just pressed).
 *
 * Basically, the program main loop calls these three functions. If it has enough
 * time (depending on how long are the update() and display() calls), it can choose
 * to sleep between two loops.
 *
 * When your screen is finished and you want to go to another screen,
 * you have to call set_next_screen() with the new screen as parameter.
 */
class Screen {

  protected:

    Solarus *solarus;           /**< the main object of the program (it manages the screens) */

  private:

    bool screen_finished;       /**< indicates that the current screen is finished and should be destroyed */
    Screen *next_screen;        /**< when the current screen is finished, indicates the next screen */

  protected:

    Screen(Solarus *solarus);

    void set_next_screen(Screen *next_screen);

  public:

    virtual ~Screen();

    bool is_screen_finished();
    Screen *get_next_screen();
    Scripts &get_scripts();

    // functions to implement in subclasses and that will be called by the main loop

    /**
     * @brief Updates this screen.
     *
     * This function is called repeatedly by the program's main loop.
     */
    virtual void update() = 0;

    /**
     * @brief Displays this screen on a surface.
     *
     * This function is called by the main loop depending of the number of frames per second.
     *
     * @param screen_surface the surface to draw
     */
    virtual void display(Surface *screen_surface) = 0;

    /**
     * @brief Notifies this screen that an event just occured.
     *
     * This function is called by the main loop when there is an input event.
     *
     * @param event the event to handle
     */
    virtual void notify_event(InputEvent &event) = 0;
};

#endif

