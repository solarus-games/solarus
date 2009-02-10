#include "GameoverSequence.h"
#include "Game.h"
#include "ResourceManager.h"
#include "Sprite.h"
#include "Color.h"
#include "Map.h"
#include "Equipment.h"
#include "Treasure.h"
#include "Sound.h"
#include "entities/Link.h"
#include <sstream>

/**
 * Creates a game over sequence.
 * @param game the game
 */
GameoverSequence::GameoverSequence(Game *game):
  game(game), state(DYING), finished(false) {

  gameover_menu_img = ResourceManager::load_image("hud/gameover_menu.png");
  fade_sprite = new Sprite("hud/gameover_fade");
  fade_sprite->stop_animation();

  std::ostringstream oss;
  oss << "link/tunic" << game->get_equipment()->get_tunic();
  hero_dead_sprite = new Sprite(oss.str());
  hero_dead_sprite->set_current_animation("dying");

  SDL_Rect *screen_position = game->get_current_map()->get_screen_position();
  Link *hero = game->get_link();
  hero_dead_x = hero->get_x() - screen_position->x;
  hero_dead_y = hero->get_y() - screen_position->y;

  ResourceManager::get_sound("gameover")->play();
}

/**
 * Destroys the game over sequence.
 */
GameoverSequence::~GameoverSequence(void) {

  SDL_FreeSurface(gameover_menu_img);
  delete fade_sprite;
  delete hero_dead_sprite;
}

/**
 * Updates the gameover sequence.
 */
void GameoverSequence::update(void) {

  hero_dead_sprite->update();

  if (state == DYING) {
    if (hero_dead_sprite->is_last_frame_reached()) {
      state = CLOSING_GAME;
      fade_sprite->restart_animation();
    }
  }
  else if (state < MENU) {
    fade_sprite->update();

    if (fade_sprite->is_over()) {

      if (state == CLOSING_GAME) {
	state = OPENING_MENU;
	fade_sprite->set_current_animation("open");
      }
      else if (game->get_equipment()->has_bottle_with(Treasure::FAIRY_IN_BOTTLE)) {
	state = SAVED_BY_FAIRY;
      }
      else {
	state = MENU;
      }
    }
  }
}

/**
 * Displays the gameover sequence.
 */
void GameoverSequence::display(SDL_Surface *destination_surface) {

  if (state > CLOSING_GAME) {
    SDL_FillRect(destination_surface, NULL, Color::black);
  }

  if (state <= OPENING_MENU) {
    fade_sprite->display(destination_surface, 0, 0);
  }

  if (state <= SAVED_BY_FAIRY) {
    hero_dead_sprite->display(destination_surface, hero_dead_x, hero_dead_y);
  }

  if (state == MENU) {
    SDL_BlitSurface(gameover_menu_img, NULL, destination_surface, NULL);
  }
}
