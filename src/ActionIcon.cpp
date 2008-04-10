/**
 * This module defines the class ActionIcon.
 */
#include "ActionIcon.h"
#include "KeysEffect.h"

/**
 * Constructor.
 * @param keys_effect current effect of the main keys
 * (here we are interested in the action key)
 * @param x x coordinate of the top-left corner of the icon on the destination surface
 * @param y y coordinate of the top-left corner of the icon on the destination surface
 */
ActionIcon::ActionIcon(KeysEffect *keys_effect, int x, int y):
  HudElement(x, y, 72, 24) {

}

/**
 * Destructor.
 */
ActionIcon::~ActionIcon(void) {

}

/**
 * This function is called repeatedly, it checks whether
 * the icon should change.
 */
void ActionIcon::update(void) {

}

/**
 * Redraws the icon on the surface.
 */
void ActionIcon::rebuild(void) {

}
