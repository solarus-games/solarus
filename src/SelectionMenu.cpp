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

/**
 * Creates a selection menu.
 */
SelectionMenu::SelectionMenu(void):
  adventure_mode(true), next_cloud_move(0) {

  for (int i = 0; i < 3; i++) {
    text_player_names[i] = NULL;
  }

  // read the saves
  read_saves();

  // load the images
  img_cloud = FileTools::open_image("menus/cloud.png");
  img_menu = FileTools::open_image("menus/select_a_file.png");
  img_1 = FileTools::open_image("menus/save1.png");
  img_2 = FileTools::open_image("menus/save2.png");
  img_3 = FileTools::open_image("menus/save3.png");

  cursor = new AnimatedSprite(zsdx->game_resource->get_sprite("menus/cursor"));

  // erase + quit
  TextDisplayed *text = new TextDisplayed(TEXT_SOLID, ALIGN_LEFT, ALIGN_MIDDLE);
  text->set_text_color(255, 255, 255);
  text->create_text("Effacer", 54, 133);
  text->display(img_menu);

  text->create_text("Quitter", 162, 133);
  text->display(img_menu);
  delete text;

  // initialize the clouds
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
 * Destroys the selection menu.
 */
SelectionMenu::~SelectionMenu(void) {
  SDL_FreeSurface(img_cloud);
  SDL_FreeSurface(img_menu);
  SDL_FreeSurface(img_1);
  SDL_FreeSurface(img_2);
  SDL_FreeSurface(img_3);

  for (int i = 0; i < 3; i++) {
    delete text_player_names[i];
    delete hearts_views[i];
    delete savegames[i];
  }

  delete cursor;
}

/**
 * Loads the data of the 3 savegames and creates the surfaces to display.
 */
void SelectionMenu::read_saves(void) {

  // load the 3 saves
  char file_name[10];
  for (int i = 0; i < 3; i++) {

    sprintf(file_name, "save%d.zsd", i + 1);
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

    text_player_names[i] = new TextDisplayed(TEXT_SOLID, ALIGN_LEFT, ALIGN_MIDDLE);
    text_player_names[i]->set_text_color(255, 255, 255);
    text_player_names[i]->create_text(player_name, 87, 88 + i * 27);

    // hearts
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
  Uint32 next_redraw = SDL_GetTicks();

  // play the selection menu music
  Music *music = zsdx->game_resource->get_music("game_over.it");
  music->play();

  cursor->set_current_animation("blue");
  cursor_position = 1;
  Sound *cursor_sound = zsdx->game_resource->get_sound("cursor");
  Sound *ok_sound = zsdx->game_resource->get_sound("ok");

  bool start = false;
  bool quit = false;
  SDL_Event event;
  SDL_EnableKeyRepeat(0, 0); // no repeat

  destination_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 32, 0, 0, 0, 0);
  transition = TransitionEffect::create_transition(TRANSITION_FADE, TRANSITION_IN);
  transition->start();

  while (!zsdx->is_exiting() && !start && !quit) {

    // if there is an event
    if (SDL_PollEvent(&event)) {
      
      zsdx->handle_event(event);
    
      if (event.type == SDL_KEYDOWN) {

	switch (event.key.keysym.sym) {

	case SDLK_SPACE:
	  if (cursor_position == 5) {
	    quit = true;
	  }
	  else if (cursor_position == 4) {
	    cursor->set_current_animation("red");
	    ok_sound->play();
	  }
	  else {
	    
	    // temporary (the savegame should be created when the user typed the name
	    if (savegames[cursor_position - 1]->is_empty()) {
	      savegames[cursor_position - 1]->set_reserved_string(SAVEGAME_PLAYER_NAME, "Link");
	      savegames[cursor_position - 1]->save();
	    }
	    
	    start = true;
	    ok_sound->play();
	  }
	  break;

	case SDLK_DOWN:
	  cursor_sound->play();
	  cursor->restart_animation();
	  cursor_position++;
	  if (cursor_position >= 5) {
	    cursor_position = 1;
	  }
	  break;

	case SDLK_UP:
	  cursor_sound->play();
	  cursor->restart_animation();
	  cursor_position--;
	  if (cursor_position == 0) {
	    cursor_position = 4;
	  }
	  else if (cursor_position == 4) {
	    cursor_position = 3;
	  }
	  break;

	case SDLK_RIGHT:
	case SDLK_LEFT:
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
      redraw();
      next_redraw += 10;
    }
  }

  delete transition;

  // transition
  if (!zsdx->is_exiting() && !quit) {

    transition = TransitionEffect::create_transition(TRANSITION_FADE, TRANSITION_OUT);
    transition->start();
    while (!zsdx->is_exiting() && !transition->is_over()) {

      if (SDL_PollEvent(&event)) {
	zsdx->handle_event(event);
      }

      redraw();
    }
    delete transition;
  }

  SDL_FreeSurface(destination_surface);

  // stop the music
  music->stop();
}

/**
 * Returns the savegame selected by the user.
 * The savegame returned is not freed when the menu is freed.
 * The show() function must have been called first.
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
 * Redraws the screen.
 */
void SelectionMenu::redraw(void) {

  int i;

  // background color
  SDL_FillRect(destination_surface, NULL, get_color(104, 144, 240));

  // display the clouds
  SDL_Rect position;
  for (i = 0; i < 16; i++) {

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

  // display the menu
  position.x = 37;
  position.y = 38;
  position.w = 246;
  position.h = 165;
  SDL_BlitSurface(img_menu, NULL, destination_surface, &position);

  // savegame names + hearts
  for (i = 0; i < 3; i++) {
    text_player_names[i]->display(destination_surface);
    hearts_views[i]->display(destination_surface);
  }

  // cursor
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

  // save numbers
  position.x = 62;
  position.y = 80;
  SDL_BlitSurface(img_1, NULL, destination_surface, &position);
  position.y = 107;
  SDL_BlitSurface(img_2, NULL, destination_surface, &position);
  position.y = 134;
  SDL_BlitSurface(img_3, NULL, destination_surface, &position);

  // transition
  SDL_FillRect(zsdx->screen, NULL, COLOR_BLACK);
  if (transition != NULL && transition->is_started()) {
    transition->display(destination_surface);
  }
  SDL_BlitSurface(destination_surface, NULL, zsdx->screen, NULL);
  SDL_Flip(zsdx->screen);
}

/**
 * Updates the sprites animations and the position of the clouds.
 */
void SelectionMenu::update(void) {

  // move the clouds
  while (SDL_GetTicks() >= next_cloud_move) {
    
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

  // update the animation of the cursor
  cursor->update_current_frame();
}
