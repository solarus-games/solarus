#include "menus/PauseSubmenu.h"
#include "TextSurface.h"
#include "Game.h"

/**
 * Constructor.
 * @param pause_menu the pause menu object
 * @param game the game
 */
PauseSubmenu::PauseSubmenu(PauseMenu *pause_menu, Game *game):
  pause_menu(pause_menu), game(game), savegame(game->get_savegame()),
  equipment(game->get_equipment()), dungeon_equipment(game->get_dungeon_equipment()) {

  caption_text_1 = new TextSurface(160, 0,
				   TextSurface::ALIGN_CENTER,
				   TextSurface::ALIGN_MIDDLE);
  caption_text_1->set_font(TextSurface::FONT_STANDARD);

  caption_text_2 = new TextSurface(160, 0,
				   TextSurface::ALIGN_CENTER,
				   TextSurface::ALIGN_MIDDLE);
  caption_text_2->set_font(TextSurface::FONT_STANDARD);
}

/**
 * Destructor.
 */
PauseSubmenu::~PauseSubmenu(void) {
  delete caption_text_1;
  delete caption_text_2;
}

/**
 * Displays the common part of a submenu.
 * The subclasses should call this function.
 * @param destination the surface to draw
 */
void PauseSubmenu::display(SDL_Surface *destination) {
  caption_text_1->display(destination);
  caption_text_2->display(destination);
}

/**
 * Sets the caption text.
 * The caption text can have one or two lines, with 20 characters maximum for each line.
 * If the text you want to display has two lines, use the '\n' character to separate them.
 * @param text the caption text
 */
void PauseSubmenu::set_caption_text(string text) {

  // check the presence of the '\n' character
  size_t index = text.find('\n', 0);

  if (index == string::npos) {
    // only one line
    caption_text_1->set_y(209);
    caption_text_1->set_text(text);
    caption_text_2->set_text("");
  }
  else {
    // two lines

    string line1 = text.substr(0, index);
    string line2 = text.substr(index + 1);

    caption_text_1->set_y(203);
    caption_text_2->set_y(215);
    caption_text_1->set_text(line1);
    caption_text_2->set_text(line2);
  }
}
