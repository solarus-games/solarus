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
#ifndef SOLARUS_PAUSE_SUBMENU_H
#define SOLARUS_PAUSE_SUBMENU_H

#include "Common.h"
#include "GameControls.h"

/**
 * @brief Abstract class for a submenu of the pause menu.
 */
class PauseSubmenu {

  protected:

    PauseMenu *pause_menu;                    /**< the pause menu this submenu belongs to */
    Game *game;                               /**< the current game */
    Savegame *savegame;                       /**< the savegame */
    Equipment *equipment;                     /**< the player's equipment */
    DungeonEquipment *dungeon_equipment;      /**< the player's dungeon-specific equipment */

    TextSurface *caption_text_1;              /**< the bottom text displayed at left */
    TextSurface *caption_text_2;              /**< the bottom text displayed at right */

    PauseSubmenu(PauseMenu *pause_menu, Game *game);

    void set_caption_text(const std::string &text);
    void play_cursor_sound(void);

  public:

    virtual ~PauseSubmenu(void);

    /**
     * @brief This function is called when a key is pressed on this submenu.
     *
     * The subclasses should redefine it to react to the key that was just pressed.
     *
     * @param key the key pressed
     */
    virtual void key_pressed(GameControls::GameKey key) = 0;

    /**
     * @brief Updates this submenu.
     *
     * This function is called continuously while this submenu is active.
     */
    virtual void update(void) = 0;

    virtual void display(Surface *destination);
};

#endif

