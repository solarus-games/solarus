/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "menus/SelectionMenu.h"
#include "menus/SelectionMenuSelectFile.h"
#include "Sprite.h"
#include "hud/HeartsView.h"
#include "KeysEffect.h"
#include "ResourceManager.h"
#include "Savegame.h"
#include "Game.h"
#include "Solarus.h"
#include "lowlevel/TextSurface.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Music.h"
#include "lowlevel/Color.h"
#include "lowlevel/System.h"
#include "StringResource.h"
#include <sstream>

/**
 * @brief Creates the selection menu.
 * @param solarus the Solarus object
 */
SelectionMenu::SelectionMenu(Solarus *solarus):
  Screen(solarus) {

  // phases
  next_phase = NULL;
  surface = new Surface(320, 240);

  // fill the arrays
  for (int i = 0; i < 3; i++) {
    savegames[i] = NULL;
    text_player_names[i] = NULL;
    hearts_views[i] = NULL;
  }

  // load the savegames
  read_savegames();

  // load the images
  cloud_img = new Surface("menus/selection_menu_cloud.png");
  background_img = new Surface("menus/selection_menu_background.png");
  save_container_img = new Surface("menus/selection_menu_save_container.png");
  option_container_img = new Surface("menus/selection_menu_option_container.png");

  for (int i = 0; i < 3; i++) {
    std::ostringstream oss;
    oss << "menus/selection_menu_save" << (i + 1) << ".png";
    number_imgs[i] = new Surface(oss.str());
  }

  background_color = Color(104, 144, 240);
  cursor_sprite = new Sprite("menus/selection_menu_cursor");
  allow_cursor_date = 0;

  // texts
  option1_text = new TextSurface(90, 172, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_MIDDLE);
  option2_text = new TextSurface(198, 172, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_MIDDLE);
  title_text = new TextSurface(160, 54, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_MIDDLE);
  title_text->set_font("fixed");

  // music
  Music::play("gameover.spc");

  // transitions
  transition = Transition::create(Transition::FADE, Transition::IN);
  transition->start();

  // initialize the clouds
  initialize_clouds();

  // first phase: select a file
  current_phase = new SelectionMenuSelectFile(this);
}

/**
 * @brief Destructor.
 */
SelectionMenu::~SelectionMenu() {

  delete current_phase;
  delete surface;
  delete transition;

  delete cloud_img;
  delete background_img;
  delete save_container_img;
  delete option_container_img;

  delete option1_text;
  delete option2_text;
  delete title_text;

  for (int i = 0; i < 3; i++) {
    delete number_imgs[i];
    delete text_player_names[i];
    delete hearts_views[i];
    delete savegames[i];
  }

  delete cursor_sprite;

  Music::play(Music::none);
}

/**
 * @brief Initializes the clouds position.
 */
void SelectionMenu::initialize_clouds() {

  next_cloud_move = System::now();

  int i;

  for (i = 0; i < 16; i++) {
    cloud_positions[i].set_size(80, 44);
  }

  i = 0;

  cloud_positions[i++].set_xy(20, 40);
  cloud_positions[i++].set_xy(50, 160);
  cloud_positions[i++].set_xy(160, 30);
  cloud_positions[i++].set_xy(270, 200);
  cloud_positions[i++].set_xy(200, 120);
  cloud_positions[i++].set_xy(90, 120);
  cloud_positions[i++].set_xy(300, 100);
  cloud_positions[i++].set_xy(240, 10);
  cloud_positions[i++].set_xy(60, 190);
  cloud_positions[i++].set_xy(150, 210);
  cloud_positions[i++].set_xy(310, 220);
  cloud_positions[i++].set_xy(70, 20);
  cloud_positions[i++].set_xy(130, 180);
  cloud_positions[i++].set_xy(200, 700);
  cloud_positions[i++].set_xy(20, 120);
  cloud_positions[i++].set_xy(170, 220);
}

/**
 * @brief Loads (or reloads) the data of the 3 savegames and creates the surfaces to display.
 */
void SelectionMenu::read_savegames() {

  // load the 3 saves
  std::string file_name;
  for (int i = 0; i < 3; i++) {

    std::ostringstream oss;
    oss << "save" << (i + 1) << ".dat";
    file_name = oss.str();

    if (savegames[i] != NULL) {
      delete savegames[i];
    }
    savegames[i] = new Savegame(file_name);

    // get the data

    // player name
    std::string player_name;
    if (!savegames[i]->is_empty()) {
      player_name = savegames[i]->get_string(Savegame::PLAYER_NAME);
    }
    else {
      player_name = (std::string) "- " + StringResource::get_string("selection_menu.empty") + " -";
    }

    if (text_player_names[i] != NULL) {
      delete text_player_names[i];
    }

    text_player_names[i] = new TextSurface(87, 88 + i * 27,
					   TextSurface::ALIGN_LEFT, TextSurface::ALIGN_MIDDLE);
    text_player_names[i]->set_text(player_name);

    // hearts
    if (hearts_views[i] != NULL) {
      delete hearts_views[i];
      hearts_views[i] = NULL;
    }

    if (!savegames[i]->is_empty()) {
      hearts_views[i] = new HeartsView(savegames[i]->get_equipment(), 168, 78 + i * 27);
    }
  }
}

/**
 * @brief Goes to a next phase in the menu.
 * @param next_phase the next phase
 */
void SelectionMenu::set_next_phase(SelectionMenuPhase *next_phase) {
  this->next_phase = next_phase;
}

/**
 * @brief Stops the selection menu and starts a game instead.
 * @param savegame the savegame to play
 */
void SelectionMenu::start_game(Savegame *savegame) {
  set_next_screen(new Game(solarus, savegame));
}

/**
 * @brief Ends the program.
 */
void SelectionMenu::set_exiting() {
  solarus->set_exiting();
}

/**
 * @brief Updates the selection menu.
 */
void SelectionMenu::update() {

  // move the clouds
  uint32_t now = System::now();
  while (now >= next_cloud_move) {
    
    for (int i = 0; i < 16; i++) {
      cloud_positions[i].add_x(1);
      cloud_positions[i].add_y(-1);

      if (cloud_positions[i].get_x() >= 320) {
	cloud_positions[i].set_x(0);
      }

      if (cloud_positions[i].get_y() <= -44) {
	cloud_positions[i].set_y(240 - 44);
      }
    }

    next_cloud_move += 100;
  }

  // update the animation of the cursor
  cursor_sprite->update();

  // update the transition (if any)
  if (transition != NULL) {
    transition->update();
  }

  // make the phase-specific update
  current_phase->update();

  // see if the phase is changed
  if (next_phase != NULL) {
    delete current_phase;
    current_phase = next_phase;
    next_phase = NULL;
  }
}

/**
 * @brief Displays this screen.
 * @param screen_surface the surface where to display
 */
void SelectionMenu::display(Surface *screen_surface) {

  // background color
  surface->fill_with_color(background_color);

  // display the clouds
  Rectangle position;
  for (int i = 0; i < 16; i++) {

    position = cloud_positions[i];

    cloud_img->blit(surface, position);

    if (cloud_positions[i].get_x() >= 320 - 80) {
      position.set_xy(cloud_positions[i].get_x() - 320, cloud_positions[i].get_y());
      cloud_img->blit(surface, position);

      if (cloud_positions[i].get_y() <= 0) {
	position.set_xy(cloud_positions[i].get_x() - 320, cloud_positions[i].get_y() + 240);
	cloud_img->blit(surface, position);
      }
    }

    if (cloud_positions[i].get_y() <= 0) {
      position.set_xy(cloud_positions[i].get_x(), cloud_positions[i].get_y() + 240);
      cloud_img->blit(surface, position);
    }
  }

  // display the background image
  position.set_xy(37, 38);
  position.set_size(246, 165);
  background_img->blit(surface, position);

  // make the phase-specific displaying
  display_title_text(surface);
  current_phase->display(surface);
  display_transition(surface);

  // blit everything on the screen
  screen_surface->fill_with_color(Color::get_black());
  surface->blit(screen_surface);
}

/**
 * @brief This function is called by the program main loop when there is an input event.
 * @param event the event to handle
 */
void SelectionMenu::notify_event(InputEvent &event) {

  if (event.is_keyboard_key_pressed(InputEvent::KEY_ESCAPE)) {
    solarus->set_exiting();
  }
  else {

    bool notify = true;
    if (event.is_joypad_axis_moved() && !event.is_joypad_axis_centered()) {

      // the cursor moves to much when using a joypad axis
      uint32_t now = System::now();
      if (now > allow_cursor_date) {
	allow_cursor_date = now + 200;
      }
      else {
	notify = false;
      }
    }

    if (notify) {
      // notify the current phase
      current_phase->notify_event(event);
    }
  }
}

/**
 * @brief Starts a transition effect in the menu.
 * @param transition the transition effect
 */
void SelectionMenu::set_transition(Transition *transition) {

  delete this->transition;
  this->transition = transition;
}

/**
 * @brief Returns whether the selection menu is currently displaying a transition effect.
 * @return true if there is a transition
 */
bool SelectionMenu::has_transition() {

  return transition != NULL && transition->is_started();
}

/**
 * @brief Returns whether the current transition effect (if any) is finished.
 * @return true if the transition effect is finished
 */
bool SelectionMenu::is_transition_finished() {

  return transition != NULL && transition->is_finished();
}


/**
 * @brief Sets the text of title to display on the top of the screen.
 * @param title_string_key key of the title string
 */
void SelectionMenu::set_title_text(const std::string &title_string_key) {

  title_text->set_text(StringResource::get_string(title_string_key));
}


/**
 * @brief Sets the text of the two options to display in the bottom of the screen.
 *
 * These texts may be empty strings.
 *
 * @param left_string_key key of the first option string (on the left)
 * @param right_string_key key of the second option string (on the right)
 */
void SelectionMenu::set_bottom_options(const std::string &left_string_key, const std::string &right_string_key) {

  if (left_string_key.size() != 0) {
    option1_text->set_text(StringResource::get_string(left_string_key));
  }
  else {
    option1_text->set_text("");
  }

  if (right_string_key.size() != 0) {
    option2_text->set_text(StringResource::get_string(right_string_key));
  }
  else {
    option2_text->set_text("");
  }
}

/**
 * @brief Returns the current position of the savegame cursor.
 * @return the cursor position (1 to 5)
 */
int SelectionMenu::get_cursor_position() {
  return cursor_position;
}

/**
 * @brief Sets the current position of the savegame cursor.
 * @param cursor_position the new position (1 to 5)
 */
void SelectionMenu::set_cursor_position(int cursor_position) {
  this->cursor_position = cursor_position;
  cursor_sprite->restart_animation();
}

/**
 * @brief Returns the savegame cursor sprite.
 * @return the cursor sprite
 */
Sprite * SelectionMenu::get_cursor_sprite() {
  return cursor_sprite;
}

/**
 * @brief Moves the savegame cursor upwards.
 */
void SelectionMenu::move_cursor_up() {

  play_cursor_sound();

  int cursor_position = this->get_cursor_position() - 1;

  if (cursor_position == 0) {
    cursor_position = 4;
  }
  else if (cursor_position == 4) {
    cursor_position = 3;
  }

  set_cursor_position(cursor_position);
}

/**
 * @brief Moves the savegame cursor downwards.
 */
void SelectionMenu::move_cursor_down() {

  play_cursor_sound();

  int cursor_position = this->get_cursor_position() + 1;

  if (cursor_position >= 5) {
    cursor_position = 1;
  }

  set_cursor_position(cursor_position);
}

/**
 * @brief Tries to move the savegame cursor to the left or to the right.
 */
void SelectionMenu::move_cursor_left_or_right() {

  if (get_cursor_position() == 4) {
    play_cursor_sound();
    set_cursor_position(5);
  }
  else if (get_cursor_position() == 5) {
    play_cursor_sound();
    set_cursor_position(4);
  }
}

/**
 * @brief Returns the three savegames of the menu.
 * @return the savegames
 */
Savegame ** SelectionMenu::get_savegames() {
  return savegames;
}

/**
 * @brief Returns a savegame of the menu.
 * @param save_number number of the savegame to get (0 to 2)
 * @return the savegame
 */
Savegame * SelectionMenu::get_savegame(int save_number) {
  return savegames[save_number];
}

/**
 * @brief Reloads the three savegames of the menu.
 *
 * This function should be called when a savegame is created or deleted.
 */
void SelectionMenu::reload_savegames() {
  read_savegames();
}

/**
 * @brief Draws the current transition effect (if any).
 * @param destination_surface the surface to draw
 */
void SelectionMenu::display_transition(Surface *destination_surface) {

  if (transition != NULL && transition->is_started()) {
    transition->display(destination_surface);
  }
}

/**
 * @brief Draws the title text.
 * @param destination_surface the surface to draw
 */
void SelectionMenu::display_title_text(Surface *destination_surface) {
  
  title_text->display(destination_surface);
}

/**
 * @brief Displays a savegame.
 * @param destination_surface the surface to draw
 * @param save_number number of the savegame to display (0 to 2)
 */
void SelectionMenu::display_savegame(Surface *destination_surface, int save_number) {

  // draw the container
  Rectangle position(57, 75 + save_number * 27);
  save_container_img->blit(destination_surface, position);

  // draw the player's name
  text_player_names[save_number]->display(destination_surface);

  // draw the life
  if (hearts_views[save_number] != NULL) {
    hearts_views[save_number]->display(destination_surface);
  }
}

/**
 * @brief Displays the number of a savegame.
 *
 * This function is separate from the display_savegame() function
 * because the cursor has to be displayed after the savegame images
 * but before the savegame number.
 *
 * @param destination_surface the surface to draw
 * @param save_number number to display (0 to 2)
 */
void SelectionMenu::display_savegame_number(Surface *destination_surface, int save_number) {

  Rectangle position(62, 80 + 27 * save_number);
  number_imgs[save_number]->blit(destination_surface, position);
}

/**
 * @brief Displays the two options at the bottom of the menu.
 *
 * If an option text is empty, it is not displayed.
 *
 * @param destination_surface the surface to draw
 */
void SelectionMenu::display_bottom_options(Surface *destination_surface) {

  Rectangle position(0, 158);

  if (!option1_text->is_empty()) {
    position.set_x(57);
    option_container_img->blit(destination_surface, position);
    option1_text->display(destination_surface);
  }

  if (!option2_text->is_empty()) {
    position.set_x(165);
    option_container_img->blit(destination_surface, position);
    option2_text->display(destination_surface);
  }
}

/**
 * @brief Displays the savegame cursor (i.e. the cursor to select a file)
 * at its current position.
 * @param destination_surface the surface to draw
 */
void SelectionMenu::display_savegame_cursor(Surface *destination_surface) {

  Rectangle position;

  if (cursor_position != 5) {
    position.set_x(58);
  }
  else {
    position.set_x(166);
  }

  if (cursor_position < 4) {
    position.set_y(49 + cursor_position * 27);
  }
  else {
    position.set_y(159);
  }
  cursor_sprite->display(destination_surface, position.get_x(), position.get_y());
}

/**
 * @brief Plays the "cursor" sound.
 */
void SelectionMenu::play_cursor_sound() {
  Sound::play("cursor");
}

/**
 * @brief Plays the "ok" sound.
 */
void SelectionMenu::play_ok_sound() {
  Sound::play("ok");
}

/**
 * @brief Plays the "error" sound.
 */
void SelectionMenu::play_error_sound() {
  Sound::play("wrong");
}

/**
 * @brief Plays the "erase" sound.
 */
void SelectionMenu::play_erase_sound() {
  Sound::play("boss_killed");
}

/**
 * @brief Plays the "letter" sound.
 */
void SelectionMenu::play_letter_sound() {
  Sound::play("danger");
}

