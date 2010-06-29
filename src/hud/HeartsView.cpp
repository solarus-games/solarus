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
#include "hud/HeartsView.h"
#include "Game.h"
#include "Equipment.h"
#include "Sprite.h"
#include "lowlevel/Surface.h"
#include "lowlevel/System.h"

/**
 * @brief Position of the hearts in the PNG image.
 */
const Rectangle HeartsView::full_heart_position(27, 0, 9, 9);
const Rectangle HeartsView::fraction_heart_positions[3] = {
  Rectangle(0, 0, 9, 9),
  Rectangle(9, 0, 9, 9),
  Rectangle(18, 0, 9, 9)
};

/**
 * @brief Creates a life view in a game.
 * @param game the current game (cannot be NULL)
 * @param x x coordinate of the top-left corner of this HUD element on the destination surface
 * @param y y coordinate of the top-left corner of this HUD element on the destination surface
 */
HeartsView::HeartsView(Game *game, int x, int y):
  HudElement(game, x, y, 90, 18) {

  create();
}

/**
 * @brief Creates a life view without game.
 * @param equipment the equipment object to represent
 * @param x x coordinate of the top-left corner of this HUD element on the destination surface
 * @param y y coordinate of the top-left corner of this HUD element on the destination surface
 */
HeartsView::HeartsView(Equipment *equipment, int x, int y):
  HudElement(equipment, x, y, 90, 18) {

  create();
}
 
/**
 * @brief Initializes the fields.
 *
 * This function is called by the constructors.
 */
void HeartsView::create(void) {

  uint32_t now = System::now();
  this->next_heart_update_date = now;
  this->next_danger_sound_date = now;

  this->img_hearts = new Surface("hud/hearts.png");
  this->empty_heart_sprite = new Sprite("hud/empty_heart");

  this->nb_max_hearts_displayed = equipment->get_max_hearts();
  this->nb_current_hearts_displayed = equipment->get_hearts();

  rebuild();
}


/**
 * @brief Destructor.
 */
HeartsView::~HeartsView(void) {
  delete img_hearts;
  delete empty_heart_sprite;
}

/**
 * @brief Updates the number of hearts displayed.
 */
void HeartsView::update(void) {

  HudElement::update();

  bool need_rebuild = false;

  // max hearts
  int nb_max_hearts = equipment->get_max_hearts();
  if (nb_max_hearts != nb_max_hearts_displayed) {
    nb_max_hearts_displayed = nb_max_hearts;
    need_rebuild = true;
  }

  // current hearts
  int nb_current_hearts = equipment->get_hearts();
  if (nb_current_hearts != nb_current_hearts_displayed && System::now() > next_heart_update_date) {

    next_heart_update_date = System::now() + 50;

    if (nb_current_hearts < nb_current_hearts_displayed) {
      nb_current_hearts_displayed--;
    }
    else {
      nb_current_hearts_displayed++;

      if (nb_current_hearts_displayed % 4 == 0 && game != NULL) {
	game->play_sound("heart");
      }
    }

    need_rebuild = true;
  }

  /* If we are in game and the player has less than 1/4 of his hearts left,
   * play the 'danger' sound and animate the empty hearts
   */
  if (game != NULL) {

    if (equipment->needs_hearts()) {

      if (empty_heart_sprite->get_current_animation() != "danger") {
	empty_heart_sprite->set_current_animation("danger");
      }
      empty_heart_sprite->update();

      uint32_t now = System::now();
      if (now > next_danger_sound_date) {
	next_danger_sound_date = now + 750;
	game->play_sound("danger");
      }

      need_rebuild = true;
    }
    else if (empty_heart_sprite->get_current_animation() != "normal") {
      empty_heart_sprite->set_current_animation("normal");
      need_rebuild = true;
    }
  }

  // redraw the surface if something has changed
  if (need_rebuild) {
    rebuild();
  }
}

/**
 * @brief Redraws the hearts on the surface.
 */
void HeartsView::rebuild(void) {

  HudElement::rebuild();
  
  // max hearts

  // blit each empty heart on the hearts surface
  
  for (int i = 0; i < nb_max_hearts_displayed; i++) {
    int x = (i % 10) * 9;
    int y = (i / 10) * 9;

    empty_heart_sprite->display(surface_drawn, x, y);
  }
  
  // current hearts
  Rectangle heart_position(0, 0, 9, 9);
  int i;
  for (i = 0; i < nb_current_hearts_displayed / 4; i++) {
    heart_position.set_x((i % 10) * 9);
    heart_position.set_y((i / 10) * 9);
    img_hearts->blit(full_heart_position, surface_drawn, heart_position);
  }

  // last heart: fraction of heart
  int remaining_fraction = nb_current_hearts_displayed % 4;
  if (remaining_fraction != 0) {

    heart_position.set_x((i % 10) * 9);
    img_hearts->blit(fraction_heart_positions[remaining_fraction - 1], surface_drawn, heart_position);
  }
}

/**
 * @brief Returns whether this hud element is visible.
 *
 * The display() function does nothing if this function returns false.
 *
 * @return true if this hud element is visible, i.e. if equipment is not NULL
 */
bool HeartsView::is_visible(void) {
  return equipment != NULL;
}

