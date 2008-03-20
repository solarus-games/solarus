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

/**
 * Creates a selection menu.
 */
SelectionMenu::SelectionMenu(void):
  adventure_mode(true), next_cloud_move(0) {

  // load the 3 saves
  char file_name[10];
  for (int i = 0; i < 3; i++) {
    sprintf(file_name, "save%d.zsd", i);
    savegames[i] = new Savegame(file_name);
  }

  // load the images
  img_cloud = IMG_Load(FileTools::data_file_add_prefix("images/menus/cloud.png"));
  img_menu = IMG_Load(FileTools::data_file_add_prefix("images/menus/select_a_file.png"));
  img_1 = IMG_Load(FileTools::data_file_add_prefix("images/menus/save1.png"));
  img_2 = IMG_Load(FileTools::data_file_add_prefix("images/menus/save2.png"));
  img_3 = IMG_Load(FileTools::data_file_add_prefix("images/menus/save3.png"));

  cursor = new AnimatedSprite(zsdx->game_resource->get_sprite("menus/cursor"));

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

  delete cursor;
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

  bool quit = false;
  SDL_Event event;
  SDL_EnableKeyRepeat(500, 500);

  while (!quit) {

    // if there is an event
    if (SDL_PollEvent(&event)) {
      
      quit = zsdx->handle_event(event);
    
      if (event.type == SDL_KEYDOWN) {

	switch (event.key.keysym.sym) {

	case SDLK_SPACE:
	  quit = true;
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
    if (SDL_GetTicks() >= next_redraw) {
      redraw();
    }
  }

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

  if (cursor_position <= 3) {
    return savegames[cursor_position - 1];
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
  SDL_FillRect(zsdx->screen, NULL, get_color(104, 144, 240));

  /*
  SDL_Color text_color = {0, 0, 0};
  SDL_Surface *text = TTF_RenderText_Blended(zsdx->font, "Bienvenue dans ZSDX !", text_color);
  SDL_Rect position = {100, 200, 0, 0};
  SDL_FillRect(zsdx->screen, NULL, get_color(104, 144, 240));
  SDL_BlitSurface(text, NULL, zsdx->screen, &position);
  */

  // display the clouds
  SDL_Rect position;
  for (i = 0; i < 16; i++) {

    position = cloud_positions[i];

    SDL_BlitSurface(img_cloud, NULL, zsdx->screen, &position);

    if (cloud_positions[i].x >= 320 - 80) {
      position.x = cloud_positions[i].x - 320;
      position.y = cloud_positions[i].y;
      SDL_BlitSurface(img_cloud, NULL, zsdx->screen, &position);

      if (cloud_positions[i].y <= 0) {
	position.x = cloud_positions[i].x - 320;
	position.y = cloud_positions[i].y + 240;
	SDL_BlitSurface(img_cloud, NULL, zsdx->screen, &position);
      }
    }

    if (cloud_positions[i].y <= 0) {
      position.x = cloud_positions[i].x;
      position.y = cloud_positions[i].y + 240;
      SDL_BlitSurface(img_cloud, NULL, zsdx->screen, &position);
    }
  }

  // display the menu
  position.x = 37;
  position.y = 38;
  position.w = 246;
  position.h = 165;
  SDL_BlitSurface(img_menu, NULL, zsdx->screen, &position);

  // cursor
  if (cursor_position != 5) {
    position.x = 58;
  }
  else {
    position.x = 172;
  }

  if (cursor_position < 4) {
    position.y = 49 + cursor_position * 27;
  }
  else {
    position.y = 159;
  }
  cursor->display(zsdx->screen, position.x, position.y);

  // save numbers
  position.x = 62;
  position.y = 80;
  SDL_BlitSurface(img_1, NULL, zsdx->screen, &position);
  position.y = 107;
  SDL_BlitSurface(img_2, NULL, zsdx->screen, &position);
  position.y = 134;
  SDL_BlitSurface(img_3, NULL, zsdx->screen, &position);

  SDL_Flip(zsdx->screen);
}

/**
 * Updates the sprites animations and the position of the clouds.
 */
void SelectionMenu::update(void) {

  // move the clouds
  while (SDL_GetTicks() >= next_cloud_move) {
    
    for (int i = 0; i < 16; i++) {
      cloud_positions[i].x += 2;
      cloud_positions[i].y -= 1;

      if (cloud_positions[i].x >= 320) {
	cloud_positions[i].x = 0;
      }

      if (cloud_positions[i].y >= 240) {
	cloud_positions[i].y = 0;
      }
    }

    next_cloud_move += 100;
  }

  // update the animation of the cursor
  cursor->update_current_frame();
}
