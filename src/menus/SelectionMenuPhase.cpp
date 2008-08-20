#include "menus/SelectionMenuPhase.h"
#include "menus/SelectionMenuCommon.h"
#include "TransitionEffect.h"
#include "Sound.h"
#include "Sprite.h"
#include "TextDisplayed.h"
#include "HeartsView.h"

/**
 * Creates a selection menu phase.
 * @param previous the previous phase (if we were already in
 * the selection menu), or NULL
 * @param title_string the title text to display in this phase
 */
SelectionMenuPhase::SelectionMenuPhase(SelectionMenuPhase *previous,
				       string title_string):
  Screen() {

  if (previous != NULL) {
    this->common_part = previous->common_part;
  }
  else {
    this->common_part = new SelectionMenuCommon();
  }

  this->title_string = title_string;
  this->destination_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 32, 0, 0, 0, 0);
  this->transition = NULL;
}

/**
 * Destroys the selection menu phase.
 * If there was a transition, it is destroyed.
 */
SelectionMenuPhase::~SelectionMenuPhase(void) {

  SDL_FreeSurface(destination_surface);

  if (transition != NULL) {
    delete transition;
  }

  if (get_next_screen() == NULL) {
    delete common_part;
  }
}

/**
 * Returns the savegame selected by the user.
 * The show() function must have been called first.
 * The savegame returned is not freed when the menu is freed.
 * @return the savegame selected by the user, or NULL
 * if he wants to quit.
 */
/*
Savegame * SelectionMenuPhase::get_selected_save(void) {

  if (cursor_position <= 3 && !zsdx->is_exiting()) {

    char file_name[10];
    sprintf(file_name, "save%d.zsd", cursor_position);
    return new Savegame(file_name);
  }
  
  return NULL;
}
*/

/**
 * Updates the selection menu in this phase.
 * The subclasses should call this function to
 * update the elements common to all phases of
 * the selection menu.
 */
void SelectionMenuPhase::update(void) {

  // update the elements common to all phases
  common_part->update();
}

/**
 * Starts to display the selection menu in this phase.
 * The subclasses should call this function to
 * display the elements common to all the phases of
 * the selection menu, then display their own elements,
 * and finally call finish_display().
 * @param screen_surface surface where this screen has to be drawn
 */
void SelectionMenuPhase::start_display(SDL_Surface *screen_surface) {

  // display the elements common to all phases
  common_part->display(destination_surface);

  // title
  display_title_text();
}

/**
 * Validates the displaying of the selection menu in this phase.
 * The subclasses should call display() to
 * display the elements common to all the phases of
 * the selection menu, then display their own elements,
 * and finally call finish_display().
 * @param screen_surface surface where this screen has to be drawn
 */
void SelectionMenuPhase::finish_display(SDL_Surface *screen_surface) {

  // transition
  if (transition != NULL && transition->is_started()) {
    transition->display(destination_surface);
  }

  SDL_BlitSurface(destination_surface, NULL, screen_surface, NULL);
}

/**
 * Plays the "ok" sound.
 */
void SelectionMenuPhase::play_ok_sound(void) {
  common_part->ok_sound->play();
}

/**
 * Returns the 3 savegames.
 * @return the 3 savegames
 */
Savegame ** SelectionMenuPhase::get_savegames(void) {
  return common_part->savegames;
}

/**
 * Returns the cursor's sprite.
 * @return the cursor's sprite
 */
Sprite * SelectionMenuPhase::get_cursor(void) {
  return common_part->cursor;
}

/**
 * Returns the current position of the cursor.
 * @return the current position of the cursor
 */
int SelectionMenuPhase::get_cursor_position(void) {
  return common_part->cursor_position;
}

/**
 * Sets the position of the cursor.
 * @param position the new position of the cursor
 */
void SelectionMenuPhase::set_cursor_position(int position) {
  common_part->cursor_position = position;
}

/**
 * This function is called when the user wants to move the cursor
 * upwards (except for the letter cursor).
 */
void SelectionMenuPhase::move_cursor_up(void) {

  common_part->cursor_sound->play();
  common_part->cursor->restart_animation();
  common_part->cursor_position--;

  if (common_part->cursor_position == 0) {
    common_part->cursor_position = 4;
  }
  else if (common_part->cursor_position == 4) {
    common_part->cursor_position = 3;
  }
}

/**
 * This function is called when the user wants to move the cursor
 * downwards (except for the letter cursor).
 */
void SelectionMenuPhase::move_cursor_down(void) {

  common_part->cursor_sound->play();
  common_part->cursor->restart_animation();
  common_part->cursor_position++;

  if (common_part->cursor_position >= 5) {
    common_part->cursor_position = 1;
  }
}

/**
 * This function is called when the user wants to move the cursor
 * to the left or to the right (except for the letter cursor).
 */
void SelectionMenuPhase::move_cursor_left_or_right(void) {

  if (common_part->cursor_position == 4) {
    common_part->cursor_sound->play();
    common_part->cursor->restart_animation();
    common_part->cursor_position = 5;
  }
  else if (common_part->cursor_position == 5) {
    common_part->cursor_sound->play();
    common_part->cursor->restart_animation();
    common_part->cursor_position = 4;
  }
}

/**
 * Draws a title on the selection menu background.
 * @param text the title to add
 */
void SelectionMenuPhase::display_title_text(void) {
  
  common_part->text_title->set_text(title_string);
  common_part->text_title->display(destination_surface);
}

/**
 * Displays a savegame on the surface.
 * @param save_number number of the savegame to display (0 to 2)
 */
void SelectionMenuPhase::display_savegame(int save_number) {

  SDL_Rect position;

  // draw the container
  position.x = 57;
  position.y = 75 + save_number * 27;
  SDL_BlitSurface(common_part->img_save_container, NULL, destination_surface, &position);

  // draw the player's name
  common_part->text_player_names[save_number]->display(destination_surface);

  // draw the hearts
  common_part->hearts_views[save_number]->display(destination_surface);
}

/**
 * Displays the number of a savegame.
 * This function is separate from the display_savegame() function
 * because the cursor has to be displayed after the savegame images
 * but before the savegame number.
 * @param save_number number to display (0 to 2)
 */
void SelectionMenuPhase::display_savegame_number(int save_number) {

  SDL_Rect position;

  position.x = 62;
  position.y = 80 + 27 * save_number;
  SDL_BlitSurface(common_part->img_numbers[save_number], NULL, destination_surface, &position);
}

/**
 * Displays the two options in the bottom of the screen.
 * @param left_string text of the first option (on the left)
 * @param right_string text of the second option (on the right)
 */
void SelectionMenuPhase::display_options(const char *left_string, const char *right_string) {

  SDL_Rect position;

  position.x = 57;
  position.y = 158;
  SDL_BlitSurface(common_part->img_option_container, NULL, destination_surface, &position);
  position.x = 165;
  SDL_BlitSurface(common_part->img_option_container, NULL, destination_surface, &position);

  common_part->text_option1->set_text(left_string);
  common_part->text_option1->display(destination_surface);

  common_part->text_option2->set_text(right_string);
  common_part->text_option2->display(destination_surface);

}

/**
 * Displays the normal cursor (i.e. the cursor to select a file)
 * at its current position.
 */
void SelectionMenuPhase::display_normal_cursor(void) {

  SDL_Rect position;

  if (common_part->cursor_position != 5) {
    position.x = 58;
  }
  else {
    position.x = 166;
  }

  if (common_part->cursor_position < 4) {
    position.y = 49 + common_part->cursor_position * 27;
  }
  else {
    position.y = 159;
  }
  common_part->cursor->display(destination_surface, position.x, position.y);
}

/**
 * Displays the "Which file do you want to erase?" phase.
 *
void SelectionMenuPhase::start_erase_file_phase(void) {

  current_phase = ERASE_FILE;

  cursor->set_current_animation("red");

  bool finished = false;
  SDL_Event event;

  Uint32 next_display = SDL_GetTicks();
  while (!zsdx->is_exiting() && !finished) {

    // if there is an event
    if (SDL_PollEvent(&event)) {
      
      zsdx->handle_event(event);
      
      if (event.type == SDL_KEYDOWN) {

	switch (event.key.keysym.sym) {

	case SDLK_SPACE:
	  if (cursor_position == 5) {
	    // the user chose "Quit"
	    zsdx->set_exiting();
	  }
	  else if (cursor_position == 4) {
	    // the user chose "Cancel"
	    ok_sound->play();
	    finished = true;
	  }
	  else {

	    save_number_to_erase = cursor_position - 1;
	    if (savegames[save_number_to_erase]->is_empty()) {
	      // the savegame doesn't exist: error sound
	      error_sound->play();
	    }
	    else {
	      // the savegame exists: confirm deleting it
	      ok_sound->play();
	      start_confirm_erase_phase();
	      current_phase = ERASE_FILE;
	      finished = true;
	      continue;
	    }
	  }
	  break;

	case SDLK_DOWN:
	  move_cursor_down();
	  break;

	case SDLK_UP:
	  move_cursor_up();
	  break;

	case SDLK_RIGHT:
	case SDLK_LEFT:
	  move_cursor_left_or_right();
	  break;

	default:
	  break;
	}
      }
    }

    // update the sprites
    update();

    // redraw if necessary
    while (SDL_GetTicks() >= next_display) {
      display_erase_file_phase();
      next_display = SDL_GetTicks() + FRAME_INTERVAL;
    }
  }

  cursor->set_current_animation("blue");
}
*/

/**
 * Displays the "Which file do you want to erase?" phase.
 *
void SelectionMenuPhase::display_erase_file_phase(void) {

  display_common();

  // savegames
  for (int i = 0; i < 3; i++) {
    display_savegame(i);
  }

  // options
  display_options("Annuler", "Quitter");

  // cursor
  display_normal_cursor();
 
  // save numbers
  for (int i = 0; i < 3; i++) {
    display_savegame_number(i);
  }

  // blit everything
  SDL_BlitSurface(destination_surface, NULL, zsdx->screen, NULL);
  SDL_Flip(zsdx->screen);
}
*/

/**
 * Displays the "Are you sure?" phase.
 *
void SelectionMenuPhase::start_confirm_erase_phase(void) {

  current_phase = CONFIRM_ERASE;

  bool finished = false;
  SDL_Event event;

  cursor_position = 4; // select "no" by default

  Uint32 next_display = SDL_GetTicks();
  while (!zsdx->is_exiting() && !finished) {

    // if there is an event
    if (SDL_PollEvent(&event)) {
      
      zsdx->handle_event(event);
      
      if (event.type == SDL_KEYDOWN) {

	switch (event.key.keysym.sym) {

	case SDLK_SPACE:
	  if (cursor_position == 5) {
	    // the user chose "Yes"
	    erase_sound->play();
	    delete_save_file(save_number_to_erase);
	    cursor_position = save_number_to_erase + 1;
	    finished = true;
	  }
	  else if (cursor_position == 4) {
	    // the user chose "No"
	    ok_sound->play();
	    finished = true;
	  }
	  break;

	case SDLK_RIGHT:
	case SDLK_LEFT:
	  move_cursor_left_or_right();
	  break;

	default:
	  break;
	}
      }
    }

    // update the sprites
    update();

    // redraw if necessary
    while (SDL_GetTicks() >= next_display) {
      display_confirm_erase_phase();
      next_display = SDL_GetTicks() + FRAME_INTERVAL;
    }
  }
}
*/

/**
 * Displays the "Are you sure?" phase.
 *
void SelectionMenuPhase::display_confirm_erase_phase(void) {

  display_common();

  // savegame
  display_savegame(save_number_to_erase);
  display_savegame_number(save_number_to_erase);

  // options
  display_options("NON", "OUI");

  // cursor
  display_normal_cursor();
 
  // blit everything
  SDL_BlitSurface(destination_surface, NULL, zsdx->screen, NULL);
  SDL_Flip(zsdx->screen);
}
*/

/**
 * Deletes a save file.
 * @param save_number number of the savegame to delete (0 to 2)
 *
void SelectionMenuPhase::delete_save_file(int save_number) {
  
  remove(savegames[save_number]->get_file_name());
  read_saves();
}
*/

/**
 * Displays the "What is your name?" phase.
 *
void SelectionMenuPhase::start_choose_name_phase(void) {

  current_phase = CHOOSE_NAME;

  player_name[0] = '\0';
  text_new_player_name->set_text(player_name);

  keys_effect->set_sword_key_enabled(true);
  cursor->set_current_animation("letters");

  x_letter_cursor = 0;
  y_letter_cursor = 0;

  bool finished = false;
  SDL_Event event;

  Uint32 next_display = SDL_GetTicks();
  while (!zsdx->is_exiting() && !finished) {

    // if there is an event
    if (SDL_PollEvent(&event)) {
      
      zsdx->handle_event(event);
    
      if (event.type == SDL_KEYDOWN) {

	switch (event.key.keysym.sym) {

	case SDLK_RETURN:
	  // directly validate the name
	  finished = validate_player_name();
	  break;

	case SDLK_c:
	case SDLK_SPACE:
	  // choose a letter
	  finished = select_letter();
	  text_new_player_name->set_text(player_name);
	  break;

	case SDLK_RIGHT:
	  cursor_sound->play();
	  x_letter_cursor = (x_letter_cursor + 14) % 13;
	  break;
	  
	case SDLK_UP:
	  cursor_sound->play();
	  y_letter_cursor = (y_letter_cursor + 4) % 5;
	  break;
	  
	case SDLK_LEFT:
	  cursor_sound->play();
	  x_letter_cursor = (x_letter_cursor + 12) % 13;
	  break;
	  
	case SDLK_DOWN:
	  cursor_sound->play();
	  y_letter_cursor = (y_letter_cursor + 6) % 5;
	  break;
	  
	default:
	  break;
	}
      }
    }

    // update the sprites
    update();

    // redraw if necessary
    while (SDL_GetTicks() >= next_display) {
      display_choose_name_phase();
      next_display = SDL_GetTicks() + FRAME_INTERVAL;
    }
  }

  cursor->set_current_animation("blue");
  keys_effect->set_sword_key_enabled(false);
}
*/

/**
 * Displays the "What is your name?" phase.
 *
void SelectionMenuPhase::display_choose_name_phase(void) {

  display_common();

  // cursor
  cursor->display(destination_surface,
		  51 + 16 * x_letter_cursor,
		  93 + 18 * y_letter_cursor);
 
  // current name
  SDL_Rect position = {57, 76, 0, 0};
  SDL_BlitSurface(img_arrow, NULL, destination_surface, &position);
  text_new_player_name->display(destination_surface);

  // letters
  position.y = 98;
  SDL_BlitSurface(img_letters, NULL, destination_surface, &position);

  // blit everything
  SDL_BlitSurface(destination_surface, NULL, zsdx->screen, NULL);
  SDL_Flip(zsdx->screen);
}
*/

/**
 * This function is called when the player chooses a letter
 * when typing his name.
 * @return true if he finished typing the name (because he validated
 * or cancelled), false otherwise
 *
bool SelectionMenuPhase::select_letter(void) {

  size_t length = strlen(player_name);
  char letter_to_add = '\0';
  bool finished = false;

  switch (y_letter_cursor) {
    
  case 0:
    // upper case letter from A to M
    letter_to_add = 'A' + x_letter_cursor;
    break;
    
  case 1:
    // upper case letter from N to Z
    letter_to_add = 'N' + x_letter_cursor;
    break;

  case 2:
    // lower case letter from a to m
    letter_to_add = 'a' + x_letter_cursor;
    break;
    
  case 3:
    // upper case letter from n to z
    letter_to_add = 'n' + x_letter_cursor;
    break;

  case 4:
    // digit or special command

    if (x_letter_cursor <= 9) {
      letter_to_add = '0' + x_letter_cursor;
    }
    else {
      switch (x_letter_cursor) {

      case 10:
	// remove the last letter
	if (length > 0) {
	  player_name[length - 1] = '\0';
	  letter_sound->play();
	}
	else {
	  error_sound->play();
	}
	break;

      case 11:
	// validate the choice
	finished = validate_player_name();
	break;

      case 12:
	// cancel
	letter_sound->play();
	finished = true;
	break;
      }
    }
    break;
  }

  if (letter_to_add != '\0') {
    // a letter was selected
    if (length < 10) {
      player_name[length] = letter_to_add;
      player_name[length + 1] = '\0';
      letter_sound->play();
    }
    else {
      error_sound->play();
    }
  }  

  return finished;
}
*/

/**
 * This function is called when the player wants to choose a letter
 * when typing his name.
 * @return true if the new name is validated, false otherwise
 *
bool SelectionMenuPhase::validate_player_name(void) {

  if (strlen(player_name) == 0) {
    error_sound->play();
    return false;
  }

  ok_sound->play();

  savegames[cursor_position - 1]->set_reserved_string(SAVEGAME_PLAYER_NAME, player_name);
  savegames[cursor_position - 1]->save();
  read_saves();

  return true;
}
*/

/**
 * Displays the "Choose your game mode" phase.
 *
void SelectionMenuPhase::start_choose_mode_phase(void) {

  current_phase = CHOOSE_MODE;

  // move the savegame elements to the top
  int save_number = cursor_position - 1;
  delete hearts_views[save_number];
  hearts_views[save_number] =
    new HeartsView(savegames[save_number]->get_equipment(), 168, 78);

  text_player_names[save_number]->set_position(87, 88);

  bool finished = false;
  SDL_Event event;

  Uint32 next_display = SDL_GetTicks();
  while (!zsdx->is_exiting() && !finished) {

    // if there is an event
    if (SDL_PollEvent(&event)) {
      
      zsdx->handle_event(event);
      
      if (event.type == SDL_KEYDOWN) {

	switch (event.key.keysym.sym) {

	case SDLK_SPACE:
	  if (adventure_mode) {
	    // the user chose "Adventure"
	    ok_sound->play();
	    finished = true;
	  }
	  else {
	    // the user chose "Solarus Dreams"
	    ok_sound->play();
	    finished = true;
	  }
	  break;

	case SDLK_RIGHT:
	case SDLK_LEFT:
	  cursor_sound->play();
	  adventure_mode = !adventure_mode;
	  break;

	default:
	  break;
	}
      }
    }

    // update the sprites
    update();

    // redraw if necessary
    while (SDL_GetTicks() >= next_display) {
      display_choose_mode_phase();
      next_display = SDL_GetTicks() + FRAME_INTERVAL;
    }
  }
}
*/

/**
 * Displays the "Choose your game mode" phase.
 *
void SelectionMenuPhase::display_choose_mode_phase(void) {

  display_common();

  // move the selected savegame to the top
  int save_number = cursor_position - 1;
  SDL_Rect position = {57, 75, 0, 0};
  SDL_BlitSurface(img_save_container, NULL, destination_surface, &position);
  text_player_names[save_number]->display(destination_surface);
 
  hearts_views[save_number]->display(destination_surface);

  position.x = 62;
  position.y = 80;
  SDL_BlitSurface(img_numbers[save_number], NULL, destination_surface, &position);

  // the two boxes
  SDL_Rect box_position = {0, 0, 73, 54};
  if (adventure_mode) {
    box_position.y = 54; // highlight the selected box
  }
  position.x = 70;
  position.y = 115;
  SDL_BlitSurface(img_mode, &box_position, destination_surface, &position);  

  box_position.x = 73;
  box_position.y = adventure_mode ? 0 : 54; // highlight the selected box
  position.x = 170;
  position.y = 115;
  SDL_BlitSurface(img_mode, &box_position, destination_surface, &position);  

  // blit everything
  SDL_BlitSurface(destination_surface, NULL, zsdx->screen, NULL);
  SDL_Flip(zsdx->screen);
}
*/
