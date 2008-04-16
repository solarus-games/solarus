/**
 * This module shows the menu to select the save file and the game mode.
 */

#include "SelectionMenu.h"
#include "Savegame.h"
#include "ZSDX.h"
#include "Music.h"
#include "GameResource.h"
#include "AnimatedSprite.h"
#include "Color.h"
#include "FileTools.h"
#include "TextDisplayed.h"
#include "TransitionEffect.h"
#include "HeartsView.h"
#include "ActionIcon.h"
#include "SwordIcon.h"

/**
 * The differents screens of the selection menu.
 */
enum SelectionMenuScreens {
  SELECT_FILE   = 0,
  ERASE_FILE    = 1,
  CONFIRM_ERASE = 2,
  CHOOSE_NAME   = 3,
  CHOOSE_MODE   = 4,
};

/**
 * The title text showed over the menu.
 */
const char *SelectionMenu::title_strings[5] = {
  "Veuillez choisir un fichier",
  "Quel fichier voulez-vous effacer ?",
  "Etes-vous sûr ?",
  "Quel est votre nom ?",
  "Choisissez un mode de jeu",
};

/**
 * Creates a selection menu.
 */
SelectionMenu::SelectionMenu(void):
  adventure_mode(true) {

  for (int i = 0; i < 3; i++) {
    savegames[i] = NULL;
    text_player_names[i] = NULL;
    hearts_views[i] = NULL;
  }

  // read the saves
  read_saves();

  // load the images
  img_cloud = FileTools::open_image("menus/selection_menu_cloud.png");
  img_background = FileTools::open_image("menus/selection_menu_background.png");
  img_save_container = FileTools::open_image("menus/selection_menu_save_container.png");
  img_option_container = FileTools::open_image("menus/selection_menu_option_container.png");
  img_arrow = FileTools::open_image("menus/selection_menu_arrow.png");
  img_letters = FileTools::open_image("menus/selection_menu_letters.png");

  char file_name[MAX_FILE_NAME];
  for (int i = 0; i < 3; i++) {
    sprintf(file_name, "menus/selection_menu_save%d.png", i + 1);
    img_numbers[i] = FileTools::open_image(file_name);
  }

  cursor = new AnimatedSprite(zsdx->game_resource->get_sprite("menus/selection_menu_cursor"));

  // texts
  text_option1 = new TextDisplayed(90, 172, ALIGN_LEFT, ALIGN_MIDDLE);
  text_option2 = new TextDisplayed(198, 172, ALIGN_LEFT, ALIGN_MIDDLE);
  text_new_player_name = new TextDisplayed(67, 85, ALIGN_LEFT, ALIGN_MIDDLE);
  text_title = new TextDisplayed(160, 54, ALIGN_CENTER, ALIGN_MIDDLE);
  text_title->set_font(FONT_STANDARD);

  // icons
  keys_effect = new KeysEffect();
  keys_effect->set_action_key_effect(ACTION_KEY_VALIDATE);
  keys_effect->set_sword_key_effect(SWORD_KEY_CHOOSE);
  keys_effect->set_sword_key_enabled(false);
  action_icon = new ActionIcon(keys_effect, 13, 31);
  sword_icon = new SwordIcon(keys_effect, NULL, 0, 9);

  // sounds
  cursor_sound = zsdx->game_resource->get_sound("cursor");
  ok_sound = zsdx->game_resource->get_sound("ok");
  letter_sound = zsdx->game_resource->get_sound("danger");
  erase_sound = zsdx->game_resource->get_sound("boss_dead");
  error_sound = zsdx->game_resource->get_sound("wrong");
  
  // initialize the clouds
  initialize_clouds();
}

/**
 * Destroys the selection menu.
 */
SelectionMenu::~SelectionMenu(void) {
  SDL_FreeSurface(img_cloud);
  SDL_FreeSurface(img_background);
  SDL_FreeSurface(img_save_container);
  SDL_FreeSurface(img_option_container);
  SDL_FreeSurface(img_arrow);
  SDL_FreeSurface(img_letters);

  delete text_option1;
  delete text_option2;
  delete text_new_player_name;
  delete text_title;

  for (int i = 0; i < 3; i++) {
    SDL_FreeSurface(img_numbers[i]);
    delete text_player_names[i];
    delete hearts_views[i];
    delete savegames[i];
  }

  delete action_icon;
  delete sword_icon;
  delete keys_effect;

  delete cursor;
}

/**
 * Initializes the clouds position.
 */
void SelectionMenu::initialize_clouds(void) {

  next_cloud_move = SDL_GetTicks();

  int i;

  for (i = 0; i < 16; i++) {
    cloud_positions[i].w = 80;
    cloud_positions[i].h = 44;
  }

  i = 0;

  cloud_positions[i].x = 20;
  cloud_positions[i].y = 40;
  i++;

  cloud_positions[i].x = 50;
  cloud_positions[i].y = 160;
  i++;

  cloud_positions[i].x = 160;
  cloud_positions[i].y = 30;
  i++;

  cloud_positions[i].x = 270;
  cloud_positions[i].y = 200;
  i++;

  cloud_positions[i].x = 200;
  cloud_positions[i].y = 120;
  i++;

  cloud_positions[i].x = 90;
  cloud_positions[i].y = 120;
  i++;

  cloud_positions[i].x = 300;
  cloud_positions[i].y = 100;
  i++;

  cloud_positions[i].x = 240;
  cloud_positions[i].y = 10;
  i++;

  cloud_positions[i].x = 60;
  cloud_positions[i].y = 190;
  i++;

  cloud_positions[i].x = 150;
  cloud_positions[i].y = 210;
  i++;

  cloud_positions[i].x = 310;
  cloud_positions[i].y = 220;
  i++;

  cloud_positions[i].x = 70;
  cloud_positions[i].y = 20;
  i++;

  cloud_positions[i].x = 130;
  cloud_positions[i].y = 180;
  i++;

  cloud_positions[i].x = 200;
  cloud_positions[i].y = 70;
  i++;

  cloud_positions[i].x = 20;
  cloud_positions[i].y = 120;
  i++;

  cloud_positions[i].x = 170;
  cloud_positions[i].y = 220;
  i++;
}

/**
 * Loads the data of the 3 savegames and creates the surfaces to display.
 */
void SelectionMenu::read_saves(void) {

  // load the 3 saves
  char file_name[10];
  for (int i = 0; i < 3; i++) {

    sprintf(file_name, "save%d.zsd", i + 1);

    if (savegames[i] != NULL) {
      delete savegames[i];
    }
    savegames[i] = new Savegame(file_name);

    // get the data

    // player name
    const char *player_name;
    if (!savegames[i]->is_empty()) {

      player_name = savegames[i]->get_reserved_string(SAVEGAME_PLAYER_NAME);
    }
    else {
      player_name = "- Vide -";
    }

    if (text_player_names[i] != NULL) {
      delete text_player_names[i];
    }

    text_player_names[i] = new TextDisplayed(87, 88 + i * 27,
					     ALIGN_LEFT, ALIGN_MIDDLE);
    text_player_names[i]->set_text(player_name);

    // hearts
    if (hearts_views[i] != NULL) {
      delete hearts_views[i];
    }

    hearts_views[i] = new HeartsView(savegames[i]->get_equipment(), 168, 78 + i * 27);
  }
}

/**
 * Shows the selection menu.
 * After calling this function, you can call
 * get_selected_save() and is_adventure_mode() to
 * know the user choice.
 */
void SelectionMenu::show(void) {

  // initialize the screen
  destination_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 32, 0, 0, 0, 0);

  // play the selection menu music
  Music *music = zsdx->game_resource->get_music("game_over.it");
  music->play();

  // in transition
  transition = TransitionEffect::create_transition(TRANSITION_FADE, TRANSITION_IN);
  transition->start();

  // show the first screen
  show_select_file_screen();

  // out transition (the selection menu is over now)
  delete transition;
  SDL_Event event;
  if (!zsdx->is_exiting()) {

    transition = TransitionEffect::create_transition(TRANSITION_FADE, TRANSITION_OUT);
    transition->start();
    while (!zsdx->is_exiting() && !transition->is_over()) {

      if (SDL_PollEvent(&event)) {
	zsdx->handle_event(event);
      }

      redraw_choose_mode_screen();
    }
    delete transition;
  }

  SDL_FreeSurface(destination_surface);
  music->stop();
}

/**
 * Returns the savegame selected by the user.
 * The show() function must have been called first.
 * The savegame returned is not freed when the menu is freed.
 * @return the savegame selected by the user, or NULL
 * if he wants to quit.
 */
Savegame * SelectionMenu::get_selected_save(void) {

  if (cursor_position <= 3 && !zsdx->is_exiting()) {

    char file_name[10];
    sprintf(file_name, "save%d.zsd", cursor_position);
    return new Savegame(file_name);
  }
  
  return NULL;
}

/**
 * Returns whether the user has selected the Adventure mode
 * or the Solarus Dreams mode.
 * The show() function must have been called first and the
 * user should have selected a savegame, otherwise calling
 * this function doesn't make sense.
 * @return true if the user has selected the Adventure mode,
 * false if he has selected the Solarus Dreams mode 
 */
bool SelectionMenu::is_adventure_mode(void) {
  return adventure_mode;
}

/**
 * Redraws the elements common to all the screens of
 * the selection menu.
 */
void SelectionMenu::redraw_common(void) {

  // background color
  SDL_FillRect(destination_surface, NULL, get_color(104, 144, 240));

  // display the clouds
  SDL_Rect position;
  for (int i = 0; i < 16; i++) {

    position = cloud_positions[i];

    SDL_BlitSurface(img_cloud, NULL, destination_surface, &position);

    if (cloud_positions[i].x >= 320 - 80) {
      position.x = cloud_positions[i].x - 320;
      position.y = cloud_positions[i].y;
      SDL_BlitSurface(img_cloud, NULL, destination_surface, &position);

      if (cloud_positions[i].y <= 0) {
	position.x = cloud_positions[i].x - 320;
	position.y = cloud_positions[i].y + 240;
	SDL_BlitSurface(img_cloud, NULL, destination_surface, &position);
      }
    }

    if (cloud_positions[i].y <= 0) {
      position.x = cloud_positions[i].x;
      position.y = cloud_positions[i].y + 240;
      SDL_BlitSurface(img_cloud, NULL, destination_surface, &position);
    }
  }

  // display the background image
  position.x = 37;
  position.y = 38;
  position.w = 246;
  position.h = 165;
  SDL_BlitSurface(img_background, NULL, destination_surface, &position);

  /* no icons to simplify the keys
  // icons
  action_icon->display(destination_surface);
  sword_icon->display(destination_surface);
  */

  // title
  display_title_text();

  // transition
  SDL_FillRect(zsdx->screen, NULL, COLOR_BLACK);
  if (transition != NULL && transition->is_started()) {
    transition->display(destination_surface);
  }
}

/**
 * Updates the data common to all screens of the selection menu
 * (i.e. the sprites animations and the position of the clouds).
 */
void SelectionMenu::update(void) {

  // move the clouds
  Uint32 now = SDL_GetTicks();
  while (now >= next_cloud_move) {
    
    for (int i = 0; i < 16; i++) {
      cloud_positions[i].x += 1;
      cloud_positions[i].y -= 1;

      if (cloud_positions[i].x >= 320) {
	cloud_positions[i].x = 0;
      }

      if (cloud_positions[i].y <= -44) {
	cloud_positions[i].y = 240 - 44;
      }
    }

    next_cloud_move += 100;
  }

  // update the icons
  action_icon->update();
  sword_icon->update();

  // update the animation of the cursor
  cursor->update_current_frame();
}

/**
 * This function is called when the user wants to move the cursor
 * upwards (except for the letter cursor).
 */
void SelectionMenu::move_cursor_up(void) {

  cursor_sound->play();
  cursor->restart_animation();
  cursor_position--;
  if (cursor_position == 0) {
    cursor_position = 4;
  }
  else if (cursor_position == 4) {
    cursor_position = 3;
  }
}

/**
 * This function is called when the user wants to move the cursor
 * downwards (except for the letter cursor).
 */
void SelectionMenu::move_cursor_down(void) {

  cursor_sound->play();
  cursor->restart_animation();
  cursor_position++;
  if (cursor_position >= 5) {
    cursor_position = 1;
  }
}

/**
 * This function is called when the user wants to move the cursor
 * to the left or to the right (except for the letter cursor).
 */
void SelectionMenu::move_cursor_left_or_right(void) {

  if (cursor_position == 4) {
    cursor_sound->play();
    cursor->restart_animation();
    cursor_position = 5;
  }
  else if (cursor_position == 5) {
    cursor_sound->play();
    cursor->restart_animation();
    cursor_position = 4;
  }
}

/**
 * Draws a title on the selection menu background.
 * @param text the title to add
 */
void SelectionMenu::display_title_text(void) {
  
  text_title->set_text(title_strings[current_screen]);
  text_title->display(destination_surface);
}

/**
 * Displays a savegame on the surface.
 * @param save_number number of the savegame to display (0 to 2)
 */
void SelectionMenu::display_savegame(int save_number) {

  SDL_Rect position;

  // draw the container
  position.x = 57;
  position.y = 75 + save_number * 27;
  SDL_BlitSurface(img_save_container, NULL, destination_surface, &position);

  // draw the player's name
  text_player_names[save_number]->display(destination_surface);

  // draw the hearts
  hearts_views[save_number]->display(destination_surface);
}

/**
 * Displays the number of a savegame.
 * This function is separate from the display_savegame() function
 * because the cursor has to be displayed after the savegame images
 * but before the savegame number.
 * @param save_number number to display (0 to 2)
 */
void SelectionMenu::display_savegame_number(int save_number) {

  SDL_Rect position;

  position.x = 62;
  position.y = 80 + 27 * save_number;
  SDL_BlitSurface(img_numbers[save_number], NULL, destination_surface, &position);
}

/**
 * Displays the two options in the bottom of the screen.
 * @param left text of the first option (on the left)
 * @param right text of the second option (on the right)
 */
void SelectionMenu::display_options(const char *left, const char *right) {

  SDL_Rect position;

  position.x = 57;
  position.y = 158;
  SDL_BlitSurface(img_option_container, NULL, destination_surface, &position);
  position.x = 165;
  SDL_BlitSurface(img_option_container, NULL, destination_surface, &position);

  text_option1->set_text(left);
  text_option1->display(destination_surface);

  text_option2->set_text(right);
  text_option2->display(destination_surface);

}

/**
 * Displays the normal cursor (i.e. the cursor to select a file)
 * at its current position.
 */
void SelectionMenu::display_normal_cursor(void) {

  SDL_Rect position;

  if (cursor_position != 5) {
    position.x = 58;
  }
  else {
    position.x = 166;
  }

  if (cursor_position < 4) {
    position.y = 49 + cursor_position * 27;
  }
  else {
    position.y = 159;
  }
  cursor->display(destination_surface, position.x, position.y);
}

/**
 * Shows the main screen of the selection menu.
 */
void SelectionMenu::show_select_file_screen(void) {

  current_screen = SELECT_FILE;

  cursor->set_current_animation("blue");
  cursor_position = 1;

  SDL_Event event;
  SDL_EnableKeyRepeat(0, 0); // no repeat

  Uint32 next_redraw = SDL_GetTicks();
  while (!zsdx->is_exiting()) {

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
	    // the user chose "Erase"
	    ok_sound->play();
	    show_erase_file_screen();
	    current_screen = SELECT_FILE;
	    continue;
	  }
	  else {
	    // the user chose a save
	    ok_sound->play();

	    if (savegames[cursor_position - 1]->is_empty()) {
	      // the savegame doesn't exist: ask the name
	      show_choose_name_screen();
	      current_screen = SELECT_FILE;
	      continue;
	    }
	    else {
	      // the savegame exists: choose the mode and then start the game
	      show_choose_mode_screen();
	      return; // don't redraw the select file screen any more
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
    while (SDL_GetTicks() >= next_redraw) {
      redraw_select_file_screen();
      next_redraw = SDL_GetTicks() + FRAME_INTERVAL;
    }
  }
}

/**
 * Redraws the main screen of the selection menu
 * (i.e. the screen showing the 3 saves).
 */
void SelectionMenu::redraw_select_file_screen(void) {

  redraw_common();

  // savegames
  for (int i = 0; i < 3; i++) {
    display_savegame(i);
  }

  // options
  display_options("Effacer", "Quitter");

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

/**
 * Displays the "Which file do you want to erase?" screen.
 */
void SelectionMenu::show_erase_file_screen(void) {

  current_screen = ERASE_FILE;

  cursor->set_current_animation("red");

  bool finished = false;
  SDL_Event event;

  Uint32 next_redraw = SDL_GetTicks();
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
	      show_confirm_erase_screen();
	      current_screen = ERASE_FILE;
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
    while (SDL_GetTicks() >= next_redraw) {
      redraw_erase_file_screen();
      next_redraw = SDL_GetTicks() + FRAME_INTERVAL;
    }
  }

  cursor->set_current_animation("blue");
}

/**
 * Redraws the "Which file do you want to erase?" screen.
 */
void SelectionMenu::redraw_erase_file_screen(void) {

  redraw_common();

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

/**
 * Displays the "Are you sure?" screen.
 */
void SelectionMenu::show_confirm_erase_screen(void) {

  current_screen = CONFIRM_ERASE;

  bool finished = false;
  SDL_Event event;

  cursor_position = 4; // select "no" by default

  Uint32 next_redraw = SDL_GetTicks();
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
    while (SDL_GetTicks() >= next_redraw) {
      redraw_confirm_erase_screen();
      next_redraw = SDL_GetTicks() + FRAME_INTERVAL;
    }
  }
}

/**
 * Redraws the "Are you sure?" screen.
 */
void SelectionMenu::redraw_confirm_erase_screen(void) {

  redraw_common();

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

/**
 * Deletes a save file.
 * @param save_number number of the savegame to delete (0 to 2)
 */
void SelectionMenu::delete_save_file(int save_number) {
  
  remove(savegames[save_number]->get_file_name());
  read_saves();
}

/**
 * Displays the "What is your name?" screen.
 */
void SelectionMenu::show_choose_name_screen(void) {

  current_screen = CHOOSE_NAME;

  player_name[0] = '\0';
  text_new_player_name->set_text(player_name);

  keys_effect->set_sword_key_enabled(true);
  cursor->set_current_animation("letters");

  x_letter_cursor = 0;
  y_letter_cursor = 0;

  bool finished = false;
  SDL_Event event;

  Uint32 next_redraw = SDL_GetTicks();
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
    while (SDL_GetTicks() >= next_redraw) {
      redraw_choose_name_screen();
      next_redraw = SDL_GetTicks() + FRAME_INTERVAL;
    }
  }

  cursor->set_current_animation("blue");
  keys_effect->set_sword_key_enabled(false);
}

/**
 * Redraws the "What is your name?" screen.
 */
void SelectionMenu::redraw_choose_name_screen(void) {

  redraw_common();

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

/**
 * This function is called when the player chooses a letter
 * when typing his name.
 * @return true if he finished typing the name (because he validated
 * or cancelled), false otherwise
 */
bool SelectionMenu::select_letter(void) {

  int length = strlen(player_name);
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

/**
 * This function is called when the player wants to choose a letter
 * when typing his name.
 * @return true if the new name is validated, false otherwise
 */
bool SelectionMenu::validate_player_name(void) {

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

/**
 * Displays the "Choose your game mode" screen.
 */
void SelectionMenu::show_choose_mode_screen(void) {

  current_screen = CHOOSE_MODE;

  // move the savegame elements to the top
  int save_number = cursor_position - 1;
  delete hearts_views[save_number];
  hearts_views[save_number] =
    new HeartsView(savegames[save_number]->get_equipment(), 168, 78);

  text_player_names[save_number]->set_position(87, 88);

  bool finished = false;
  SDL_Event event;

  Uint32 next_redraw = SDL_GetTicks();
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
    while (SDL_GetTicks() >= next_redraw) {
      redraw_choose_mode_screen();
      next_redraw = SDL_GetTicks() + FRAME_INTERVAL;
    }
  }
}

/**
 * Redraws the "Choose your game mode" screen.
 */
void SelectionMenu::redraw_choose_mode_screen(void) {

  redraw_common();

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
  
  // highlight the selected box

  // text

  // blit everything
  SDL_BlitSurface(destination_surface, NULL, zsdx->screen, NULL);
  SDL_Flip(zsdx->screen);
}
