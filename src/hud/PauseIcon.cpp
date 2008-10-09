#include "hud/PauseIcon.h"
#include "KeysEffect.h"
#include "ResourceManager.h"
#include "Sprite.h"

/**
 * Constructor.
 * @param keys_effect current effect of the main keys
 * (here we are interested in the pause key)
 * @param x x coordinate of the top-left corner of the icon on the destination surface
 * @param y y coordinate of the top-left corner of the icon on the destination surface
 */
PauseIcon::PauseIcon(KeysEffect *keys_effect, int x, int y):
  HudElement(x, y, 72, 24), keys_effect(keys_effect) {

  // static icon
  this->pause_key_effect_displayed = keys_effect->get_pause_key_effect();
  this->img_pause_icon = ResourceManager::load_image("hud/pause_icon.png");

  this->is_enabled = true;

  // flipping icon
  this->is_flipping = false;
  this->sprite_pause_icon = new Sprite("hud/pause_icon_flip");

  rebuild();
}

/**
 * Destructor.
 */
PauseIcon::~PauseIcon(void) {
  delete sprite_pause_icon;
  SDL_FreeSurface(img_pause_icon);
}

/**
 * This function is called repeatedly, it checks whether
 * the icon should change.
 */
void PauseIcon::update(void) {

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

    if (sprite_pause_icon->is_over()) {
      is_flipping = false;
    }
  }

  // icon opacity
  if (keys_effect->is_pause_key_enabled() && !is_enabled) {
    SDL_SetAlpha(surface_drawn, SDL_SRCALPHA, 255);
    is_enabled = true;
    need_rebuild = true;
  }
  else if (!keys_effect->is_pause_key_enabled() && is_enabled) {
    SDL_SetAlpha(surface_drawn, SDL_SRCALPHA, 128);
    is_enabled = false;
    need_rebuild = true;
  }

  // redraw the surface if something has changed
  if (need_rebuild) {
    rebuild();
  }
}

/**
 * Redraws the icon on the surface.
 */
void PauseIcon::rebuild(void) {

  HudElement::rebuild();
  
  if (!is_flipping) {

    // draw the static image of the icon, with the current
    // pause key effect

    SDL_Rect icon_position = {0, 0, 72, 24};
    
    // draw the pause icon with some text (Save, Return...)
    icon_position.y = 24 * pause_key_effect_displayed;

    SDL_BlitSurface(img_pause_icon, &icon_position, surface_drawn, NULL);
  }
  else {
    // draw the flipping sprite
    sprite_pause_icon->display(surface_drawn, 24, 0);
  }
}
