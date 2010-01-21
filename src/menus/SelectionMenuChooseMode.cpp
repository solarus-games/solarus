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
#include "menus/SelectionMenu.h"
#include "Transition.h"
#include "ResourceManager.h"
#include "Savegame.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Rectangle.h"

/**
 * Creates a selection phase where the player has to choose the game mode.
 * @param menu the selection menu
 */
SelectionMenuChooseMode::SelectionMenuChooseMode(SelectionMenu *menu):
//  SelectionMenuPhase(menu, "selection_menu.choose_mode"),
  SelectionMenuPhase(menu, "selection_menu.select_file"),
  adventure_mode(true) {

  this->mode_img = ResourceManager::load_image("menus/selection_menu_mode.png");
  this->savegame_surface = new Surface(320, 240);

  Transition *transition = Transition::create(Transition::FADE, Transition::OUT);
  transition->start();
  menu->set_transition(transition);
}

/**
 * Destructor.
 */
SelectionMenuChooseMode::~SelectionMenuChooseMode(void) {
  delete mode_img;
  delete savegame_surface;
}

/**
 * Handles an event in this phase.
 * @param event the event
 */
void SelectionMenuChooseMode::handle_event(const SDL_Event &event) {
/*
  if (!menu->has_transition() && event.type == SDL_KEYDOWN) {

    bool finished = false;

    switch (event.key.keysym.sym) {

    case SDLK_SPACE:
    case SDLK_RETURN:
      menu->play_ok_sound();
      finished = true;
      break;

    case SDLK_RIGHT:
    case SDLK_LEFT:
      menu->play_cursor_sound();
      adventure_mode = !adventure_mode;
      break;

    default:
      break;
    }

    if (finished) {
      Transition *transition = Transition::create(Transition::FADE, Transition::OUT);
      transition->start();
      menu->set_transition(transition);
    }
  }
  */
}

/**
 * Updates the selection menu in this phase.
 */
void SelectionMenuChooseMode::update(void) {

  if (menu->is_transition_finished()) {

    Savegame *savegame = menu->get_savegame(menu->get_cursor_position() - 1);
    Savegame *savegame_copy = new Savegame(savegame->get_file_name());
    // because the first one will be deleted

    menu->start_game(savegame_copy);
  }
}

/**
 * Displays this selection menu phase.
 * @param destination_surface the surface to draw
 */
void SelectionMenuChooseMode::display(Surface *destination_surface) {

  // the savegame
  int i = menu->get_cursor_position() - 1;
  menu->display_savegame(savegame_surface, i);
  menu->display_savegame_number(savegame_surface, i);

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
  mode_img->blit(box_position, destination_surface, position);

  box_position.set_xy(73, adventure_mode ? 0 : 54); // highlight the selection
  position.set_xy(170, 115);
  mode_img->blit(box_position, destination_surface, position);
*/
}

