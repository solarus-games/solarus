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
#include "menus/TitleScreen.h"
#include "menus/SelectionMenu.h"
#include "TransitionFade.h"
#include "ResourceManager.h"
#include "StringResource.h"
#include "lowlevel/Music.h"
#include "lowlevel/Sound.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Color.h"
#include "lowlevel/TextSurface.h"
#include "lowlevel/Surface.h"
#include "lowlevel/System.h"
#include <ctime>

/**
 * Creates a title screen.
 * @param solarus the Solarus object
 */
TitleScreen::TitleScreen(Solarus *solarus):
  Screen(solarus) {

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
      exit_phase_introduction_message();
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

  uint32_t now = System::now();

  switch (current_phase) {

  case PHASE_BLACK_SCREEN:

    if (now >= next_phase_date) {
      // go to next phase after 0.3 second
      init_phase_introduction_message();
    }

    break;

  case PHASE_ZS_PRESENTS:

    if (now >= next_phase_date) {

      if (transition_out->is_finished()) {

	// unload current phase
	exit_phase_introduction_message();

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
void TitleScreen::display(Surface *destination_surface) {

  switch (current_phase) {

  case PHASE_BLACK_SCREEN:
    // nothing to display
    break;

  case PHASE_ZS_PRESENTS:
    if (transition_out->is_started()) { // out transition
      transition_out->display(introduction_message_img);
    }
    introduction_message_img->blit(destination_surface, introduction_message_position);
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
      && !transition_out->is_started()
      && !transition_out->is_finished()) {

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
  next_phase_date = System::now() + 300;
}

/**
 * Initializes phase 2 of the title screen.
 * A introduction message will be shown.
 */
void TitleScreen::init_phase_introduction_message(void) {

  current_phase = PHASE_ZS_PRESENTS;

  introduction_message_img = ResourceManager::load_image("menus/zelda_solarus_presents.png");
  ResourceManager::get_sound("intro")->play();
  introduction_message_position.set_x(160 - (introduction_message_img->get_width() / 2));
  introduction_message_position.set_y(120 - (introduction_message_img->get_height() / 2));

  next_phase_date = System::now() + 2000; // intro: 2 seconds
  transition_out = new TransitionFade(Transition::OUT);
}

/**
 * Exits phase 2 of the title screen.
 */
void TitleScreen::exit_phase_introduction_message(void) {
  delete introduction_message_img;
  delete transition_out;
}

/**
 * Initializes phase 3 of the title screen.
 * The title screen will be shown.
 */
void TitleScreen::init_phase_title(void) {

  static const std::string time_of_day_strings[] = { "daylight", "sunset", "night" };
  static Color text_colors[] = { Color(0, 0, 92), Color(0, 0, 92), Color(255, 128, 0) };
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
  title_surface = new Surface(320, 240);

  clouds_position.set_xy(320, 30);
  uint32_t now = System::now();
  next_clouds_move_date = now;

  counter = 0;
  next_image_date = System::now() + 5000;

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
  delete background_img;
  delete clouds_img;
  delete logo_img;
  delete dx_img;
  delete star_img;
  delete website_img;
  delete press_space_img;
  delete title_surface;
  title_screen_music->stop();
}

/**
 * Updates phase 3 of the title screen.
 */
void TitleScreen::update_phase_title(void) {

  uint32_t now = System::now();

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
    clouds_position.add_x(1);
    clouds_position.add_y(-1);
    if (clouds_position.get_x() >= 535) {
      clouds_position.add_x(-535);
    }
    if (clouds_position.get_y() < 0) {
      clouds_position.add_y(299);
    }
    next_clouds_move_date = now + 50;
  }

  if (transition_out->is_finished()) {
    exit_phase_title();
    set_next_screen(new SelectionMenu(solarus)); 
  }
}

/**
 * Displays phase 3 of the title screen.
 * @param destination surface the surface to draw
 */
void TitleScreen::display_phase_title(Surface *destination_surface) {

  // fill with black
  title_surface->fill_with_color(Color::get_black());

  // background
  background_img->blit(title_surface);

  // clouds
  Rectangle clouds_dst_position = clouds_position;
  clouds_img->blit(title_surface, clouds_dst_position);
  clouds_dst_position.set_xy(clouds_position.get_x() - 535, clouds_position.get_y());
  clouds_img->blit(title_surface, clouds_dst_position);
  clouds_dst_position.set_xy(clouds_position.get_x(), clouds_position.get_y() - 299);
  clouds_img->blit(title_surface, clouds_dst_position);
  clouds_dst_position.set_xy(clouds_position.get_x() - 535, clouds_position.get_y() - 299);
  clouds_img->blit(title_surface, clouds_dst_position);

  // website name and logo
  website_img->display(title_surface);
  logo_img->blit(title_surface);

  // logo and other appearing stuff
  if (counter >= 1) {
    dx_img->blit(title_surface);

    if (counter >= 2) {
      star_img->blit(title_surface);

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

  title_surface->blit(destination_surface);
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

