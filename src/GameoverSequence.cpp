#include "GameoverSequence.h"
#include "Game.h"
#include "ResourceManager.h"
#include "Sprite.h"
#include "Color.h"
#include "Map.h"
#include "Equipment.h"
#include "Treasure.h"
#include "Sound.h"
#include "entities/Hero.h"
#include "movements/TargetMovement.h"
#include <sstream>

/**
 * Creates a game over sequence.
 * @param game the game
 */
GameoverSequence::GameoverSequence(Game *game):
  game(game), music_id(game->get_current_music_id()), state(DYING) {

  gameover_menu_img = ResourceManager::load_image("hud/gameover_menu.png");
  fade_sprite = new Sprite("hud/gameover_fade");
  fade_sprite->stop_animation();
  red_screen_color = Color::create(224, 32, 32);

  std::ostringstream oss;
  oss << "hero/tunic" << game->get_equipment()->get_tunic();
  hero_dead_sprite = new Sprite(oss.str());
  hero_dead_sprite->set_current_animation("dying");

  SDL_Rect *screen_position = game->get_current_map()->get_screen_position();
  Hero *hero = game->get_hero();
  hero_dead_x = hero->get_x() - screen_position->x;
  hero_dead_y = hero->get_y() - screen_position->y;

  fairy_sprite = new Sprite("entities/fairy");

  ResourceManager::get_sound("hero_dying")->play();
}

/**
 * Destroys the game over sequence.
 */
GameoverSequence::~GameoverSequence(void) {

  SDL_FreeSurface(gameover_menu_img);
  delete fade_sprite;
  delete hero_dead_sprite;
  delete fairy_sprite;
}

/**
 * Updates the gameover sequence.
 */
void GameoverSequence::update(void) {

  hero_dead_sprite->update();

  if (state == DYING && hero_dead_sprite->is_last_frame_reached()) {
    state = CLOSING_GAME;
    fade_sprite->restart_animation();
    game->stop_music();
  }
  else if (state < MENU) {

    fade_sprite->update();
    Uint32 now = SDL_GetTicks();

    if (state == CLOSING_GAME && fade_sprite->is_last_frame_reached()) {
      state = RED_SCREEN;
      open_menu_date = now + 500;
    }
    else if (state == RED_SCREEN) {

      if (now >= open_menu_date) {
	state = OPENING_MENU;
	fade_sprite->set_current_animation("open");
	game->play_music("game_over.it");
      }
    }
    else if (state == OPENING_MENU && fade_sprite->is_animation_finished()) {

      if (game->get_equipment()->has_bottle_with(Treasure::FAIRY_IN_BOTTLE)) {
	state = SAVED_BY_FAIRY;
	fairy_x = hero_dead_x + 12;
	fairy_y = hero_dead_y + 21;
	fairy_movement = new TargetMovement(10, 240, 22);
	fairy_movement->set_position(fairy_x, fairy_y);
      }
      else {
	state = MENU;
      }
    }
    else if (state == SAVED_BY_FAIRY) {
      fairy_sprite->update();
      fairy_movement->update();
      fairy_x = fairy_movement->get_x();
      fairy_y = fairy_movement->get_y();

      if (fairy_movement->is_finished()) {
	state = RESUME_GAME;
	game->get_equipment()->add_hearts(7 * 4);
	game->get_hero()->get_back_from_death();
	game->play_music(music_id);
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

    if (state == RED_SCREEN) {
      SDL_FillRect(destination_surface, NULL, red_screen_color);
    }
    else {
      fade_sprite->display(destination_surface, 0, 0);
    }
  }

  if (state <= SAVED_BY_FAIRY) {
    hero_dead_sprite->display(destination_surface, hero_dead_x, hero_dead_y);

    if (state == SAVED_BY_FAIRY) {
      fairy_sprite->display(destination_surface, fairy_x, fairy_y);
    }
  }
  else if (state == MENU) {
    SDL_BlitSurface(gameover_menu_img, NULL, destination_surface, NULL);
  }
}

/**
 * Returns whether the game over sequence is finished.
 * @return true if the game over sequence is finished
 */
bool GameoverSequence::is_finished(void) {
  return state == RESUME_GAME;
}
