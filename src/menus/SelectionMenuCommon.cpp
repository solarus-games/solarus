#include "menus/SelectionMenuCommon.h"
#include "FileTools.h"
#include "Sprite.h"
#include "TextDisplayed.h"
#include "Sound.h"
#include "Music.h"
#include "ActionIcon.h"
#include "SwordIcon.h"
#include "KeysEffect.h"
#include "ZSDX.h"
#include "GameResource.h"
#include "HeartsView.h"
#include "Savegame.h"
#include "Color.h"

/**
 * Creates the common part of all selection menu phases.
 */
SelectionMenuCommon::SelectionMenuCommon(void) {

  // fill the arrays
  for (int i = 0; i < 3; i++) {
    savegames[i] = NULL;
    text_player_names[i] = NULL;
    hearts_views[i] = NULL;
  }

  // load the savegames
  read_savegames();

  // load the images
  img_cloud = FileTools::open_image("menus/selection_menu_cloud.png");
  img_background = FileTools::open_image("menus/selection_menu_background.png");
  img_save_container = FileTools::open_image("menus/selection_menu_save_container.png");
  img_option_container = FileTools::open_image("menus/selection_menu_option_container.png");

  for (int i = 0; i < 3; i++) {
    ostringstream oss;
    oss << "menus/selection_menu_save" << (i + 1) << ".png";
    img_numbers[i] = FileTools::open_image(oss.str());
  }

  cursor = new Sprite("menus/selection_menu_cursor");

  // texts
  text_option1 = new TextDisplayed(90, 172, ALIGN_LEFT, ALIGN_MIDDLE);
  text_option2 = new TextDisplayed(198, 172, ALIGN_LEFT, ALIGN_MIDDLE);
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
  error_sound = zsdx->game_resource->get_sound("wrong");

  // music
  music = zsdx->game_resource->get_music("game_over.it");
  music->play();

  // initialize the clouds
  initialize_clouds();
}

/**
 * Destructor.
 */
SelectionMenuCommon::~SelectionMenuCommon(void) {
  SDL_FreeSurface(img_cloud);
  SDL_FreeSurface(img_background);
  SDL_FreeSurface(img_save_container);
  SDL_FreeSurface(img_option_container);

  delete text_option1;
  delete text_option2;
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

  music->stop();
}

/**
 * Initializes the clouds position.
 */
void SelectionMenuCommon::initialize_clouds(void) {

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
 * Loads (or reloads) the data of the 3 savegames and creates the surfaces to display.
 */
void SelectionMenuCommon::read_savegames(void) {

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
    string player_name;
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
 * Updates the common part of the selection menu.
 * This function is called by the update() function of
 * the current selection menu phase.
 */
void SelectionMenuCommon::update(void) {

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
 * Displays the common part of the selection menu.
 * This function is called by the display() function of
 * the current selection menu phase.
 * @param destination_surface the surface where to display
 */
void SelectionMenuCommon::display(SDL_Surface *destination_surface) {

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

}
