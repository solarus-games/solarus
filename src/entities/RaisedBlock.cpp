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
#include "entities/RaisedBlock.h"
#include "ZSDX.h"
#include "Game.h"
#include "Map.h"
#include "Sprite.h"

/**
 * Creates a new raised block.
 * @param layer layer of the entity to create on the map
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 * @param width width of the block (the pattern can be repeated)
 * @param height height of the block (the pattern can be repeated)
 * @param subtype subtype of raised block
 */
RaisedBlock::RaisedBlock(Layer layer, int x, int y, int width, int height, Subtype subtype):
  Detector(COLLISION_NONE, "", layer, x, y, width, height),
  subtype(subtype), orange_raised(false) {

  create_sprite("entities/raised_block");

  if (subtype == BLUE) {
    get_sprite()->set_current_animation("blue_raised");
  }
}

/**
 * Destructor.
 */
RaisedBlock::~RaisedBlock(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType RaisedBlock::get_type() {
  return RAISED_BLOCK;
}

/**
 * Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this block is raised
 */
bool RaisedBlock::is_obstacle_for(MapEntity *other) {

  if ((subtype == ORANGE && !orange_raised) ||
      (subtype == BLUE && orange_raised)) {

    return false;
  }

  return !overlaps(other->get_position_in_map());
}

/**
 * Updates the entity.
 */
void RaisedBlock::update(void) {

  bool orange_raised = zsdx->game->get_crystal_switch_state();
  if (orange_raised != this->orange_raised) {

    this->orange_raised = orange_raised;

    if (subtype == ORANGE) {
      get_sprite()->set_current_animation(orange_raised ? "orange_raised" : "orange_lowered");
    }
    else {
      get_sprite()->set_current_animation(orange_raised ? "blue_lowered" : "blue_raised");
    }
  }
  MapEntity::update();
}

/**
 * Displays the entity on the map.
 * This is a redefinition of MapEntity::display_on_map to repeat the block pattern.
 */
void RaisedBlock::display_on_map(void) {

  Sprite *sprite = get_sprite();

  int x1 = get_top_left_x();
  int y1 = get_top_left_y();
  int x2 = x1 + get_width();
  int y2 = y1 + get_height();

  for (int y = y1; y < y2; y += 16) {
    for (int x = x1; x < x2; x += 16) {
      map->display_sprite(sprite, x, y);
    }
  }
}
