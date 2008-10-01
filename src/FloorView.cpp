#include "FloorView.h"
#include "Game.h"
#include "ResourceManager.h"

/**
 * Constructor.
 * @param game the game
 * @param x x coordinate of the top-left corner of the element on the destination surface
 * @param y y coordinate of the top-left corner of the element on the destination surface
 */
FloorView::FloorView(Game *game, int x, int y):
  HudElement(x, y, 32, 84), game(game) {

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
  // TODO
}


/**
 * Redraws the floor (if any) on the surface.
 */
void FloorView::rebuild(void) {

  HudElement::rebuild();
  /* TODO
  if (is_floor_displayed) { // a floor is being displayed

    int floor = highest_floor;
    for (int i = 0; i <= nb_floors_displayed; i++) {

      int src_y = (15 - floor) * 12;
      int src_x = 32;
      if (floor == current_floor) {
	src_x = 0;
      }

      SDL_Rect src_position = {src_x, src_y, 32, 12};

      int dst_y = floor * 12;
      SDL_Rect dst_position = {0, dst_y, 0, 0};

      SDL_BlitSurface(img_floors, &src_position, surface_drawn, NULL);

      floor--;
    }
    }*/
}
