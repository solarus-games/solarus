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

const std::string EnumInfoTraits<KeysEffect::ActionKeyEffect>::pretty_name = "action key effect";

/**
 * \brief Lua name of each value of the ActionKeyEffect enum.
 */
const EnumInfo<KeysEffect::ActionKeyEffect>::names_type EnumInfoTraits<KeysEffect::ActionKeyEffect>::names = {
  { KeysEffect::ACTION_KEY_NONE, "" },
  { KeysEffect::ACTION_KEY_NEXT, "next" },
  { KeysEffect::ACTION_KEY_LOOK, "look" },
  { KeysEffect::ACTION_KEY_OPEN, "open" },
  { KeysEffect::ACTION_KEY_LIFT, "lift" },
  { KeysEffect::ACTION_KEY_THROW, "throw" },
  { KeysEffect::ACTION_KEY_GRAB, "grab" },
  { KeysEffect::ACTION_KEY_SPEAK, "speak" },
  { KeysEffect::ACTION_KEY_SWIM, "swim"}
};

const std::string EnumInfoTraits<KeysEffect::SwordKeyEffect>::pretty_name = "sword key effect";

/**
 * \brief Lua name of each value of the SwordKeyEffect enum.
 */
const EnumInfo<KeysEffect::SwordKeyEffect>::names_type EnumInfoTraits<KeysEffect::SwordKeyEffect>::names = {
  { KeysEffect::SWORD_KEY_NONE, "" },
  { KeysEffect::SWORD_KEY_SWORD, "sword" }
};

const std::string EnumInfoTraits<KeysEffect::PauseKeyEffect>::pretty_name = "pause key effect";

/**
 * \brief Lua name of each value of the PauseKeyEffect enum.
 */
const EnumInfo<KeysEffect::PauseKeyEffect>::names_type EnumInfoTraits<KeysEffect::PauseKeyEffect>::names = {
  { KeysEffect::PAUSE_KEY_NONE, "" },
  { KeysEffect::PAUSE_KEY_PAUSE, "pause" },
  { KeysEffect::PAUSE_KEY_RETURN, "return" }
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

