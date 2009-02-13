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
  game(game), music_id(game->get_current_music_id()), state(WAITING_START) {

  gameover_menu_img = ResourceManager::load_image("hud/gameover_menu.png");
  fade_sprite = new Sprite("hud/gameover_fade");
  fade_sprite->stop_animation();
  red_screen_color = Color::create(224, 32, 32);

  std::ostringstream oss;
  oss << "hero/tunic" << game->get_equipment()->get_tunic();
  hero_dead_sprite = new Sprite(oss.str());
  hero_dead_sprite->set_current_animation("hurt");
  hero_dead_sprite->set_current_direction(game->get_hero()->get_animation_direction());
  hero_dead_sprite->set_suspended(true);

  SDL_Rect *camera_position = game->get_current_map()->get_camera_position();
  Hero *hero = game->get_hero();
  hero_dead_x = hero->get_x() - camera_position->x;
  hero_dead_y = hero->get_y() - camera_position->y;

  fairy_sprite = new Sprite("entities/fairy");
  fairy_movement = NULL;

  next_state_date = SDL_GetTicks() + 500;
}

/**
 * Destroys the game over sequence.
 */
GameoverSequence::~GameoverSequence(void) {

  SDL_FreeSurface(gameover_menu_img);
  delete fade_sprite;
  delete hero_dead_sprite;
  delete fairy_sprite;
  delete fairy_movement;
}

/**
 * Updates the gameover sequence.
 */
void GameoverSequence::update(void) {

  Uint32 now = SDL_GetTicks();
  hero_dead_sprite->update();

  switch (state) {
    
  case WAITING_START:
    if (now >= next_state_date) {
      state = CLOSING_GAME;
      fade_sprite->restart_animation();
      game->stop_music();
    }
    break;

  case CLOSING_GAME:
    fade_sprite->update();

    if (fade_sprite->is_animation_finished()) {
      state = RED_SCREEN;
      ResourceManager::get_sound("hero_dying")->play();
      hero_dead_sprite->set_suspended(false);
      hero_dead_sprite->set_current_animation("dying");
      hero_dead_sprite->set_current_direction(0);
      next_state_date = now + 2000;
    }
    break;

  case RED_SCREEN:
    hero_dead_sprite->update();

    if (hero_dead_sprite->is_last_frame_reached() && now >= next_state_date) {
      state = OPENING_MENU;
      fade_sprite->set_current_animation("open");
    }
    break;

  case OPENING_MENU:
    fade_sprite->update();
    if (fade_sprite->is_animation_finished()) {

      Equipment *equipment = game->get_equipment();
      if (equipment->has_bottle_with(Treasure::FAIRY_IN_BOTTLE)) {
	state = SAVED_BY_FAIRY;
	fairy_x = hero_dead_x + 12;
	fairy_y = hero_dead_y + 21;
	fairy_movement = new TargetMovement(240, 22, 10);
	fairy_movement->set_position(fairy_x, fairy_y);
	equipment->set_bottle_empty(equipment->get_first_bottle_with(Treasure::FAIRY_IN_BOTTLE));
      }
      else {
	state = MENU;
	game->play_music("game_over.it");
	fairy_x = 76;
	fairy_y = 124;
	cursor_position = 0;
      }
    }
    break;

  case SAVED_BY_FAIRY:
    fairy_sprite->update();
    fairy_movement->update();
    fairy_x = fairy_movement->get_x();
    fairy_y = fairy_movement->get_y();
    
    if (fairy_movement->is_finished()) {
      state = WAITING_END;
      next_state_date = now + 1000;
      game->get_equipment()->add_hearts(7 * 4);
    }
    break;

  case WAITING_END:
    if (now >= next_state_date) {
      state = RESUME_GAME;
      game->get_hero()->get_back_from_death();
      game->play_music(music_id);
    }
    break;

  case RESUME_GAME:

    break;

  case MENU:
    fairy_sprite->update();
    break;

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

  if (state <= WAITING_END) {
    hero_dead_sprite->display(destination_surface, hero_dead_x, hero_dead_y);

    if (state == SAVED_BY_FAIRY) {
      fairy_sprite->display(destination_surface, fairy_x, fairy_y);
    }
  }
  else if (state == MENU) {
    SDL_BlitSurface(gameover_menu_img, NULL, destination_surface, NULL);
    fairy_sprite->display(destination_surface, fairy_x, fairy_y);
  }
}

/**
 * This function is called when a game key is pressed.
 * @param key the key pressed
 */
void GameoverSequence::key_pressed(Controls::GameKey key) {

  if (state == MENU) {

    if (key == Controls::DOWN) {
      ResourceManager::get_sound("cursor")->play();
      cursor_position = (cursor_position + 1) % 4;
      fairy_y = 124 + cursor_position * 16;
    }
    else if (key == Controls::UP) {
      ResourceManager::get_sound("cursor")->play();
      cursor_position = (cursor_position + 3) % 4;
      fairy_y = 124 + cursor_position * 16;
    }
    else if (key == Controls::ACTION || key == Controls::SWORD) {
      ResourceManager::get_sound("danger")->play();
      game->get_equipment()->add_hearts(7 * 4);

      switch (cursor_position) {

      case 0: // save and continue
	game->get_savegame()->save();
	game->restart();
	break;

      case 1: // save and quit
	game->get_savegame()->save();
	game->reset();
	break;

      case 2: // continue without saving
	game->restart();
	break;

      case 3: // quit without saving
	game->reset();
	break;

      }
    }
  }
}

/**
 * Returns whether the game over sequence is finished.
 * @return true if the game over sequence is finished
 */
bool GameoverSequence::is_finished(void) {
  return state == RESUME_GAME;
}
