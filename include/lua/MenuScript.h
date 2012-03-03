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
#ifndef SOLARUS_MENU_SCRIPT_H
#define SOLARUS_MENU_SCRIPT_H

#include "Common.h"
#include "lua/Script.h"

/**
 * @brief Represents the Lua script of a menu displayed to the player.
 */
class MenuScript: public Script {

  public:

    MenuScript(const std::string& file_name, CustomScreen* screen);
    ~MenuScript();

    CustomScreen& get_screen();

    void notify_input(InputEvent& event);

    void event_menu_started();
    void event_display(Surface& dst_surface);
    void event_key_pressed(InputEvent& event);
    void event_key_released(InputEvent& event);
    void event_joypad_button_pressed(InputEvent& event);
    void event_joypad_button_released(InputEvent& event);
    void event_joypad_axis_moved(InputEvent& event);
    void event_joypad_hat_moved(InputEvent& event);
    void event_direction_pressed(InputEvent& event);

  private:

    CustomScreen* screen;    /**< the screen controlled by this script, if any */
};

#endif

