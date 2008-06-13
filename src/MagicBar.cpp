#include "MagicBar.h"
#include "FileTools.h"
#include "Equipment.h"
#include "ZSDX.h"
#include "Game.h"
#include "GameResource.h"
#include "Sound.h"
#include "Sprite.h"

/**
 * Constructor.
 * @param equipment the player's equipment
 * @param x x coordinate of the top-left corner of the magic bar on the destination surface
 * @param y y coordinate of the top-left corner of the magic bar on the destination surface
 */
MagicBar::MagicBar(Equipment *equipment, int x, int y):
  HudElement(x, y, 88, 8),
  equipment(equipment),
  next_magic_update_date(SDL_GetTicks()) {

  img_magic_bar = FileTools::open_image("hud/magic_bar.png");
  sprite_magic_bar_container = new Sprite("hud/magic_bar");
  sound_magic_bar = zsdx->game_resource->get_sound("magic_bar");

  current_magic_displayed = equipment->get_magic();
  max_magic_displayed = equipment->get_max_magic();
  is_magic_decreasing = equipment->is_magic_decreasing();
}

/**
 * Destructor.
 */
MagicBar::~MagicBar(void) {
  SDL_FreeSurface(img_magic_bar);
  delete sprite_magic_bar_container;
}

/**
 * Returns whether this hud element is visible.
 * The display() function does nothing if this function
 * returns false.
 * @return true if this hud element is visible, i.e. if
 * the player has a magic bar
 */
bool MagicBar::is_visible(void) {
  return max_magic_displayed > 0;
}

/**
 * Updates the magic bar level displayed.
 */
void MagicBar::update(void) {

  bool need_rebuild = false;

  // max magic
  int max_magic = equipment->get_max_magic();
  if (max_magic != max_magic_displayed) {
    max_magic_displayed = max_magic;
    sprite_magic_bar_container->set_current_direction(max_magic_displayed / 42 - 1);
    need_rebuild = true;
  }

  // are the magic points decreasing continuously?
  if (equipment->is_magic_decreasing()) {

    // animate the magic bar 
    if (!is_magic_decreasing) {
      is_magic_decreasing = true;
      sprite_magic_bar_container->set_current_animation("decreasing");
    }

    // suspend the magic bar animation if the game is suspended
    if (zsdx->game->is_suspended() && !sprite_magic_bar_container->is_suspended()) {
      sprite_magic_bar_container->set_current_frame(1);
      sprite_magic_bar_container->set_suspended(true);
    }
    else if (sprite_magic_bar_container->is_suspended() && !zsdx->game->is_suspended()) {
      sprite_magic_bar_container->set_suspended(false);
    }

    sprite_magic_bar_container->update_current_frame();
    need_rebuild = true;
  }
  else if (!equipment->is_magic_decreasing() && is_magic_decreasing) {

    // stop the magic bar animation
    is_magic_decreasing = false;
    sprite_magic_bar_container->set_current_animation("normal");
    need_rebuild = true;
  }

  // current magic
  int current_magic = equipment->get_magic();
  if (current_magic < current_magic_displayed) {
    current_magic_displayed = current_magic;
    need_rebuild = true;
  }
  else if (current_magic > current_magic_displayed
	   && SDL_GetTicks() > next_magic_update_date) {

    next_magic_update_date = SDL_GetTicks() + 20;
    current_magic_displayed++;
    need_rebuild = true;

    // play the magic bar sound
    if ((current_magic - current_magic_displayed) % 10 == 1) {
      sound_magic_bar->play();
    }
  }

  // redraw the surface if something has changed
  if (need_rebuild) {
    rebuild();
  }
}

/**
 * Redraws the magic bar on the surface.
 */
void MagicBar::rebuild(void) {

  HudElement::rebuild();

  if (!is_visible()) {
    return;
  }
  
  // max magic
  sprite_magic_bar_container->display(surface_drawn, 0, 0);

  // current magic
  SDL_Rect current_magic_position = {46, 24, 0, 8};
  current_magic_position.w = 2 + current_magic_displayed;
  SDL_BlitSurface(img_magic_bar, &current_magic_position, surface_drawn, NULL);
}
