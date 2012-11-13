/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "EquipmentItem.h"
#include "Equipment.h"
#include "Game.h"
#include "lua/LuaContext.h"
#include "entities/Pickable.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <map>

const std::string EquipmentItem::shadow_size_names[] = {
    "none",
    "small",
    "big",
    ""  // Sentinel.
};

/**
 * @brief Creates the properties of an item by parsing a group of the specified ini file
 *
 * The current group of the ini file describes the item to parse.
 *
 * @param equipment the equipment object that stores all item properties
 * @param ini the ini file to parse
 */
EquipmentItem::EquipmentItem(Equipment& equipment):
  equipment(equipment),
  name(""),
  savegame_variable(""),
  initial_variant(1),
  counter_savegame_variable(""),
  fixed_limit(0),
  item_limiting(""),
  item_limited(""),
  item_counter_changed(""),
  amounts(1, 1),
  probabilities(1, 0),
  can_be_assigned(false),
  can_disappear(false),
  brandish_when_picked(true),
  sound_when_picked(""),
  sound_when_brandished(""),
  shadow_size(SHADOW_BIG) {

}

/**
 * Destructor.
 */
EquipmentItem::~EquipmentItem() {
}

/**
 * @brief Returns the equipment object this item belongs to.
 * @return The equipment.
 */
Equipment& EquipmentItem::get_equipment() const {
  return equipment;
}

/**
 * @brief If this equipment item is currently running in a game, return that game.
 * @return A game or NULL.
 */
Game* EquipmentItem::get_game() const {
  return equipment.get_game();
}

/**
 * @brief Returns the shared Lua context.
 * @return The Lua context where all scripts are run.
 */
LuaContext& EquipmentItem::get_lua_context() const {
  return get_game()->get_lua_context();
}

/**
 * @brief Returns the name identifying the item.
 * @return The name of this item.
 */
const std::string& EquipmentItem::get_name() const {
  return name;
}

/**
 * @brief Sets the name identifying the item.
 * @param name The name of this item.
 */
void EquipmentItem::set_name(const std::string& name) {
  this->name = name;
}

/**
 * @brief Returns whether the item possession state is saved.
 * @return true if the item is saved.
 */
bool EquipmentItem::is_saved() const {
  return !get_savegame_variable().empty();
}

/**
 * @brief Returns the savegame variable where the item possession state
 * is saved.
 * @return The savegame variable that stores the item possession state,
 * or an empty string if the item is not saved.
 */
const std::string& EquipmentItem::get_savegame_variable() const {
  return savegame_variable;
}

/**
 * @brief Sets the savegame variable where the item possession state is saved.
 * @param savegame_variable The savegame variable that stores the item
 * possession state, or an empty string.
 */
void EquipmentItem::set_savegame_variable(const std::string& savegame_variable) {
  this->savegame_variable = savegame_variable;
}

/**
 * @brief Returns the number of variants of this item.
 * @return The number of variants.
 */
int EquipmentItem::get_nb_variants() const {
  return amounts.size();
}

/**
 * @brief Sets the number of variants of this item.
 * @return The number of variants.
 */
void EquipmentItem::set_nb_variants(int nb_variants) {

  amounts.clear();
  probabilities.clear();
  for (int i = 0; i < nb_variants; i++) {
    amounts.push_back(1);
    probabilities.push_back(0);
  }
}

/**
 * @brief Returns the initial possession state of this item.
 * @return The initial variant of this item.
 */
int EquipmentItem::get_initial_variant() const {
  return initial_variant;
}

/**
 * @brief Sets the initial possession state of this item.
 * @param initial_variant The initial variant of this item.
 */
void EquipmentItem::set_initial_variant(int initial_variant) {
  this->initial_variant = initial_variant;
}

/**
 * @brief Returns whether this item has a counter associated to it.
 * @return true if this item has a counter.
 */
bool EquipmentItem::has_counter() const {
  return !get_counter_savegame_variable().empty();
}

/**
 * @brief If this item has a counter, returns the savegame variable
 * that stores the value of this counter.
 * @return The savegame variable of the counter, or an empty string if there
 * is no counter associated to this item.
 */
const std::string& EquipmentItem::get_counter_savegame_variable() const {
  return counter_savegame_variable;
}

/**
 * @brief If this item has a counter, sets the savegame variable
 * that stores the value of this counter.
 * @param counter_savegame_variable The savegame variable of the counter
 * or an empty string.
 */
void EquipmentItem::set_counter_savegame_variable(
    const std::string& counter_savegame_variable) {
  this->counter_savegame_variable = counter_savegame_variable;
}

/**
 * @brief Returns whether the counter associated to this item has a fixed limit.
 * @return true if there is a counter with a fixed limit.
 */
bool EquipmentItem::has_fixed_limit() const {
  return get_fixed_limit() != 0;
}

/**
 * @brief Returns the fixed limit of the counter associated to this item (if any).
 * @return The fixed limit of the counter, or 0 if there is no counter
 * or no fixed limit.
 */
int EquipmentItem::get_fixed_limit() const {
  return fixed_limit;
}

/**
 * @brief Sets the fixed limit of the counter associated to this item (if any).
 * @param fixed_limit The fixed limit of the counter, or 0 if there is no
 * counter or no fixed limit.
 */
void EquipmentItem::set_fixed_limit(int fixed_limit) {
  this->fixed_limit = fixed_limit;
}

/**
 * @brief Returns the name of an item that sets the limit to the counter of
 * this item.
 * @return The name of the item that sets the limit of the counter of this
 * item, or an empty string.
 */
const std::string& EquipmentItem::get_item_limiting() const {
  return item_limiting;
}

/**
 * @brief Sets the an item that acts as a limit to the counter of this item.
 * @param item_limiting Name of the item that acts as a limit to the counter
 * of this item, or an empty string.
 */
void EquipmentItem::set_item_limiting(const std::string& item_limiting) {
  this->item_limiting = item_limiting;
}

/**
 * @brief Returns the name of an item whose counter is limited by this item.
 *
 * The special names "money", "life" and "magic" may also be returned.
 *
 * @return the name of the item whose counter is limited by this item, or an empty string
 */
const std::string& EquipmentItem::get_item_limited() const {
  return item_limited;
}

/**
 * @brief Sets an item whose counter is limited by this item.
 *
 * The built-in names "money", "life" and "magic" are also accepted.
 *
 * @param item_limited The name of the item whose counter is limited by this
 * item, or an empty string.
 */
void EquipmentItem::set_item_limited(const std::string& item_limited) {

  this->item_limited = item_limited;
  if (!item_limited.empty()
      && item_limited != "life"
      && item_limited != "money"
      && item_limited != "magic") {
    equipment.get_item(item_limited).item_limiting = get_name();
  }
}

/**
 * @brief Returns the name of an item whose counter is changed when obtaining
 * this item.
 *
 * The special names "money", "life" and "magic" may also be returned.
 *
 * @return The name of the item whose counter is changed by this item, or an
 * empty string.
 */
const std::string& EquipmentItem::get_item_counter_changed() const {
  return item_counter_changed;
}

/**
 * @brief Sets an item whose counter will be changed when obtaining this item.
 *
 * The special names "money", "life" and "magic" are also accepted.
 *
 * @param item_counter_changed Name of an item whose counter is changed by
 * this item, or an empty string.
 */
void EquipmentItem::set_item_counter_changed(const std::string& item_counter_changed) {
  this->item_counter_changed = item_counter_changed;
}

/**
 * @brief Returns the amount associated to the specified variant of the item.
 *
 * This amount applies to the counter indicated by get_item_limited()
 * or get_item_counter_changed().
 *
 * @param variant A variant of this item.
 * @return The amount of this variant.
 */
int EquipmentItem::get_amount(int variant) const {

  Debug::check_assertion(variant > 0 && variant <= get_nb_variants(), StringConcat() <<
      "Invalid variant '" << variant << "' for item '" << get_name() << "'");

  return amounts[variant - 1];
}

/**
 * @brief Sets the amount associated to the specified variant of the item.
 *
 * This amount applies to the counter indicated by get_item_limited()
 * or get_item_counter_changed().
 *
 * @param variant A variant of this item.
 * @param amount The amount to set for this variant.
 */
void EquipmentItem::set_amount(int variant, int amount) {

  Debug::check_assertion(variant > 0 && variant <= get_nb_variants(), StringConcat() <<
      "Invalid variant '" << variant << "' for item '" << get_name() << "'");

  this->amounts[variant - 1] = amount;
}

/**
 * @brief Returns the probability that this item with the specified variant
 * appears when an item is selected randomly.
 * @param variant A variant of this item.
 * @return The probability that this variant of this item appears,
 * between 0 and 1000.
 */
int EquipmentItem::get_probability(int variant) const {

  Debug::check_assertion(variant > 0 && variant <= get_nb_variants(), StringConcat() <<
      "Invalid variant '" << variant << "' for item '" << get_name() << "'");

  return probabilities[variant - 1];
}

/**
 * @brief Sets the probability that this item with the specified variant
 * appears when an item is selected randomly.
 * @param variant A variant of this item.
 * @param probability The probability to set, between 0 and 1000.
 */
void EquipmentItem::set_probability(int variant, int probability) {

  Debug::check_assertion(variant > 0 && variant <= get_nb_variants(), StringConcat() <<
      "Invalid variant '" << variant << "' for item '" << get_name() << "'");

  this->probabilities[variant - 1] = probability;
}

/**
 * @brief Returns whether this item can be assigned to an item key
 * and then used explicitly by pressing this item key.
 * @return true if this item can be assigned.
 */
bool EquipmentItem::get_can_be_assigned() const {
  return can_be_assigned;
}

/**
 * @brief Sets whether this item can be assigned to an item key
 * and then used explicitly by pressing this item key.
 * @param can_be_assigned true if this item can be assigned.
 */
void EquipmentItem::set_can_be_assigned(bool can_be_assigned) {
  this->can_be_assigned = can_be_assigned;
}

/**
 * @brief Returns whether this item disappears after a few seconds
 * when it is dropped by an enemy or a destructible entity.
 * @return true if this item can disappear.
 */
bool EquipmentItem::get_can_disappear() const {
  return can_disappear;
}

/**
 * @brief Returns whether this item disappears after a few seconds
 * when it is dropped by an enemy or a destructible entity.
 * @param can_disappear true if this item can disappear.
 */
void EquipmentItem::set_can_disappear(bool can_disappear) {
  this->can_disappear = can_disappear;
}

/**
 * @brief Returns whether this item is brandished when the hero picks it
 * on the ground.
 * @return true if the hero brandishes the item after he picks it.
 */
bool EquipmentItem::get_brandish_when_picked() const {
  return brandish_when_picked;
}

/**
 * @brief Sets whether this item is brandished when the hero picks it
 * on the ground.
 * @param brandish_when_picked true to make the hero brandish the item after
 * he picks it.
 */
void EquipmentItem::set_brandish_when_picked(bool brandish_when_picked) {
  this->brandish_when_picked = brandish_when_picked;
}

/**
 * @brief Returns the sound to play when this item is picked on the ground.
 * @return The sound to play when this item is picked
 * or an empty string.
 */
const std::string& EquipmentItem::get_sound_when_picked() const {
  return sound_when_picked;
}

/**
 * @brief Sets the sound to play when this item is picked on the ground.
 * @param sound_when_picked The sound to play when this item is picked
 * or an empty string.
 */
void EquipmentItem::set_sound_when_picked(
    const std::string& sound_when_picked) {
  this->sound_when_picked = sound_when_picked;
}

/**
 * @brief Returns the sound to play when this item is brandished.
 * @return The sound to play when this item is brandished or an empty string.
 */
const std::string& EquipmentItem::get_sound_when_brandished() const {
  return sound_when_brandished;
}

/**
 * @brief Sets the sound to play when this item is brandished.
 * @param sound_when_brandished The sound to play when this item is brandished or an empty string.
 */
void EquipmentItem::set_sound_when_brandished(
    const std::string& sound_when_brandished) {
  this->sound_when_brandished = sound_when_brandished;
}

/**
 * @brief Returns the size of the shadow to display when this item is on the
 * ground.
 * @return The size of the shadow.
 */
EquipmentItem::ShadowSize EquipmentItem::get_shadow_size() const {
  return shadow_size;
}

/**
 * @brief Sets the size of the shadow to display when this item is on the
 * ground.
 * @param shadow_size The size of the shadow.
 */
void EquipmentItem::set_shadow_size(ShadowSize shadow_size) {
  this->shadow_size = shadow_size;
}

/**
 * @brief Converts a ShadowSize enum value into its name.
 * @param shadow_size A shadow size value.
 * @return The corresponding name.
 */
const std::string& EquipmentItem::get_shadow_size_name(ShadowSize shadow_size) {

  return shadow_size_names[shadow_size];
}

/**
 * @brief Gets a ShadowSize enum value from its name.
 * @param shadow_size_name Name of a shadow size value.
 * @return The corresponding ShadowSize value.
 */
EquipmentItem::ShadowSize EquipmentItem::get_shadow_size_by_name(const std::string& shadow_size_name) {

  int i = 0;
  while (!shadow_size_names[i].empty()) {
    if (shadow_size_names[i] == shadow_size_name) {
      return ShadowSize(i);
    }
    ++i;
  }

  Debug::die(StringConcat() << "Invalid shadow size name: '"
      << shadow_size_name << "'");
  throw;
}

/**
 * @brief This function is be called repeatedly by the game.
 */
void EquipmentItem::update() {

  if (get_game() != NULL) {  // Nothing dynamic when there is no game.
    get_lua_context().item_on_update(*this);
  }
}

/**
 * @brief This function is called when the game is suspended or resumed.
 * @param suspended true if the game is suspended, false if it is resumed
 */
void EquipmentItem::set_suspended(bool suspended) {

  get_lua_context().item_on_suspended(*this, suspended);
}

/**
 * @brief This function is called when a game starts with this equipment item.
 * @param game The game.
 */
void EquipmentItem::notify_game_started(Game& game) {

  game.get_lua_context().notify_item_created(*this);
}

/**
 * @brief This function is called when a map becomes active.
 * @param map The map.
 */
void EquipmentItem::notify_map_started(Map& map) {

  get_lua_context().item_on_map_changed(*this, map);
}

/**
 * @brief Notifies this item that its possession state has just changed.
 * @param variant The new possession state.
 */
void EquipmentItem::notify_variant_changed(int variant) {

  get_lua_context().item_on_variant_changed(*this, variant);
}

/**
 * @brief Notifies the script that the amount of this item has just changed.
 * @param amount The new amount.
 */
void EquipmentItem::notify_amount_changed(int amount) {

  get_lua_context().item_on_amount_changed(*this, amount);
}

/**
 * @brief Notifies the script that the hero is using this item from his inventory.
 * @param inventory_item The inventory item.
 */
void EquipmentItem::notify_inventory_item_used(InventoryItem& inventory_item) {

  get_lua_context().item_on_using(*this);
}

/**
 * @brief Notifies this item that a built-in ability was used.
 * @param ability_name Name of an ability.
 */
void EquipmentItem::notify_ability_used(const std::string& ability_name) {

  get_lua_context().item_on_ability_used(*this, ability_name);
}

/**
 * @brief Notifies the script that a pickable instance of this item has
 * appeared on the map.
 * @param pickable The pickable treasure.
 */
void EquipmentItem::notify_pickable_appeared(Pickable& pickable) {

  get_lua_context().item_on_pickable_created(*this, pickable);
}

/**
 * @brief Notifies the script that a pickable instance of this item has moved.
 * @param pickable The pickable treasure.
 */
void EquipmentItem::notify_movement_changed(Pickable& pickable) {

  get_lua_context().item_on_pickable_movement_changed(*this, pickable, *pickable.get_movement());
}

/**
 * @brief Returns the possession state of this item.
 *
 * This function only makes sense if the item is saved.
 *
 * @return The possessed variant of this item.
 */
int EquipmentItem::get_current_variant() {
  return equipment.get_item_variant(get_name());
}

/**
 * @brief Sets the possession state of this item.
 *
 * This function only makes sense if the item is saved.
 *
 * @param variant The possessed variant of this item.
 */
void EquipmentItem::set_current_variant(int variant) {
  equipment.set_item_variant(get_name(), variant);
}

/**
 * @brief Returns the current value of the counter associated to this item.
 * @return The player's current amount of this item.
 */
int EquipmentItem::get_current_amount() {
  return equipment.get_item_amount(get_name());
}

/**
 * @brief Sets the current value of the counter associated to this item.
 * @param amount The player's new amount of this item.
 */
void EquipmentItem::set_current_amount(int amount) {
  equipment.set_item_amount(get_name(), amount);
}

/**
 * @brief Returns the name identifying this type in Lua.
 * @return The name identifying this type in Lua.
 */
const std::string& EquipmentItem::get_lua_type_name() const {
  return LuaContext::item_module_name;
}
