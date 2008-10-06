#include "menus/SelectionMenuConfirmErase.h"
#include "menus/SelectionMenuSelectFile.h"
#include "ZSDX.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "Savegame.h"

/**
 * Creates a selection menu with the phase where the
 * player has to confirm to erase a file.
 * @param previous the previous phase
 * @param save_number_to_erase number of the savegame to delete (0 to 2)
 */
SelectionMenuConfirmErase::SelectionMenuConfirmErase(SelectionMenuPhase *previous,
						     int save_number_to_erase):
  SelectionMenuPhase(previous, "Etes-vous sûr ?"),
  save_number_to_erase(save_number_to_erase) {

  set_cursor_position(4); // select "no" by default
}

/**
 * Destructor.
 */
SelectionMenuConfirmErase::~SelectionMenuConfirmErase(void) {

}

/**
 * Handles an SDL event in this phase.
 * @param event the event
 */
void SelectionMenuConfirmErase::handle_event(const SDL_Event &event) {

  if (event.type == SDL_KEYDOWN) {
    
    int cursor_position = get_cursor_position();

    switch (event.key.keysym.sym) {

    case SDLK_SPACE:
    case SDLK_RETURN:
      if (cursor_position == 5) {
	// the user chose "Yes"
	ResourceManager::get_sound("boss_dead")->play();
	delete_save_file(save_number_to_erase);
	set_cursor_position(save_number_to_erase + 1);
	set_next_screen(new SelectionMenuSelectFile(this));
      }
      else if (cursor_position == 4) {
	// the user chose "No"
	play_ok_sound();
	set_next_screen(new SelectionMenuSelectFile(this));
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

/**
 * Displays the selection menu in this phase.
 */
void SelectionMenuConfirmErase::display(SDL_Surface *screen_surface) {

  start_display(screen_surface);

  // savegame
  display_savegame(save_number_to_erase);
  display_savegame_number(save_number_to_erase);

  // options
  display_options("NON", "OUI");

  // cursor
  display_normal_cursor();
 
  finish_display(screen_surface);
}

/**
 * Deletes a save file.
 * @param save_number number of the savegame to delete (0 to 2)
 */
void SelectionMenuConfirmErase::delete_save_file(int save_number) {

  Savegame *savegame = get_savegame(save_number);
  remove(savegame->get_file_name());
  reload_savegames();
}
