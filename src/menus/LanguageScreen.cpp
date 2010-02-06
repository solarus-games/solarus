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
#include "menus/LanguageScreen.h"
#include "menus/TitleScreen.h"
#include "Transition.h"
#include "ResourceManager.h"
#include "lowlevel/Sound.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Surface.h"
#include "lowlevel/TextSurface.h"

/**
 * Creates a language screen.
 * @param solarus the Solarus object
 */
LanguageScreen::LanguageScreen(Solarus *solarus):
  Screen(solarus) {

  if (FileTools::get_language().size() != 0) {
    // a language is already set: skip this screen
    finished = true;
  }
  else {
    // transition = Transition::create(Transition::FADE, Transition::IN);
    // transition->start();
    transition = NULL;
    intermediate_surface = new Surface(320, 240);

    std::map<std::string, std::string> language_map = FileTools::get_languages();
    nb_languages = language_map.size();
    first_visible_language = 0;
    language_texts = new TextSurface*[nb_languages];
    language_codes = new std::string[nb_languages];
    int cursor_position = 0;
    int i = 0;
    std::map<std::string, std::string>::iterator it;
    for (it = language_map.begin(); it != language_map.end(); it++) {
      language_codes[i] = it->first;
      language_texts[i] = new TextSurface(160, 0, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_MIDDLE);
      language_texts[i]->set_font(TextSurface::FONT_STANDARD);
      language_texts[i]->set_text(it->second);
      if (language_codes[i] == FileTools::get_default_language()) {
	cursor_position = i;
      }
      i++;
    }
    set_cursor_position(cursor_position);
  }
}

/**
 * Destroys the language screen.
 */
LanguageScreen::~LanguageScreen(void) {

  delete transition;
}

/**
 * Sets the position of the cursor.
 * @param cursor_position
 */
void LanguageScreen::set_cursor_position(int cursor_position) {

  const int max_visible_languages = 10;
  const int nb_visible_languages = std::min(nb_languages, max_visible_languages);

  language_texts[this->cursor_position]->set_text_color(Color::get_white());
  language_texts[cursor_position]->set_text_color(Color::get_yellow());

  if (cursor_position < first_visible_language) {
    first_visible_language = cursor_position;
  }
  if (cursor_position >= first_visible_language + max_visible_languages) {
    first_visible_language = cursor_position - max_visible_languages + 1;
  }

  int y = 120 - 8 * nb_visible_languages;
  for (int i = 0; i < nb_languages; i++) {
    language_texts[i]->set_y(y);
    y += 16;
  }

  this->cursor_position = cursor_position;
}

/**
 * Updates this screen.
 */
void LanguageScreen::update(void) {

  if (finished) {
    set_next_screen(new TitleScreen(solarus));
  }

  if (transition != NULL) {
    transition->update();
    if (transition->is_finished()) {
      delete transition;
      transition = NULL;

      if (FileTools::get_language().size() != 0) {
        set_next_screen(new TitleScreen(solarus));
      }
    }
  }
}

/**
 * Displays this screen.
 * @param destination_surface the surface to draw
 */
void LanguageScreen::display(Surface *destination_surface) {

  intermediate_surface->fill_with_color(Color::get_black());

  for (int i = 0; i < nb_languages; i++) {
    language_texts[i]->display(intermediate_surface);
  }

  if (transition != NULL) {
    transition->display(intermediate_surface);
  }

  intermediate_surface->blit(destination_surface);
}

/**
 * Handles an event.
 * This function is called by the main loop when there is an event.
 * @param event the event to handle
 */
void LanguageScreen::handle_event(const SDL_Event &event) {

  if (event.type == SDL_KEYDOWN && transition == NULL) {

    switch (event.key.keysym.sym) {
      
      case SDLK_UP:
	set_cursor_position((cursor_position - 1 + nb_languages) % nb_languages);
	ResourceManager::get_sound("cursor")->play();
	break;
    
      case SDLK_DOWN:
	set_cursor_position((cursor_position + 1) % nb_languages);
	ResourceManager::get_sound("cursor")->play();
	break;

      case SDLK_SPACE:
      case SDLK_RETURN:
	FileTools::set_language(language_codes[cursor_position]);
	transition = Transition::create(Transition::FADE, Transition::OUT);
	transition->start();
	break;

      default:
	break;
    }
  }
}

