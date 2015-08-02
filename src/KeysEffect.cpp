/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/KeysEffect.h"

namespace Solarus {

/**
 * \brief Lua name of each value of the ActionKeyEffect enum.
 */
const std::vector<std::string> KeysEffect::action_key_effect_names = {
  "",
  "next",
  "look",
  "open",
  "lift",
  "throw",
  "grab",
  "speak",
  "swim"
};

/**
 * \brief Lua name of each value of the SwordKeyEffect enum.
 */
const std::vector<std::string> KeysEffect::sword_key_effect_names = {
  "",
  "sword"
};

/**
 * \brief Lua name of each value of the PauseKeyEffect enum.
 */
const std::vector<std::string> KeysEffect::pause_key_effect_names = {
  "",
  "pause",
  "return"
};

/**
 * \brief Constructor.
 */
KeysEffect::KeysEffect():
  action_key_effect(ACTION_KEY_NONE),
  action_key_effect_saved(ACTION_KEY_NONE),
  action_key_enabled(true),
  sword_key_effect(SWORD_KEY_NONE),
  sword_key_effect_saved(SWORD_KEY_NONE),
  sword_key_enabled(true),
  pause_key_effect(PAUSE_KEY_PAUSE),
  pause_key_effect_saved(PAUSE_KEY_PAUSE),
  pause_key_enabled(true),
  item_keys_enabled(true) {

}

// action key

/**
 * \brief Returns the current effect of the action key.
 * \return the current effect of the action key
 */
KeysEffect::ActionKeyEffect KeysEffect::get_action_key_effect() {
  return action_key_effect;
}
/**
 * \brief Sets the current effect of the action key.
 * \param action_key_effect the current effect of the action key
 */
void KeysEffect::set_action_key_effect(KeysEffect::ActionKeyEffect action_key_effect) {
  this->action_key_effect = action_key_effect;
}

/**
 * \brief Returns whether the action key is enabled.
 * \return true if the action key is enabled, false otherwise
 */
bool KeysEffect::is_action_key_enabled() {
  return action_key_enabled;
}

/**
 * \brief Sets whether the action key is enabled.
 * \param enable true to enable the action key, false to disable it
 */
void KeysEffect::set_action_key_enabled(bool enable) {
  this->action_key_enabled = enable;
}

/**
 * \brief Saves the current effect of the action key.
 *
 * Call restore_action_key_effect() to restore the action key saved here.
 */
void KeysEffect::save_action_key_effect() {
  this->action_key_effect_saved = get_action_key_effect();
}

/**
 * \brief Restores the action key effect saved by the last
 * call to save_action_key_effect().
 */
void KeysEffect::restore_action_key_effect() {
  this->action_key_effect = action_key_effect_saved;
}

/**
 * \brief Returns whether the action key is currently taking effect
 * on the entity the hero is facing.
 *
 * Such action key effects are ACTION_KEY_LOOK, ACTION_KEY_OPEN,
 * ACTION_KEY_LIFT, ACTION_KEY_SPEAK and ACTION_KEY_GRAB.
 *
 * \return true if the action key is acting on the facing entity
 */
bool KeysEffect::is_action_key_acting_on_facing_entity() {
  return action_key_effect == ACTION_KEY_LOOK
    || action_key_effect == ACTION_KEY_OPEN
    || action_key_effect == ACTION_KEY_LIFT
    || action_key_effect == ACTION_KEY_SPEAK
    || action_key_effect == ACTION_KEY_GRAB;
}

/**
 * \brief Returns the name of an action command effect.
 * \param effect An effect of the action command.
 * \return The name of this effect, or an empty string if the effect is ACTION_KEY_NONE.
 */
const std::string& KeysEffect::get_action_key_effect_name(
    ActionKeyEffect effect) {

  return action_key_effect_names[effect];
}

/**
 * \brief Returns an action command effect given its Lua name.
 * \param effect_name Lua name of an action command effect.
 * \return The corresponding action command effect, or ACTION_KEY_NONE
 * if the name is invalid.
 */
KeysEffect::ActionKeyEffect KeysEffect::get_action_key_effect_by_name(
    const std::string& effect_name) {

  for (int i = 0; i < ACTION_KEY_NB; i++) {
    if (action_key_effect_names[i] == effect_name) {
      return ActionKeyEffect(i);
    }
  }
  return ACTION_KEY_NONE;
}

// sword key

/**
 * \brief Returns the current effect of the sword key.
 * \return the current effect of the sword key
 */
KeysEffect::SwordKeyEffect KeysEffect::get_sword_key_effect() {
  return sword_key_effect;
}

/**
 * \brief Sets the current effect of the sword key.
 * \param sword_key_effect the current effect of the sword key
 */
void KeysEffect::set_sword_key_effect(KeysEffect::SwordKeyEffect sword_key_effect) {
  this->sword_key_effect = sword_key_effect;
}

/**
 * \brief Returns whether the sword key is enabled.
 * \return true if the sword key is enabled, false otherwise
 */
bool KeysEffect::is_sword_key_enabled() {
  return sword_key_enabled;
}

/**
 * \brief Sets whether the sword key is enabled.
 * \param enable true to enable the sword key, false to disable it
 */
void KeysEffect::set_sword_key_enabled(bool enable) {
  this->sword_key_enabled = enable;
}

/**
 * \brief Saves the current effect of the sword key.
 *
 * Call restore_sword_key_effect to restore the sword key saved here.
 */
void KeysEffect::save_sword_key_effect() {
  this->sword_key_effect_saved = get_sword_key_effect();
}

/**
 * \brief Restores the sword key effect saved by the last
 * call to save_sword_key_effect().
 */
void KeysEffect::restore_sword_key_effect() {
  this->sword_key_effect = sword_key_effect_saved;
}

/**
 * \brief Returns the name of an attack command effect.
 * \param effect An effect of the attack command.
 * \return The name of this effect, or an empty string if the effect is SWORD_KEY_NONE.
 */
const std::string& KeysEffect::get_sword_key_effect_name(
    SwordKeyEffect effect) {

  return sword_key_effect_names[effect];
}

/**
 * \brief Returns an attack command effect given its Lua name.
 * \param effect_name Lua name of an attack command effect.
 * \return The corresponding attack command effect, or SWORD_KEY_NONE
 * if the name is invalid.
 */
KeysEffect::SwordKeyEffect KeysEffect::get_sword_key_effect_by_name(
    const std::string& effect_name) {

  for (int i = 0; i < SWORD_KEY_NB; i++) {
    if (sword_key_effect_names[i] == effect_name) {
      return SwordKeyEffect(i);
    }
  }
  return SWORD_KEY_NONE;
}

// pause key

/**
 * \brief Returns the current effect of the pause key.
 * \return the current effect of the pause key
 */
KeysEffect::PauseKeyEffect KeysEffect::get_pause_key_effect() {
  return pause_key_effect;
}

/**
 * \brief Sets the current effect of the pause key.
 * \param pause_key_effect the current effect of the pause key
 */
void KeysEffect::set_pause_key_effect(KeysEffect::PauseKeyEffect pause_key_effect) {
  this->pause_key_effect = pause_key_effect;
}

/**
 * \brief Returns whether the pause key is enabled.
 * \return true if the pause key is enabled, false otherwise
 */
bool KeysEffect::is_pause_key_enabled() {
  return pause_key_enabled;
}

/**
 * \brief Sets whether the pause key is enabled.
 * \param enable true to enable the pause key, false to disable it
 */
void KeysEffect::set_pause_key_enabled(bool enable) {
  this->pause_key_enabled = enable;
}

/**
 * \brief Saves the current effect of the pause key.
 *
 * Call restore_pause_key_effect to restore the pause key saved here.
 */
void KeysEffect::save_pause_key_effect() {
  this->pause_key_effect_saved = get_pause_key_effect();
}

/**
 * \brief Restores the pause key effect saved by the last
 * call to save_pause_key_effect().
 */
void KeysEffect::restore_pause_key_effect() {
  this->pause_key_effect = pause_key_effect_saved;
}

/**
 * \brief Returns the name of a pause command effect.
 * \param effect An effect of the pause command.
 * \return The name of this effect, or an empty string if the effect is PAUSE_KEY_NONE.
 */
const std::string& KeysEffect::get_pause_key_effect_name(
    PauseKeyEffect effect) {

  return pause_key_effect_names[effect];
}

/**
 * \brief Returns a pause command effect given its Lua name.
 * \param effect_name Lua name of a pause command effect.
 * \return The corresponding pause command effect, or PAUSE_KEY_NONE
 * if the name is invalid.
 */
KeysEffect::PauseKeyEffect KeysEffect::get_pause_key_effect_by_name(
    const std::string& effect_name) {

  for (int i = 0; i < PAUSE_KEY_NB; i++) {
    if (pause_key_effect_names[i] == effect_name) {
      return PauseKeyEffect(i);
    }
  }
  return PAUSE_KEY_NONE;
}

// item keys

/**
 * \brief Returns whether the two item keys are enabled.
 * \return true if the two item keys are enabled, false otherwise
 */
bool KeysEffect::are_item_keys_enabled() {
  return item_keys_enabled;
}

/**
 * \brief Sets whether the two item keys are enabled.
 * \param enable true to enable the two item keys, false to disable them
 */
void KeysEffect::set_item_keys_enabled(bool enable) {
  this->item_keys_enabled = enable;
}

/**
 * \brief Enables or disables the 5 keys.
 * \param enable true to enable the 5 keys, false to disable them
 */
void KeysEffect::set_all_keys_enabled(bool enable) {
  set_action_key_enabled(enable);
  set_sword_key_enabled(enable);
  set_pause_key_enabled(enable);
  set_item_keys_enabled(enable);
}

}

