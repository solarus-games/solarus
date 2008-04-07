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
#include "AnimatedSprite.h"

/**
 * Position of the hearts in the PNG image.
 */
SDL_Rect HeartsView::full_heart_position = {27, 0, 9, 9};
SDL_Rect HeartsView::fraction_heart_positions[3] = {
  {0, 0, 9, 9},
  {9, 0, 9, 9},
  {18, 0, 9, 9}
};

/**
 * Constructor.
 * @param x x coordinate of the top-left corner of the hearts on the destination surface
 * @param y y coordinate of the top-left corner of the hearts on the destination surface
 */
HeartsView::HeartsView(Savegame *savegame, int x, int y):
  HudElement(savegame, x, y, 90, 18),
  next_heart_update_date(SDL_GetTicks()),
  next_danger_sound_date(SDL_GetTicks()) {

  img_hearts = FileTools::open_image("hud/hearts.png");
  empty_heart_sprite = new AnimatedSprite(zsdx->game_resource->get_sprite("hud/empty_heart"));

  if (!savegame->is_empty()) {
    
    nb_max_hearts_displayed = savegame->get_reserved_integer(SAVEGAME_MAX_HEARTS);
    nb_current_hearts_displayed = savegame->get_reserved_integer(SAVEGAME_CURRENT_HEARTS);
    
    rebuild();
  }
}

/**
 * Destructor.
 */
HeartsView::~HeartsView(void) {
  SDL_FreeSurface(img_hearts);
  delete empty_heart_sprite;
}

/**
 * Updates the number of hearts displayed.
 */
void HeartsView::update(void) {

  bool need_rebuild = false;

  // max hearts
  int nb_max_hearts = savegame->get_reserved_integer(SAVEGAME_MAX_HEARTS);
  if (nb_max_hearts != nb_max_hearts_displayed) {
    nb_max_hearts_displayed = nb_max_hearts;
    need_rebuild = true;
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

    need_rebuild = true;
  }

  /* If we are in game and the player has less than 1/4 of his hearts left,
   * play the 'danger' sound and animate the empty hearts
   */
  if (zsdx->game != NULL) {

    if (nb_current_hearts_displayed <= nb_max_hearts) {

      if (empty_heart_sprite->get_current_animation() != "danger") {
	empty_heart_sprite->set_current_animation("danger");
      }
      empty_heart_sprite->update_current_frame();

      if (SDL_GetTicks() > next_danger_sound_date) {
	next_danger_sound_date = SDL_GetTicks() + 750;
	zsdx->game_resource->get_sound("danger")->play();
      }

      need_rebuild = true;
    }
    else if (empty_heart_sprite->get_current_animation() != "normal") {
      empty_heart_sprite->set_current_animation("normal");
      need_rebuild = true;
    }
  }

  // redraw the surface if something has changed
  if (need_rebuild) {
    rebuild();
  }
}

/**
 * Redraws the hearts on the surface.
 */
void HeartsView::rebuild(void) {

  SDL_FillRect(surface_drawn, NULL, COLOR_BLACK);

  // max hearts

  // blit each empty heart on the hearts surface
  
  for (int i = 0; i < nb_max_hearts_displayed; i++) {
    int x = (i % 10) * 9;
    int y = (i / 10) * 9;

    empty_heart_sprite->display(surface_drawn, x, y);
  }
  
  // current hearts
  SDL_Rect heart_position = {0, 0, 9, 9};
  int i;
  for (i = 0; i < nb_current_hearts_displayed / 4; i++) {
    heart_position.x = (i % 10) * 9;
    heart_position.y = (i / 10) * 9;
    SDL_BlitSurface(img_hearts, &full_heart_position, surface_drawn, &heart_position);
  }

  // last heart: fraction of heart
  int remaining_fraction = nb_current_hearts_displayed % 4;
  if (remaining_fraction != 0) {

    heart_position.x = (i % 10) * 9;
    SDL_BlitSurface(img_hearts, &fraction_heart_positions[remaining_fraction - 1],
		    surface_drawn, &heart_position);
  }
}
