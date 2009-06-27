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
#include "menus/TitleScreen.h"
#include "menus/SelectionMenuSelectFile.h"
#include "Music.h"
#include "ResourceManager.h"
#include "FileTools.h"
#include "Color.h"
#include "TransitionFade.h"

// TODO remove
#include "Game.h"
#include "Savegame.h"

/**
 * Creates a title screen.
 */
TitleScreen::TitleScreen(void):
  Screen() {

  // go to phase 1
  init_phase_black_screen();
}

/**
 * Destroys the title screen.
 */
TitleScreen::~TitleScreen(void) {

  if (!is_screen_finished()) {

    switch (current_phase) {

    case PHASE_ZS_PRESENTS:
      exit_phase_zs_presents();
      break;

    case PHASE_TITLE:
      exit_phase_title();
      break;

    default:
      break;
    }
  }
}

/**
 * Updates the title screen.
 */
void TitleScreen::update(void) {

  // TODO remove
  set_next_screen(new Game(new Savegame("save1.zsd")));

  Uint32 now = SDL_GetTicks();

  switch (current_phase) {

  case PHASE_BLACK_SCREEN:

    if (now >= next_phase_date) {
      // go to next phase after 0.3 second
      init_phase_zs_presents();
    }

    break;

  case PHASE_ZS_PRESENTS:

    if (now >= next_phase_date) {

      if (transition_out->is_over()) {

	// unload current phase
	exit_phase_zs_presents();

	// go to next phase
	init_phase_title();
      }
      else if (!transition_out->is_started()) {
	transition_out->start();
      }
    }

    break;

  case PHASE_TITLE:
    update_phase_title();
    break;
  }
}

/**
 * Displays the title screen.
 * @param destination surface the surface to draw
 */
void TitleScreen::display(SDL_Surface *destination_surface) {

  switch (current_phase) {

  case PHASE_BLACK_SCREEN:
    // nothing to display
    break;

  case PHASE_ZS_PRESENTS:
    if (transition_out->is_started()) { // out transition
      transition_out->display(zs_presents_img);
    }
    SDL_BlitSurface(zs_presents_img, NULL, destination_surface, &zs_presents_position);
    break;

  case PHASE_TITLE:
    display_phase_title(destination_surface);
    break;
  }
}

/**
 * Handles an SDL event.
 * This function is called by the SDL main loop when there is an event.
 * @param event the SDL event to handle
 */
void TitleScreen::handle_event(const SDL_Event &event) {

  if (current_phase == PHASE_TITLE
      && event.type == SDL_KEYDOWN
      && (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN)
      && counter >= 1
      && !transition_out->is_started()) {

    transition_out->start();
  }
}

/**
 * Initializes phase 1 of the title screen.
 * A black screen will be shown for a fraction of second.
 */
void TitleScreen::init_phase_black_screen(void) {

  // black screen during 0.3 second
  current_phase = PHASE_BLACK_SCREEN;
  next_phase_date = SDL_GetTicks() + 300;
}

/**
 * Initializes phase 2 of the title screen.
 * The message "Zelda Solarus presents" will be shown.
 */
void TitleScreen::init_phase_zs_presents(void) {

  current_phase = PHASE_ZS_PRESENTS;

  zs_presents_img = ResourceManager::load_image("menus/zelda_solarus_presents.png");
  ResourceManager::get_sound("intro")->play();
  zs_presents_position.x = 112;
  zs_presents_position.y = 96;

  next_phase_date = SDL_GetTicks() + 2000; // intro: 2 seconds
  transition_out = new TransitionFade(Transition::OUT);
}

/**
 * Exits phase 2 of the title screen.
 */
 void TitleScreen::exit_phase_zs_presents(void) {

   SDL_FreeSurface(zs_presents_img);
   delete transition_out;
 }

/**
 * Initializes phase 3 of the title screen.
 * The title screen will be shown.
 */
void TitleScreen::init_phase_title(void) {

  current_phase = PHASE_TITLE;

  title_screen_music = ResourceManager::get_music("title_screen_full.it");
  title_screen_music->play();

  background_img = ResourceManager::load_image("menus/title_background.png");
  logo_img = ResourceManager::load_image("menus/title_logo.png");
  dx_img = ResourceManager::load_image("menus/title_dx.png");
  website_img = ResourceManager::load_image("menus/title_website.png");
  press_space_img = ResourceManager::load_image("menus/title_press_space.png");
  title_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 32, 0, 0, 0, 0);

  counter = 0;
  next_image_date = SDL_GetTicks() + 5000;

  transition_in = new TransitionFade(Transition::IN);
  transition_in->set_delay(60);
  transition_in->start();
  transition_out = new TransitionFade(Transition::OUT);
}

/**
 * Exits phase 3 of the title screen.
 */
void TitleScreen::exit_phase_title(void) {

  delete transition_in;
  delete transition_out;
  SDL_FreeSurface(background_img);
  SDL_FreeSurface(logo_img);
  SDL_FreeSurface(dx_img);
  SDL_FreeSurface(website_img);
  SDL_FreeSurface(press_space_img);
  SDL_FreeSurface(title_surface);
  title_screen_music->stop();
}

/**
 * Updates phase 3 of the title screen.
 */
void TitleScreen::update_phase_title(void) {

  Uint32 now = SDL_GetTicks();

  if (now >= next_image_date) {

    if (counter == 0) {
      ResourceManager::get_sound("ok")->play();
      next_image_date = now + 1000;
      counter++;
    }
    else if (counter == 1) {
      next_image_date = now + 1000;
      counter++;
    }
    else {
      counter = 5 - counter;
      next_image_date = now + 500;
    }
  }

  if (transition_out->is_over()) {
    exit_phase_title();
    set_next_screen(new SelectionMenuSelectFile(NULL));
  }
}

/**
 * Displays phase 3 of the title screen.
 * @param destination surface the surface to draw
 */
void TitleScreen::display_phase_title(SDL_Surface *destination_surface) {

  SDL_FillRect(title_surface, NULL, Color::black);

  SDL_BlitSurface(background_img, NULL, title_surface, NULL);
  SDL_BlitSurface(website_img, NULL, title_surface, NULL);

  if (counter >= 1) {
    SDL_BlitSurface(logo_img, NULL, title_surface, NULL);

    if (counter >= 2) {
      SDL_BlitSurface(dx_img, NULL, title_surface, NULL);

      if (counter >= 3) {
	SDL_BlitSurface(press_space_img, NULL, title_surface, NULL);
      }
    }
  }

  if (transition_in->is_started()) {
    transition_in->display(title_surface);
  }
  else if (transition_out->is_started()) {
    transition_out->display(title_surface);
  }

  SDL_BlitSurface(title_surface, NULL, destination_surface, NULL);
}
