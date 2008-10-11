#include "menus/PauseSubmenuOptions.h"
#include "menus/PauseMenu.h"
#include "TextSurface.h"
#include "Controls.h"
#include "Game.h"
#include "Color.h"
#include "Sprite.h"
#include "ZSDX.h"
#include "KeysEffect.h"

// TODO: load this from some external file (for future translation)
static const string texts[] = {
  "Plein écran",
  "Oui",
  "Non",
  "Commandes",
  "Clavier",
  "Joypad",
};

/**
 * Constructor.
 * @param pause_menu the pause menu object
 * @param game the game
 */
PauseSubmenuOptions::PauseSubmenuOptions(PauseMenu *pause_menu, Game *game):
  PauseSubmenu(pause_menu, game), controls(game->get_controls()) {

  // create the text surfaces
  fullscreen_label_text = new TextSurface(56, 64, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_TOP);
  fullscreen_label_text->set_font(TextSurface::FONT_STANDARD);
  fullscreen_label_text->set_text(texts[0]);

  fullscreen_answer_text = new TextSurface(240, 64, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_TOP);
  fullscreen_answer_text->set_font(TextSurface::FONT_STANDARD);

  controls_text = new TextSurface(84, 82, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_TOP);
  controls_text->set_font(TextSurface::FONT_STANDARD);
  controls_text->set_text(texts[3]);

  keyboard_text = new TextSurface(153, 82, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_TOP);
  keyboard_text->set_font(TextSurface::FONT_STANDARD);
  keyboard_text->set_text(texts[4]);

  joypad_text = new TextSurface(229, 82, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_TOP);
  joypad_text->set_font(TextSurface::FONT_STANDARD);
  joypad_text->set_text(texts[5]);

  controls_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 215, 160, 32, 0, 0, 0, 0);
  SDL_SetColorKey(controls_surface, SDL_SRCCOLORKEY, Color::black);
  controls_visible_y = 0;
  moving_visible_y = 0;

  for (int i = 0; i < 9; i++) {

    int y = 2 + 16 * i;

    game_key_texts[i] = new TextSurface(4, y, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_TOP);
    game_key_texts[i]->set_font(TextSurface::FONT_STANDARD);
    game_key_texts[i]->set_text(controls->get_key_name((Controls::GameKey) (i + 1)));

    keyboard_control_texts[i] = new TextSurface(74, y, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_TOP);
    keyboard_control_texts[i]->set_font(TextSurface::FONT_STANDARD);

    joypad_control_texts[i] = new TextSurface(143, y, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_TOP);
    joypad_control_texts[i]->set_font(TextSurface::FONT_STANDARD);
  }
  load_control_texts();

  up_arrow_sprite = new Sprite("menus/arrow");
  up_arrow_sprite->set_current_direction(0);

  down_arrow_sprite = new Sprite("menus/arrow");
  down_arrow_sprite->set_current_direction(1);

  // action icon
  KeysEffect *keys_effect = game->get_keys_effect();
  keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_CHANGE);
}

/**
 * Destructor.
 */
PauseSubmenuOptions::~PauseSubmenuOptions(void) {

  delete fullscreen_label_text;
  delete fullscreen_answer_text;

  delete controls_text;
  delete keyboard_text;
  delete joypad_text;

  for (int i = 0; i < 9; i++) {
    delete game_key_texts[i];
    delete keyboard_control_texts[i];
    delete joypad_control_texts[i];
  }

  SDL_FreeSurface(controls_surface);

  delete up_arrow_sprite;
  delete down_arrow_sprite;
}

/**
 * Loads the text displayed for each control, for the
 * keyboard and the joypad.
 */
void PauseSubmenuOptions::load_control_texts(void) {

  Controls *controls = game->get_controls();
  for (int i = 0; i < 9; i++) {

    Controls::GameKey key = (Controls::GameKey) (i + 1);

    string text = controls->get_keyboard_string(key);
    keyboard_control_texts[i]->set_text(text);

    text = controls->get_joypad_string(key);
    joypad_control_texts[i]->set_text(text);

    game_key_texts[i]->display(controls_surface);
    keyboard_control_texts[i]->display(controls_surface);
    joypad_control_texts[i]->display(controls_surface);
  }
}

/**
 * This function is called when a key is pressed on this submenu.
 * @param key the key pressed
 */
void PauseSubmenuOptions::key_pressed(Controls::GameKey key) {
  
  switch (key) {

  case Controls::LEFT:
    pause_menu->show_left_submenu();
    break;

  case Controls::RIGHT:
    pause_menu->show_right_submenu();
    break;

  case Controls::UP:
    // move the controls view
    if (controls_visible_y > 0) {
      moving_visible_y = -1;
      next_moving_visible_y_date = SDL_GetTicks();
    }
    break;

  case Controls::DOWN:
    // move the controls view
    if (controls_visible_y < 60) {
      moving_visible_y = 1;
      next_moving_visible_y_date = SDL_GetTicks();
    }
    break;

  default:
    break;
  }
}

/**
 * Updates this submenu.
 */
void PauseSubmenuOptions::update(void) {

  fullscreen_answer_text->set_text(zsdx->is_fullscreen() ? texts[1] : texts[2]);

  bool up = controls->is_key_pressed(Controls::UP);
  bool down = controls->is_key_pressed(Controls::DOWN);

  if (moving_visible_y == -1) {

    if (!up) {
      moving_visible_y = down ? 1 : 0;
    }
    else if (controls_visible_y <= 0) {
      moving_visible_y = 0;
    }
  }
  else if (moving_visible_y == 1) {

    if (!down) {
      moving_visible_y = up ? -1 : 0;
    }
    else if (controls_visible_y >= 60) {
      moving_visible_y = 0;
    }
  }

  Uint32 now = SDL_GetTicks();
  if (moving_visible_y != 0 && now >= next_moving_visible_y_date) {
    controls_visible_y += moving_visible_y;
    next_moving_visible_y_date += 10;
  }
}

/**
 * Displays this submenu.
 * @param destination the destination surface
 */
void PauseSubmenuOptions::display(SDL_Surface *destination) {
  PauseSubmenu::display(destination);

  fullscreen_label_text->display(destination);
  fullscreen_answer_text->display(destination);

  controls_text->display(destination);
  keyboard_text->display(destination);
  joypad_text->display(destination);

  SDL_Rect src_position = {0, controls_visible_y, 215, 86};
  static SDL_Rect dst_position = {53, 100};
  SDL_BlitSurface(controls_surface, &src_position, destination, &dst_position);

  // display the arrows
  if (controls_visible_y > 0) {
    up_arrow_sprite->display(destination, 96, 96);
    up_arrow_sprite->display(destination, 211, 96);
  }

  if (controls_visible_y < 60) {
    down_arrow_sprite->display(destination, 96, 182);
    down_arrow_sprite->display(destination, 211, 182);
  }
}
