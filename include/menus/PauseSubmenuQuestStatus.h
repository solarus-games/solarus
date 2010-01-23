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
#ifndef PAUSE_SUBMENU_QUEST_STATUS_H
#define PAUSE_SUBMENU_QUEST_STATUS_H

#include "Common.h"
#include "menus/PauseSubmenu.h"
#include "Controls.h"
#include "lowlevel/Rectangle.h"

/**
 * The Quest Status submenu of the pause menu.
 */
class PauseSubmenuQuestStatus: public PauseSubmenu {

  private:

    Surface *quest_items_surface;

    Sprite *cursor_sprite;
    int cursor_position; /**< 0 to 7 */
    Rectangle cursor_sprite_position;

    std::string caption_texts[8];

    void set_cursor_position(int position);

  public:

    PauseSubmenuQuestStatus(PauseMenu *pause_menu, Game *game);
    ~PauseSubmenuQuestStatus(void);

    void key_pressed(Controls::GameKey key);
    void update(void);
    void display(Surface *destination);
};

#endif

