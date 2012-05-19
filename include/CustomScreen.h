/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_CUSTOM_SCREEN_H
#define SOLARUS_CUSTOM_SCREEN_H

#include "Common.h"
#include "Screen.h"

/**
 * @brief An execution phase of the program fully controlled by Lua menu.
 */
class CustomScreen: public Screen {

  public:

    // creation and destruction
    CustomScreen(MainLoop& main_loop, int menu_ref);
    ~CustomScreen();

    void start();
    void stop();

    // update and display
    void notify_input(InputEvent& event);
    void update();
    void display(Surface& dst_surface);

  private:

    int menu_ref;  /**< The Lua ref of the menu shown in this screen. */

};

#endif


