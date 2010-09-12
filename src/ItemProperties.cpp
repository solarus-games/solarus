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
#include "ItemProperties.h"
#include "lowlevel/IniFile.h"
#include <map>

/**
 * @brief Creates the properties of an item by parsing a group of the specified ini file
 *
 * The current group of the ini file describes the item to parse.
 *
 * @param ini the ini file to parse
 */
ItemProperties::ItemProperties(IniFile *ini) {

  name = ini->get_group();
  savegame_variable = ini->get_integer_value("savegame_variable", -1);
  nb_variants = ini->get_integer_value("nb_variants", 1);

  amounts = new int[nb_variants];
  probabilities = new int[nb_variants];
  for (int i = 0; i < nb_variants; i++) {

    std::ostringstream oss;
    oss << "amount_" << (i + 1);
    amounts[i] = ini->get_integer_value(oss.str(), 0);
    oss.str("");
    oss << "probability_" << (i + 1);
    probabilities[i] = ini->get_integer_value(oss.str(), 0);
  }
  amounts[0] = ini->get_integer_value("amount", 0);
  probabilities[0] = ini->get_integer_value("probability", 0);

  allow_assigned = ini->get_boolean_value("can_be_assigned", false);
  counter_savegame_variable = ini->get_integer_value("counter", -1);
  fixed_limit = ini->get_integer_value("limit", 0);
  limited_counter_name = ini->get_string_value("limit_for_counter", "");
  changed_counter_name = ini->get_string_value("changes_counter", "");
  disappears = ini->get_boolean_value("can_disappear", false);
  brandish_when_picked = ini->get_boolean_value("brandish_when_picked", true);
  sound_when_picked = ini->get_string_value("sound_when_picked", "picked_item");
  sound_when_brandished = ini->get_string_value("sound_when_brandished", "treasure");
  
  const std::string &shadow_name = ini->get_string_value("shadow", "big");

  std::map<std::string, ShadowSize> shadows;
  shadows["none"] = SHADOW_NONE;
  shadows["small"] = SHADOW_SMALL;
  shadows["big"] = SHADOW_BIG;
  shadow_size = shadows[shadow_name];
}

/**
 * Destructor.
 */
ItemProperties::~ItemProperties(void) {

  delete[] amounts;
  delete[] probabilities;
}

/**
 * @brief Returns the name identifying the item.
 * @return the name of the item
 */
const std::string & ItemProperties::get_name(void) {
  return name;
}

/**
 * @brief Returns whether the item possession state is saved.
 * @return true if the item is saved
 */
bool ItemProperties::is_saved(void) {
  return get_savegame_variable() != -1;
}

/**
 * @brief Returns the savegame variable where the item possession state
 * is saved.
 * @return the savegame variable that stores the item possession state,
 * or -1 if the item is not saved
 */
int ItemProperties::get_savegame_variable(void) {
  return savegame_variable;
}

/**
 * @brief Returns the number of variants of this item.
 * @return the number of variants
 */
int ItemProperties::get_nb_variants(void) {
  return nb_variants;
}

/**
 * @brief If this item has a counter, returns the savegame variable
 * that stores the value of this counter.
 * @return the savegame variable of the counter, or -1 if there is no counter
 * associated to this item.
 */
int ItemProperties::get_counter_savegame_variable(void) {
  return counter_savegame_variable;
}

/**
 * @brief Returns whether the counter associated to this item has a fixed limit.
 * @return true if there is a counter with a fixed limit
 */
bool ItemProperties::has_fixed_limit(void) {
  return get_fixed_limit() != 0;
}

/**
 * @brief Returns the fixed limit of the counter associated to this item (if any).
 * @return the fixed limit of the counter, or 0 if there is no counter
 * or no fixed limit
 */
int ItemProperties::get_fixed_limit(void) {
  return fixed_limit;
}

/**
 * @brief Returns the name of an item whose counter is limited by this item.
 *
 * The special names "money", "life" and "magic" may also be returned.
 *
 * @return the name of the item whose counter is limited by this item, or an empty string
 */
const std::string & ItemProperties::get_limited_counter_name(void) {
  return limited_counter_name;
}

/**
 * @brief Returns the name of an item whose counter is changed when obtaining this item.
 *
 * The special names "money", "life", "magic" and "small_keys" may also be returned.
 *
 * @return the name of the item whose counter is changed by this item, or an empty string
 */
const std::string & ItemProperties::get_changed_counter_name(void) {
  return changed_counter_name;
}

/**
 * @brief Returns whether an amount is associated to this item.
 *
 * This function returns true if get_limited_counter_name() or get_changed_counter_name()
 * return a non-empty string. The amount applies to the corresponding counter.
 *
 * @return true if this item has an amount
 */
bool ItemProperties::has_amount(void) {
  return get_limited_counter_name().size() > 0
    || get_changed_counter_name().size() > 0;
}

/**
 * @brief Returns the amount associated to the specified variant of the item.
 *
 * This amount applies to the counter indicated by get_limited_counter_name()
 * or get_changed_counter_name().
 *
 * @param variant a variant of this item
 * @return the amount of this variant
 */
bool ItemProperties::get_amount(int variant) {
  return amounts[variant - 1];
}

/**
 * @brief Returns the probability that this item with the specified variant appears
 * when an item is selected randomly.
 * @param variant a variant of this item
 * @return the probability that this variant of this item appears, between 0 and 1000
 */
bool ItemProperties::get_probability(int variant) {
  return probabilities[variant - 1];
}

/**
 * @brief Returns whether this item can be assigned to an item key
 * and then used explicitely by pressing this item key.
 * @return true if this item can be assigned
 */
bool ItemProperties::can_be_assigned(void) {
  return allow_assigned;
}

/**
 * @brief Returns whether this item disappears after a few seconds
 * when it is dropped by an enemy or a destructible entity.
 * @return true if this item can disappear
 */
bool ItemProperties::can_disappear(void) {
  return disappears;
}

/**
 * @brief Returns whether this item is brandished when the hero picks it
 * on the ground.
 * @return true if the hero brandishes the item after he picks it.
 */
bool ItemProperties::is_brandished_when_picked(void) {
  return brandish_when_picked;
}

/**
 * @brief Returns the sound to play when this item is picked on the ground.
 * @return the sound to play when this item is picked
 */
const SoundId & ItemProperties::get_sound_when_picked(void) {
  return sound_when_picked;
}

/**
 * @brief Returns the sound to play when this item is brandished.
 * @return the sound to play when this item is brandished
 */
const SoundId & ItemProperties::get_sound_when_brandished(void) {
  return sound_when_brandished;
}

/**
 * @brief Returns the size of the shadow to display when this item
 * is on the ground.
 * @return the size of the shadow
 */
ItemProperties::ShadowSize ItemProperties::get_shadow_size(void) {
  return shadow_size;
}

