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
#include "menus/SelectionMenuChooseMode.h"
#include "Transition.h"
#include "ResourceManager.h"
#include "Savegame.h"
#include "Game.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Rectangle.h"

/**
 * Creates a selection menu with the phase where the
 * player has to choose the game mode.
 * @param previous the previous phase
 */
SelectionMenuChooseMode::SelectionMenuChooseMode(SelectionMenuPhase *previous):
//  SelectionMenuPhase(previous, "selection_menu.choose_mode"),
  SelectionMenuPhase(previous, "selection_menu.select_file"),
  adventure_mode(true) {

  this->img_mode = ResourceManager::load_image("menus/selection_menu_mode.png");
  this->savegame_surface = new Surface(320, 240);

  transition = Transition::create(Transition::FADE, Transition::OUT);
  transition->start();
}

/**
 * Destructor.
 */
SelectionMenuChooseMode::~SelectionMenuChooseMode(void) {
  delete img_mode;
  delete savegame_surface;
}

/**
 * Handles an event in this phase.
 * @param event the event
 */
void SelectionMenuChooseMode::handle_event(const SDL_Event &event) {
/*
  if (transition == NULL && event.type == SDL_KEYDOWN) {

    bool finished = false;

    switch (event.key.keysym.sym) {

    case SDLK_SPACE:
    case SDLK_RETURN:
      play_ok_sound();
      finished = true;
      break;

    case SDLK_RIGHT:
    case SDLK_LEFT:
      play_cursor_sound();
      adventure_mode = !adventure_mode;
      break;

    default:
      break;
    }

    if (finished) {
      transition = Transition::create(Transition::FADE, Transition::OUT);
      transition->start();
    }
  }
  */
}

/**
 * Updates the selection menu in this phase.
 */
void SelectionMenuChooseMode::update(void) {

  if (transition != NULL && transition->is_over()) {

    Savegame *savegame = get_savegame(get_cursor_position() - 1);
    Savegame *savegame_copy = new Savegame(savegame->get_file_name());
    // because the first one will be deleted

    set_next_screen(new Game(savegame_copy));
  }

  SelectionMenuPhase::update();
}

/**
 * Displays the selection menu in this phase.
 */
void SelectionMenuChooseMode::display(Surface *screen_surface) {

  start_display(screen_surface);

  // the savegame
  int i = get_cursor_position() - 1;
  display_savegame(i, savegame_surface);
  display_savegame_number(i, savegame_surface);

  // move the savegame to the top
  Rectangle savegame_position(57, 75 + i * 27, 208, 23);
  Rectangle position(57, 75 + i * 27);
//  Rectangle position = {57, 75};

  savegame_surface->blit(savegame_position, destination_surface, position);

  /*
  // options
  Rectangle box_position(0, 0, 73, 54);
  if (adventure_mode) {
    box_position.set_y(54); // highlight the selection
  }

  position.set_xy(70, 115);
  img_mode->blit(box_position, destination_surface, position);

  box_position.set_xy(73, adventure_mode ? 0 : 54); // highlight the selection
  position.set_xy(170, 115);
  img_mode->blit(box_position, destination_surface, position);
*/

  finish_display(screen_surface);
}

