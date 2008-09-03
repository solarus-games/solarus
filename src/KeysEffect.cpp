#include "KeysEffect.h"

/**
 * Constructor.
 */
KeysEffect::KeysEffect(void) {
  
  action_key_effect = ACTION_KEY_NONE;
  action_key_enabled = true;

  sword_key_effect = SWORD_KEY_NONE;
  sword_key_enabled = true;

  pause_key_effect = PAUSE_KEY_PAUSE;
  pause_key_enabled = true;

  item_keys_enabled = true;
}

/**
 * Destructor.
 */
KeysEffect::~KeysEffect(void) {

}

// action key

/**
 * Returns the current effect of the action key.
 * @return the current effect of the action key
 */
ActionKeyEffect KeysEffect::get_action_key_effect(void) {
  return action_key_effect;
}

/**
 * Sets the current effect of the action key.
 * @param action_key_effect the current effect of the action key
 */
void KeysEffect::set_action_key_effect(ActionKeyEffect action_key_effect) {
  this->action_key_effect = action_key_effect;
}

/**
 * Returns whether the action key is enabled.
 * @return true if the action key is enabled, false otherwise
 */
bool KeysEffect::is_action_key_enabled(void) {
  return action_key_enabled;
}

/**
 * Sets whether the action key is enabled.
 * @param enable true to enable the action key, false to disable it
 */
void KeysEffect::set_action_key_enabled(bool enable) {
  this->action_key_enabled = enable;
}

/**
 * Saves the current effect of the action key.
 * Call restore_action_key_effect() to restore the action key saved here.
 */
void KeysEffect::save_action_key_effect(void) {
  this->action_key_effect_saved = get_action_key_effect();
}

/**
 * Restores the action key effect saved by the last
 * save_action_key_effect() call.
 */
void KeysEffect::restore_action_key_effect(void) {
  this->action_key_effect = action_key_effect_saved;
}

// sword key

/**
 * Returns the current effect of the sword key.
 * @return the current effect of the sword key
 */
SwordKeyEffect KeysEffect::get_sword_key_effect(void) {
  return sword_key_effect;
}

/**
 * Sets the current effect of the sword key.
 * @param sword_key_effect the current effect of the sword key
 */
void KeysEffect::set_sword_key_effect(SwordKeyEffect sword_key_effect) {
  this->sword_key_effect = sword_key_effect;
}

/**
 * Returns whether the sword key is enabled.
 * @return true if the sword key is enabled, false otherwise
 */
bool KeysEffect::is_sword_key_enabled(void) {
  return sword_key_enabled;
}

/**
 * Sets whether the sword key is enabled.
 * @param enable true to enable the sword key, false to disable it
 */
void KeysEffect::set_sword_key_enabled(bool enable) {
  this->sword_key_enabled = enable;
}

/**
 * Saves the current effect of the sword key.
 * Call restore_sword_key_effect to restore the sword key saved here.
 */
void KeysEffect::save_sword_key_effect(void) {
  this->sword_key_effect_saved = get_sword_key_effect();
}

/**
 * Restores the sword key effect saved by the last
 * save_sword_key_effect() call.
 */
void KeysEffect::restore_sword_key_effect(void) {
  this->sword_key_effect = sword_key_effect_saved;
}


// pause key

/**
 * Returns the current effect of the pause key.
 * @return the current effect of the pause key
 */
PauseKeyEffect KeysEffect::get_pause_key_effect(void) {
  return pause_key_effect;
}

/**
 * Sets the current effect of the pause key.
 * @param pause_key_effect the current effect of the pause key
 */
void KeysEffect::set_pause_key_effect(PauseKeyEffect pause_key_effect) {
  this->pause_key_effect = pause_key_effect;
}

/**
 * Returns whether the pause key is enabled.
 * @return true if the pause key is enabled, false otherwise
 */
bool KeysEffect::is_pause_key_enabled(void) {
  return pause_key_enabled;
}

/**
 * Sets whether the pause key is enabled.
 * @param enable true to enable the pause key, false to disable it
 */
void KeysEffect::set_pause_key_enabled(bool enable) {
  this->pause_key_enabled = enable;
}

// item keys

/**
 * Returns whether the two item keys are enabled.
 * @return true if the two item keys are enabled, false otherwise
 */
bool KeysEffect::are_item_keys_enabled(void) {
  return item_keys_enabled;
}

/**
 * Sets whether the two item keys are enabled.
 * @param enable true to enable the two item keys, false to disable them
 */
void KeysEffect::set_item_keys_enabled(bool enable) {
  this->item_keys_enabled = enable;
}

/**
 * Enables or disables the 5 keys.
 * @param enable true to enable the 5 keys, false to disable them
 */
void KeysEffect::set_all_keys_enabled(bool enable) {
  set_action_key_enabled(enable);
  set_sword_key_enabled(enable);
  set_pause_key_enabled(enable);
  set_item_keys_enabled(enable);
}
