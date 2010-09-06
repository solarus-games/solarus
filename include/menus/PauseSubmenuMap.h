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
#ifndef PAUSE_SUBMENU_MAP_H
#define PAUSE_SUBMENU_MAP_H

#include "Common.h"
#include "menus/PauseSubmenu.h"
#include "lowlevel/Rectangle.h"

/**
 * @brief The Map submenu of the pause menu.
 *
 * When the player is not in a dungeon, this submenu shows the outside map 
 * if he has the world map.
 * When the player is in a dungeon, this submenu shows the dungeon information,
 * possibly including the minimap and other data depending on
 * whether he has the map and the compass.
 */
class PauseSubmenuMap: public PauseSubmenu {

  private:

    static const Rectangle outside_world_minimap_size;

    // data
    Equipment *equipment;        /**< general equipment */
    Dungeon *dungeon;            /**< the dungeon whose map is displayed, or NULL if we are not in a dungeon */
    Rectangle hero_position;     /**< position of the hero on the minimap */

    // graphics
    Sprite *hero_head_sprite;
    Sprite *up_arrow_sprite;
    Sprite *down_arrow_sprite;

    // outside world map
    Surface *world_map_img;
    int world_minimap_visible_y; /**< when the world map is displayed: y position of the minimap visible rectangle */
    int moving_visible_y;        /**< 0, -1 or 1 depending on the arrow pressed */
    uint32_t next_moving_visible_y_date;

    // dungeon map
    Surface *dungeon_map_background;
    Surface *dungeon_map_icons;
    Counter *small_keys_counter;

    Surface *dungeon_floors_img;
    int hero_floor, boss_floor, highest_floor, lowest_floor, nb_floors;
    int nb_floors_displayed, highest_floor_displayed, selected_floor;

    Surface *dungeon_map_img;
    Sprite *hero_point_sprite;

    void load_dungeon_map_image(void);
    static void to_dungeon_minimap_coordinates(const Rectangle &floor_coords,
	Rectangle &minimap_coords, const Rectangle &floor_size);

  public:

    PauseSubmenuMap(PauseMenu *pause_menu, Game *game);
    ~PauseSubmenuMap(void);

    void key_pressed(GameControls::GameKey key);
    void update(void);
    void display(Surface *destination);
    void display_world_map(Surface *destination);
    void display_dungeon_map(Surface *destination);
    void display_dungeon_items(Surface *destination);
    void display_dungeon_floors(Surface *destination);
};

#endif

