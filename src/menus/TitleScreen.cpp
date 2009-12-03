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
#include "lowlevel/Music.h"
#include "lowlevel/Sound.h"
#include "ResourceManager.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Color.h"
#include "TransitionFade.h"
#include "lowlevel/TextSurface.h"
#include "StringResource.h"
#include <ctime>

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
 * @param destination_surface the surface to draw
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
  zs_presents_position.x = 160 - (zs_presents_img->w / 2);
  zs_presents_position.y = 120 - (zs_presents_img->h / 2);

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

  static const std::string time_of_day_strings[] = { "daylight", "sunset", "night" };
  static const SDL_Color text_colors[] = { {0, 0, 92}, {0, 0, 92}, {255, 128, 0} };
  TimeOfDay time_of_day = get_time_of_day();
//  time_of_day = TimeOfDay(2);

  current_phase = PHASE_TITLE;

  title_screen_music = ResourceManager::get_music("title_screen.spc");
  title_screen_music->play();

  const std::string &time_of_day_name = time_of_day_strings[time_of_day];
  std::string background_img_name = (std::string) "menus/title_" + time_of_day_name + "_background.png";
  std::string clouds_img_name = (std::string) "menus/title_" + time_of_day_name + "_clouds.png";

  background_img = ResourceManager::load_image(background_img_name);
  clouds_img = ResourceManager::load_image(clouds_img_name);
  logo_img = ResourceManager::load_image("menus/title_logo.png");
  dx_img = ResourceManager::load_image("menus/title_dx.png");
  star_img = ResourceManager::load_image("menus/title_star.png");

  website_img = new TextSurface(160, 220, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_MIDDLE);
  website_img->set_font(TextSurface::FONT_LA);
  website_img->set_rendering_mode(TextSurface::TEXT_BLENDED);
  website_img->set_text_color(text_colors[time_of_day]);
  website_img->set_text(StringResource::get_string("title_screen.website"));
  press_space_img = new TextSurface(160, 190, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_MIDDLE);
  press_space_img->set_font(TextSurface::FONT_LA_BIG);
  press_space_img->set_rendering_mode(TextSurface::TEXT_BLENDED);
  press_space_img->set_text_color(text_colors[time_of_day]);
  press_space_img->set_text(StringResource::get_string("title_screen.press_space"));
  title_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 32, 0, 0, 0, 0);

  clouds_position.x = 320;
  clouds_position.y = 30;
  Uint32 now = SDL_GetTicks();
  next_clouds_move_date = now;

  counter = 0;
  next_image_date = SDL_GetTicks() + 5000;

  transition_in = new TransitionFade(Transition::IN);
  transition_in->set_delay(30);
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
  SDL_FreeSurface(clouds_img);
  SDL_FreeSurface(logo_img);
  SDL_FreeSurface(dx_img);
  SDL_FreeSurface(star_img);
  delete website_img;
  delete press_space_img;
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

  while (now >= next_clouds_move_date) {
    clouds_position.x += 1;
    clouds_position.y -= 1;
    if (clouds_position.x >= 535) {
      clouds_position.x -= 535;
    }
    if (clouds_position.y < 0) {
      clouds_position.y += 299;
    }
    next_clouds_move_date = now + 50;
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

  // fill with black
  SDL_FillRect(title_surface, NULL, Color::black);

  // background
  SDL_BlitSurface(background_img, NULL, title_surface, NULL);

  // clouds
  SDL_Rect clouds_dst_position = clouds_position;
  SDL_BlitSurface(clouds_img, NULL, title_surface, &clouds_dst_position);
  clouds_dst_position.x = clouds_position.x - 535;
  clouds_dst_position.y = clouds_position.y;
  SDL_BlitSurface(clouds_img, NULL, title_surface, &clouds_dst_position);
  clouds_dst_position.x = clouds_position.x;
  clouds_dst_position.y = clouds_position.y - 299;
  SDL_BlitSurface(clouds_img, NULL, title_surface, &clouds_dst_position);
  clouds_dst_position.x = clouds_position.x - 535;
  clouds_dst_position.y = clouds_position.y - 299;
  SDL_BlitSurface(clouds_img, NULL, title_surface, &clouds_dst_position);

  // website name
  website_img->display(title_surface);

  SDL_BlitSurface(logo_img, NULL, title_surface, NULL);

  // logo and other appearing stuff
  if (counter >= 1) {
    SDL_BlitSurface(dx_img, NULL, title_surface, NULL);

    if (counter >= 2) {
      SDL_BlitSurface(star_img, NULL, title_surface, NULL);

      if (counter >= 3) {
	press_space_img->display(title_surface);
      }
    }
  }

  // transitions
  if (transition_in->is_started()) {
    transition_in->display(title_surface);
  }
  else if (transition_out->is_started()) {
    transition_out->display(title_surface);
  }

  SDL_BlitSurface(title_surface, NULL, destination_surface, NULL);
}

/**
 * Returns a number describing the current time of the day.
 * @return DAYLIGHT, SUNSET or NIGHT
 */
TitleScreen::TimeOfDay TitleScreen::get_time_of_day(void) {

  time_t timestamp = time(NULL);
  struct tm *local_time = localtime(&timestamp);
  int hour = local_time->tm_hour;

  TimeOfDay result;
  if (hour >= 8 && hour <= 18) {
    result = DAYLIGHT;
  }
  else if (hour > 18 && hour <= 20) {
    result = SUNSET;
  }
  else {
    result = NIGHT;
  }
  return result;
}

