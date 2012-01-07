/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#include "lua/MenuScript.h"

/**
 * @brief A screen controlled by a script.
 */
class CustomScreen: public Screen {

  public:

    // creation and destruction
    CustomScreen(Solarus& solarus, const std::string& screen_name);
    ~CustomScreen();

    // update and display
    void notify_event(InputEvent& event);
    void update();
    void display(Surface* destination_surface);

    void start_screen(const std::string& screen_name);
    void start_game(const std::string& savegame_file);

  private:

    MenuScript script;
};

#endif


