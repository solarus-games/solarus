/**
 * This module defines the class PauseIcon.
 */
#include "PauseIcon.h"
#include "KeysEffect.h"

/**
 * Constructor.
 * @param keys_effect current effect of the main keys
 * (here we are interested in the pause key)
 * @param x x coordinate of the top-left corner of the icon on the destination surface
 * @param y y coordinate of the top-left corner of the icon on the destination surface
 */
PauseIcon::PauseIcon(KeysEffect *keys_effect, int x, int y):
  HudElement(x, y, 72, 24) {

}

/**
 * Destructor.
 */
PauseIcon::~PauseIcon(void) {

}

/**
 * This function is called repeatedly, it checks whether
 * the icon should change.
 */
void PauseIcon::update(void) {

}

/**
 * Redraws the icon on the surface.
 */
void PauseIcon::rebuild(void) {

}
