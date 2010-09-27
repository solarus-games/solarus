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
#include "hud/PauseIcon.h"
#include "KeysEffect.h"
#include "Sprite.h"
#include "lowlevel/Surface.h"

/**
 * @brief Constructor.
 * @param game the current game
 * @param x x coordinate of the top-left corner of the icon on the destination surface
 * @param y y coordinate of the top-left corner of the icon on the destination surface
 */
PauseIcon::PauseIcon(Game &game, int x, int y):
  HudElement(game, x, y, 72, 24) {

  // static icon
  this->pause_key_effect_displayed = keys_effect->get_pause_key_effect();
  this->img_pause_icon = new Surface("pause_icon.png", Surface::DIR_LANGUAGE);

  // flipping icon
  this->is_flipping = false;
  this->sprite_pause_icon = new Sprite("hud/pause_icon_flip");

  rebuild();
}

/**
 * @brief Destructor.
 */
PauseIcon::~PauseIcon() {
  delete sprite_pause_icon;
  delete img_pause_icon;
}

/**
 * @brief This function is called repeatedly.
 *
 * It checks whether the icon should change.
 */
void PauseIcon::update() {

  HudElement::update();

  bool need_rebuild = false;

  // text shown
  if (!is_flipping) {

    KeysEffect::PauseKeyEffect pause_key_effect = keys_effect->get_pause_key_effect();

    if (pause_key_effect_displayed != pause_key_effect) {

      pause_key_effect_displayed = pause_key_effect;

      sprite_pause_icon->start_animation();
      is_flipping = true;
      need_rebuild = true;
    }
  }
  else {
    sprite_pause_icon->update();
    need_rebuild = true;

    if (sprite_pause_icon->is_animation_finished()) {
      is_flipping = false;
    }
  }

  // icon opacity
  if (keys_effect->is_pause_key_enabled() && get_opacity() == 128) {
    set_opacity(255);
  }
  else if (!keys_effect->is_pause_key_enabled() && get_opacity() == 255) {
    set_opacity(128);
  }

  // redraw the surface if something has changed
  if (need_rebuild) {
    rebuild();
  }
}

/**
 * @brief Redraws the icon on the surface.
 */
void PauseIcon::rebuild() {

  HudElement::rebuild();
  
  if (!is_flipping) {

    // draw the static image of the icon, with the current
    // pause key effect

    Rectangle icon_position(0, 0, 72, 24);
    
    // draw the pause icon with some text (Save, Return...)
    icon_position.set_y(24 * pause_key_effect_displayed);

    img_pause_icon->blit(icon_position, surface_drawn);
  }
  else {
    // draw the flipping sprite
    sprite_pause_icon->display(surface_drawn, 24, 0);
  }
}

