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
#include "hud/FloorView.h"
#include "Game.h"
#include "Map.h"
#include "Dungeon.h"
#include "lowlevel/Surface.h"
#include "lowlevel/System.h"

/**
 * @brief Constructor.
 * @param game the current game
 * @param x x coordinate of the top-left corner of the element on the destination surface
 * @param y y coordinate of the top-left corner of the element on the destination surface
 */
FloorView::FloorView(Game &game, int x, int y):
  HudElement(game, x, y, 32, 85),
  current_map(NULL),
  current_floor(-100),
  is_floor_displayed(false) {

  img_floors = new Surface("floors.png", Surface::DIR_LANGUAGE);

  rebuild();
}

/**
 * @brief Destructor.
 */
FloorView::~FloorView() {
  delete img_floors;
}

/**
 * @brief Updates the floors displayed.
 */
void FloorView::update() {

  HudElement::update();

  bool need_rebuild = false;

  // detect when the hero enters a new map
  if (&game->get_current_map() != current_map) {

    int old_floor = (current_map != NULL) ? current_floor : -100;
    current_map = &game->get_current_map();
    current_floor = current_map->get_floor();

    if (current_map->has_floor() && current_floor != old_floor) {
      is_floor_displayed = true;
      hide_floor_date = System::now() + 3000;
    }
    else {
      is_floor_displayed = false;
    }
    need_rebuild = true;
  }
  else if (is_floor_displayed && System::now() >= hide_floor_date) {
    is_floor_displayed = false;
    need_rebuild = true;
  }

  if (need_rebuild) {
    rebuild();
  }
}


/**
 * @brief Redraws the floor (if any) on the surface.
 */
void FloorView::rebuild() {

  HudElement::rebuild();

  if (is_floor_displayed) { // a floor is being displayed

    current_floor = current_map->get_floor();
    int highest_floor, highest_floor_displayed;

    // if we are in a dungeon, show several floors (but no more than 7)
    if (current_map->is_in_dungeon() && current_floor != -99) {

      Dungeon &dungeon = game->get_current_dungeon();
      highest_floor = dungeon.get_highest_floor();

      int nb_floors_displayed = dungeon.get_nb_floors_displayed();
      highest_floor_displayed = dungeon.get_highest_floor_displayed(current_floor);

      /*
      std::cout << "lowest_floor: " << lowest_floor
	   << ", highest_floor: " << highest_floor
	   << ", nb_floors: " << nb_floors
	   << ", current_floor: " << current_floor
	   << ", highest_floor_displayed: " << highest_floor_displayed
	   << ", nb_floors_displayed: " << nb_floors_displayed
	   << endl;
      */

      int src_y = (15 - highest_floor_displayed) * 12;
      int src_height = nb_floors_displayed * 12 + 1;

      Rectangle src_position(32, src_y, 32, src_height);

      img_floors->blit(src_position, surface_drawn);
    }
    else {
      highest_floor = current_floor;
      highest_floor_displayed = highest_floor;
    }

    // show the current floor
    int dst_y = (highest_floor_displayed - current_floor) * 12;
    int src_y;

    // special case of the unknown floor
    if (current_floor == -99) {
      src_y = 32 * 12;
    }
    else {
      src_y = (15 - current_floor) * 12;
    }

    Rectangle src_position(0, src_y, 32, 13);
    Rectangle dst_position(0, dst_y, 0, 0);

    img_floors->blit(src_position, surface_drawn, dst_position);
  }
}

