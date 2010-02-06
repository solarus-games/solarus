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
#ifndef SOLARUS_LANGUAGE_SCREEN_H
#define SOLARUS_LANGUAGE_SCREEN_H

#include "Common.h"
#include "Screen.h"

/**
 * This class shows a screen to let the user choose his language.
 * The screen is displayed only the first time the program is launched,
 * that is, when the configuration file does not exist or does not contains
 * the language setting.
 */
class LanguageScreen: public Screen {

  private:

    Transition *transition;
    Surface *intermediate_surface;

    std::string *language_codes;
    TextSurface **language_texts;
    int cursor_position;
    int first_visible_language;
    int nb_languages;
    bool finished;

    void set_cursor_position(int cursor_position);

  public:

    // creation and destruction
    LanguageScreen(Solarus *solarus);
    ~LanguageScreen(void);

    // update and display
    void handle_event(const SDL_Event &event);
    void update(void);
    void display(Surface *destination_surface);
};

#endif

