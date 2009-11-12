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
#include "TransitionScrolling.h"
#include "ZSDX.h"
#include "Game.h"
#include "Map.h"
#include "Color.h"

/**
 * Position where the previous map is blitted on both_maps_surface,
 * for each possible scrolling direction.
 */
const SDL_Rect TransitionScrolling::previous_map_dst_positions[] = {
  {0, 0, 0, 0},   // scroll to the east
  {0, 240, 0, 0}, // scroll to the north
  {320, 0, 0, 0}, // scroll to the west
  {0, 0, 0, 0},   // scroll to the south
};

/**
 * Constructor.
 */
TransitionScrolling::TransitionScrolling(Transition::Direction direction):
  Transition(direction), dx(0), dy(0) {

}

/**
 * Destructor.
 */
TransitionScrolling::~TransitionScrolling(void) {

  if (direction == IN) {
    SDL_FreeSurface(both_maps_surface);
  }
}

/**
 * Starts this transition effect.
 */
void TransitionScrolling::start(void) {
  
  if (direction == OUT) {
    return;
  }

  // get the scrolling direction
  scrolling_direction = (zsdx->game->get_current_map()->get_destination_side() + 2) % 4;

  const int scrolling_step = 5;
  int width = 320;
  int height = 240;
  if (scrolling_direction % 2 == 0) {
    // right or left
    width *= 2;
    dx = (scrolling_direction == 0) ? scrolling_step : -scrolling_step;
  }
  else {
    height *= 2;
    dy = (scrolling_direction == 3) ? scrolling_step : -scrolling_step;
  }

  // create a surface with the two maps
  both_maps_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, 32, 0, 0, 0, 0);

  // set the blitting rectangles

  previous_map_dst_position = previous_map_dst_positions[scrolling_direction];
  current_map_dst_position = previous_map_dst_positions[(scrolling_direction + 2) % 4];
  current_scrolling_position = previous_map_dst_position;

  current_scrolling_position.w = 320;
  current_scrolling_position.h = 240;

  next_scroll_date = SDL_GetTicks();
}

/**
 * Returns whether this transition effect needs the previous surface.
 * @return true
 */
bool TransitionScrolling::needs_previous_surface(void) {
  return true;
}

/**
 * Returns whether the transition effect is started.
 * @return false
 */
bool TransitionScrolling::is_started(void) {
  return !is_over();
}

/**
 * Returns whether the transition effect is over.
 * @return true
 */
bool TransitionScrolling::is_over(void) {

  if (direction == OUT) {
    return true;
  }

  return current_scrolling_position.x == current_map_dst_position.x
    && current_scrolling_position.y == current_map_dst_position.y;
}

/**
 * Displays the transition effect on a surface.
 */
void TransitionScrolling::display(SDL_Surface *surface) {

  if (direction == OUT) {
    return;
  }

  if (previous_surface == NULL) {
    DIE("No previous surface defined for scrolling");
  }

  // draw the old map
  SDL_BlitSurface(previous_surface, NULL, both_maps_surface, &previous_map_dst_position); 

  // draw the new map
  SDL_BlitSurface(surface, NULL, both_maps_surface, &current_map_dst_position);

  // blit both surfaces
  Uint32 now = SDL_GetTicks();
  while (now >= next_scroll_date && !is_over()) {
    scroll();
    next_scroll_date += 10;
  }

  SDL_FillRect(surface, NULL, Color::black);
  SDL_BlitSurface(both_maps_surface, &current_scrolling_position, surface, NULL);
}


/**
 * Makes a scrolling step.
 */
void TransitionScrolling::scroll(void) {
  current_scrolling_position.x += dx;
  current_scrolling_position.y += dy;
}

