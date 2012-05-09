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
#include "lowlevel/Surface.h"

/**
 * @brief A screen controlled by a script.
 */
class CustomScreen: public Screen {

  public:

    // creation and destruction
    CustomScreen(MainLoop& main_loop, const std::string& screen_name);
    ~CustomScreen();

    // update and display
    void notify_input(InputEvent& event);
    void update();
    void display(Surface& dst_surface);

  private:

    std::string menu_id;  // Id of the Lua menu showed in this screen.
};

#endif


