/**
 * This module defines the class HeartsView.
 */

#include "HeartsView.h"
#include "FileTools.h"
#include "Savegame.h"
#include "Color.h"
#include "ZSDX.h"
#include "GameResource.h"
#include "Sound.h"

/**
 * Position of the hearts in the PNG image.
 */
SDL_Rect HeartsView::empty_heart_position = {0, 0, 9, 9};
SDL_Rect HeartsView::full_heart_position = {36, 0, 9, 9};
SDL_Rect HeartsView::fraction_heart_positions[3] = {
  {9, 0, 9, 9},
  {18, 0, 9, 9},
  {27, 0, 9, 9},
};

/**
 * Constructor.
 * @param x x coordinate of the top-left corner of the hearts on the destination surface
 * @param y y coordinate of the top-left corner of the hearts on the destination surface
 */
HeartsView::HeartsView(Savegame *savegame, int x, int y):
  savegame(savegame), next_heart_update_date(SDL_GetTicks()) {

  destination_position.x = x;
  destination_position.y = y;
  destination_position.w = 9;
  destination_position.h = 9;

  img_hearts = FileTools::open_image("hud/hearts.png");
  hearts_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 90, 18, 32, 0, 0, 0, 0);
  SDL_SetColorKey(hearts_surface, SDL_SRCCOLORKEY, COLOR_BLACK);

  if (!savegame->is_empty()) {
    
    nb_max_hearts_displayed = savegame->get_reserved_integer(SAVEGAME_MAX_HEARTS);
    nb_current_hearts_displayed = savegame->get_reserved_integer(SAVEGAME_CURRENT_HEARTS);
    
    redraw();
  }
}

/**
 * Destructor.
 */
HeartsView::~HeartsView(void) {
  SDL_FreeSurface(img_hearts);
  SDL_FreeSurface(hearts_surface);
}

/**
 * Updates the number of hearts displayed.
 */
void HeartsView::update(void) {

  bool need_redraw = false;

  // max hearts
  int nb_max_hearts = savegame->get_reserved_integer(SAVEGAME_MAX_HEARTS);
  if (nb_max_hearts != nb_max_hearts_displayed) {
    nb_max_hearts_displayed = nb_max_hearts;
    need_redraw = true;
  }

  // current hearts
  int nb_current_hearts = savegame->get_reserved_integer(SAVEGAME_CURRENT_HEARTS);
  if (nb_current_hearts != nb_current_hearts_displayed && SDL_GetTicks() > next_heart_update_date) {

    next_heart_update_date = SDL_GetTicks() + 50;

    if (nb_current_hearts < nb_current_hearts_displayed) {
      nb_current_hearts_displayed--;
    }
    else {
      nb_current_hearts_displayed++;

      if (nb_current_hearts_displayed % 4 == 0) {
	zsdx->game_resource->get_sound("heart")->play();
      }
    }

    need_redraw = true;
  }

  // redraw the screen if something has changed
  if (need_redraw) {
    redraw();
  }
}

/**
 * Redraws the hearts on the surface.
 */
void HeartsView::redraw(void) {

  SDL_FillRect(hearts_surface, NULL, COLOR_BLACK);

  // max hearts

  // blit each empty heart on the hearts surface
  SDL_Rect heart_position = {0, 0, 9, 9};
  
  for (int i = 0; i < nb_max_hearts_displayed; i++) {
    heart_position.x = (i % 10) * 9;
    heart_position.y = (i / 10) * 9;
    SDL_BlitSurface(img_hearts, &empty_heart_position, hearts_surface, &heart_position);
  }

  
  // current hearts
  heart_position.x = 36;
  int i;
  for (i = 0; i < nb_current_hearts_displayed / 4; i++) {
    heart_position.x = (i % 10) * 9;
    heart_position.y = (i / 10) * 9;
    SDL_BlitSurface(img_hearts, &full_heart_position, hearts_surface, &heart_position);
  }

  // last heart: fraction of heart
  int remaining_fraction = nb_current_hearts_displayed % 4;
  if (remaining_fraction != 0) {

    heart_position.x = (i % 10) * 9;
    SDL_BlitSurface(img_hearts, &fraction_heart_positions[remaining_fraction - 1],
		    hearts_surface, &heart_position);
  }
}

/**
 * Displays the hearts on a surface.
 * @param destination the destination surface
 */
void HeartsView::display(SDL_Surface *destination) {

  if (!savegame->is_empty()) {
    SDL_BlitSurface(hearts_surface, NULL, destination, &destination_position);
  }
}
