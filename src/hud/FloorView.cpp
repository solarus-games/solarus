#include "hud/FloorView.h"
#include "Game.h"
#include "ResourceManager.h"
#include "Map.h"
#include "Dungeon.h"

/**
 * Constructor.
 * @param game the game
 * @param x x coordinate of the top-left corner of the element on the destination surface
 * @param y y coordinate of the top-left corner of the element on the destination surface
 */
FloorView::FloorView(Game *game, int x, int y):
  HudElement(x, y, 32, 85), game(game), current_map(NULL),
  is_floor_displayed(false) {

  img_floors = ResourceManager::load_image("hud/floors.png");

  rebuild();
}

/**
 * Destructor.
 */
FloorView::~FloorView(void) {
  SDL_FreeSurface(img_floors);
}

/**
 * Updates the floors displayed.
 */
void FloorView::update(void) {

  bool need_rebuild = false;

  // detect when the players enters a new map
  if (game->get_current_map() != current_map) {

    current_map = game->get_current_map();

    if (current_map->has_floor()) {
      is_floor_displayed = true;
      hide_floor_date = SDL_GetTicks() + 3000;
    }
    else {
      is_floor_displayed = false;
    }
    need_rebuild = true;
  }
  else if (is_floor_displayed && SDL_GetTicks() >= hide_floor_date) {
    is_floor_displayed = false;
    need_rebuild = true;
  }

  if (need_rebuild) {
    rebuild();
  }
}


/**
 * Redraws the floor (if any) on the surface.
 */
void FloorView::rebuild(void) {

  HudElement::rebuild();

  if (is_floor_displayed) { // a floor is being displayed

    int current_floor = current_map->get_floor();
    int highest_floor, highest_floor_displayed;

    // if we are in a dungeon, show several floors (but no more than 7)
    if (current_map->is_in_dungeon() && current_floor != -99) {

      Dungeon *dungeon = game->get_current_dungeon();
      highest_floor = dungeon->get_highest_floor();

      int nb_floors_displayed = dungeon->get_nb_floors_displayed();
      highest_floor_displayed = dungeon->get_highest_floor_displayed(current_floor);

      /*
      cout << "lowest_floor: " << lowest_floor
	   << ", highest_floor: " << highest_floor
	   << ", nb_floors: " << nb_floors
	   << ", current_floor: " << current_floor
	   << ", highest_floor_displayed: " << highest_floor_displayed
	   << ", nb_floors_displayed: " << nb_floors_displayed
	   << endl;
      */

      int src_y = (15 - highest_floor_displayed) * 12;
      int src_height = nb_floors_displayed * 12 + 1;

      SDL_Rect src_position = {32, src_y, 32, src_height};

      SDL_BlitSurface(img_floors, &src_position, surface_drawn, NULL);
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

    SDL_Rect src_position = {0, src_y, 32, 13};
    SDL_Rect dst_position = {0, dst_y, 0, 0};

    SDL_BlitSurface(img_floors, &src_position, surface_drawn, &dst_position);
  }
}
