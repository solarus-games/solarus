/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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
#include "GameoverSequence.h"
#include "Game.h"
#include "Savegame.h"
#include "Sprite.h"
#include "Map.h"
#include "Equipment.h"
#include "lowlevel/System.h"
#include "lowlevel/Surface.h"
#include "movements/TargetMovement.h"
#include <sstream>

/**
 * @brief Creates a game over sequence.
 * @param game the game
 * @param hero_direction direction of the hero sprite before game over
 */
GameoverSequence::GameoverSequence(Game *game, int hero_direction):
  game(game), music_id(game->get_current_music_id()), state(WAITING_START) {

  gameover_menu_img = new Surface("gameover_menu.png", Surface::DIR_LANGUAGE);
  fade_sprite = new Sprite("hud/gameover_fade");
  fade_sprite->stop_animation();
  red_screen_color = Color(224, 32, 32);

  std::ostringstream oss;
  oss << "hero/tunic" << game->get_equipment()->get_ability("tunic");
  hero_dead_sprite = new Sprite(oss.str());
  hero_dead_sprite->set_current_animation("hurt");
  hero_dead_sprite->set_current_direction(hero_direction);
  hero_dead_sprite->set_suspended(true);

  const Rectangle &camera_position = game->get_current_map()->get_camera_position();
  const Rectangle &hero_xy = game->get_hero_xy();
  hero_dead_x = hero_xy.get_x() - camera_position.get_x();
  hero_dead_y = hero_xy.get_y() - camera_position.get_y();

  fairy_sprite = new Sprite("entities/fairy");
  fairy_movement = NULL;

  next_state_date = System::now() + 500;
}

/**
 * @brief Destroys the game over sequence.
 */
GameoverSequence::~GameoverSequence(void) {

  delete gameover_menu_img;
  delete fade_sprite;
  delete hero_dead_sprite;
  delete fairy_sprite;
  delete fairy_movement;
}

/**
 * @brief Updates the gameover sequence.
 */
void GameoverSequence::update(void) {

  uint32_t now = System::now();
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
      game->play_sound("hero_dying");
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
      if (equipment->has_ability("get_back_from_death")) {
	state = SAVED_BY_FAIRY;
	fairy_x = hero_dead_x + 12;
	fairy_y = hero_dead_y + 21;
	fairy_movement = new TargetMovement(240, 22, 10);
	fairy_movement->set_position(fairy_x, fairy_y);
      }
      else {
	state = MENU;
	game->play_music("game_over.spc");
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
      game->get_equipment()->add_life(7 * 4);
    }
    break;

  case WAITING_END:
    if (now >= next_state_date) {
      state = RESUME_GAME;
      game->get_back_from_death();
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
 * @brief Displays the gameover sequence.
 * @param destination_surface the surface to draw
 */
void GameoverSequence::display(Surface *destination_surface) {

  if (state > CLOSING_GAME) {
    destination_surface->fill_with_color(Color::get_black());
  }

  if (state <= OPENING_MENU) {

    if (state == RED_SCREEN) {
      destination_surface->fill_with_color(red_screen_color);
    }
    else {
      fade_sprite->display(destination_surface, hero_dead_x, hero_dead_y);
    }
  }

  if (state <= WAITING_END) {
    hero_dead_sprite->display(destination_surface, hero_dead_x, hero_dead_y);

    if (state == SAVED_BY_FAIRY) {
      fairy_sprite->display(destination_surface, fairy_x, fairy_y);
    }
  }
  else if (state == MENU) {
    gameover_menu_img->blit(destination_surface);
    fairy_sprite->display(destination_surface, fairy_x, fairy_y);
  }
}

/**
 * @brief This function is called when a game key is pressed.
 * @param key the key pressed
 */
void GameoverSequence::key_pressed(GameControls::GameKey key) {

  if (state == MENU) {

    if (key == GameControls::DOWN) {
      game->play_sound("cursor");
      cursor_position = (cursor_position + 1) % 4;
      fairy_y = 124 + cursor_position * 16;
    }
    else if (key == GameControls::UP) {
      game->play_sound("cursor");
      cursor_position = (cursor_position + 3) % 4;
      fairy_y = 124 + cursor_position * 16;
    }
    else if (key == GameControls::ACTION || key == GameControls::SWORD) {
      game->play_sound("danger");
      game->get_equipment()->add_life(7 * 4);

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
 * @brief Returns whether the game over sequence is finished.
 * @return true if the game over sequence is finished
 */
bool GameoverSequence::is_finished(void) {
  return state == RESUME_GAME;
}

