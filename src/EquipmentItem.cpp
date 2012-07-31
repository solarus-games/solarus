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
#include "lowlevel/IniFile.h"
#include "lua/LuaContext.h"
#include <map>
#include <sstream>

/**
 * @brief Creates the properties of an item by parsing a group of the specified ini file
 *
 * The current group of the ini file describes the item to parse.
 *
 * @param equipment the equipment object that stores all item properties
 * @param ini the ini file to parse
 */
EquipmentItem::EquipmentItem(Equipment& equipment, IniFile& ini):
  equipment(equipment),
  pickable_item(NULL),
  inventory_item(NULL) {

  name = ini.get_group();
  savegame_variable = ini.get_integer_value("savegame_variable", -1);
  nb_variants = ini.get_integer_value("nb_variants", 1);
  initial_variant = ini.get_integer_value("initial_variant", 0);

  amounts = new int[nb_variants + 1];
  amounts[0] = 0;
  probabilities = new int[nb_variants + 1];
  probabilities[0] = 0;
  for (int i = 1; i <= nb_variants; i++) {

    std::ostringstream oss;
    oss << "amount_" << i;
    amounts[i] = ini.get_integer_value(oss.str(), 1);
    oss.str("");
    oss << "probability_" << i;
    probabilities[i] = ini.get_integer_value(oss.str(), 0);
  }
  amounts[1] = ini.get_integer_value("amount", amounts[1]);
  probabilities[1] = ini.get_integer_value("probability", probabilities[1]);

  allow_assigned = ini.get_boolean_value("can_be_assigned", false);
  counter_savegame_variable = ini.get_integer_value("counter", -1);
  fixed_limit = ini.get_integer_value("limit", 0);
  item_limiting = "";
  item_limited = ini.get_string_value("limit_for_counter", "");
  if (item_limited.size() != 0
      && item_limited != "life"
      && item_limited != "money"
      && item_limited != "magic") {
    equipment.get_item(item_limited).item_limiting = this->name;
  }
  item_counter_changed = ini.get_string_value("changes_counter", "");
  disappears = ini.get_boolean_value("can_disappear", false);
  brandish_when_picked = ini.get_boolean_value("brandish_when_picked", true);
  sound_when_picked = ini.get_string_value("sound_when_picked", "picked_item");
  sound_when_brandished = ini.get_string_value("sound_when_brandished", "treasure");
 
  const std::string &shadow_name = ini.get_string_value("shadow", "big");

  std::map<std::string, ShadowSize> shadows;
  shadows["none"] = SHADOW_NONE;
  shadows["small"] = SHADOW_SMALL;
  shadows["big"] = SHADOW_BIG;
  shadow_size = shadows[shadow_name];
}

/**
 * Destructor.
 */
EquipmentItem::~EquipmentItem() {

  delete[] amounts;
  delete[] probabilities;
}

/**
 * @brief Returns the equipment object this item belongs to.
 * @return The equipment.
 */
Equipment& EquipmentItem::get_equipment() {
  return equipment;
}

/**
 * @brief If this equipment item is currently running in a game, return that game.
 * @return A game or NULL.
 */
Game* EquipmentItem::get_game() {
  return equipment.get_game();
}

/**
 * @brief Returns the shared Lua context.
 * @return The Lua context where all scripts are run.
 */
LuaContext& EquipmentItem::get_lua_context() {
  return get_game()->get_lua_context();
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

  get_lua_context().item_on_set_suspended(*this, suspended);
}

/**
 * @brief This function is called when a game starts with this equipment item.
 * @param game The game.
 */
void EquipmentItem::notify_game_started(Game& game) {

  game.get_lua_context().notify_item_started(*this);
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
 * @param Variant The new possession state.
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

  this->inventory_item = &inventory_item;
  get_lua_context().item_on_use(*this, inventory_item);
  this->inventory_item = NULL;
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
 * @param pickable The pickable item.
 */
void EquipmentItem::notify_pickable_appeared(PickableItem& pickable) {

  this->pickable_item = &pickable;
  get_lua_context().item_on_appear(*this, pickable);
  this->pickable_item = NULL;
}

/**
 * @brief Notifies the script that a pickable instance of this item has moved.
 * @param pickable The pickable item.
 */
void EquipmentItem::notify_movement_changed(PickableItem& pickable) {

  this->pickable_item = &pickable;
  get_lua_context().item_on_movement_changed(*this, pickable);
  this->pickable_item = NULL;
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
 * @brief Returns the pickable item related to the current call to item:on_appear().
 * @return The current pickable item or NULL.
 */
PickableItem* EquipmentItem::get_pickable_item() {
  return pickable_item;
}

/**
 * @brief Returns the inventory item related to the current call item:on_use().
 * @return The current inventory item or NULL.
 */
InventoryItem* EquipmentItem::get_inventory_item() {
  return inventory_item;
}

/**
 * @brief Returns the name identifying the item.
 * @return the name of the item
 */
const std::string& EquipmentItem::get_name() {
  return name;
}

/**
 * @brief Returns whether the item possession state is saved.
 * @return true if the item is saved
 */
bool EquipmentItem::is_saved() {
  return get_savegame_variable() != -1;
}

/**
 * @brief Returns the savegame variable where the item possession state
 * is saved.
 * @return the savegame variable that stores the item possession state,
 * or -1 if the item is not saved
 */
int EquipmentItem::get_savegame_variable() {
  return savegame_variable;
}

/**
 * @brief Returns the number of variants of this item.
 * @return the number of variants
 */
int EquipmentItem::get_nb_variants() {
  return nb_variants;
}

/**
 * @brief Returns the initial possession state of this item.
 * @return the initial variant of this item
 */
int EquipmentItem::get_initial_variant() {
  return initial_variant;
}

/**
 * @brief Returns whether this item has a counter associated to it.
 * @return true if this item has a counter
 */
bool EquipmentItem::has_counter() {
  return get_counter_savegame_variable() != -1;
}

/**
 * @brief If this item has a counter, returns the savegame variable
 * that stores the value of this counter.
 * @return the savegame variable of the counter, or -1 if there is no counter
 * associated to this item.
 */
int EquipmentItem::get_counter_savegame_variable() {
  return counter_savegame_variable;
}

/**
 * @brief Returns whether the counter associated to this item has a fixed limit.
 * @return true if there is a counter with a fixed limit
 */
bool EquipmentItem::has_fixed_limit() {
  return get_fixed_limit() != 0;
}

/**
 * @brief Returns the fixed limit of the counter associated to this item (if any).
 * @return the fixed limit of the counter, or 0 if there is no counter
 * or no fixed limit
 */
int EquipmentItem::get_fixed_limit() {
  return fixed_limit;
}

/**
 * @brief Returns the name of an item that sets the limit to the counter of this item.
 * @return the name of the item that sets the limit of the counter of this item, or an empty string
 */
const std::string& EquipmentItem::get_item_limiting() {
  return item_limiting;
}

/**
 * @brief Returns the name of an item whose counter is limited by this item.
 *
 * The special names "money", "life" and "magic" may also be returned.
 *
 * @return the name of the item whose counter is limited by this item, or an empty string
 */
const std::string& EquipmentItem::get_item_limited() {
  return item_limited;
}

/**
 * @brief Returns the name of an item whose counter is changed when obtaining this item.
 *
 * The special names "money", "life", "magic" and "small_keys" may also be returned.
 *
 * @return the name of the item whose counter is changed by this item, or an empty string
 */
const std::string& EquipmentItem::get_item_counter_changed() {
  return item_counter_changed;
}

/**
 * @brief Returns the amount associated to the specified variant of the item.
 *
 * This amount applies to the counter indicated by get_item_limited()
 * or get_item_counter_changed().
 *
 * @param variant a variant of this item
 * @return the amount of this variant
 */
int EquipmentItem::get_other_amount(int variant) {
  return amounts[variant];
}

/**
 * @brief Returns the probability that this item with the specified variant appears
 * when an item is selected randomly.
 * @param variant a variant of this item
 * @return the probability that this variant of this item appears, between 0 and 1000
 */
int EquipmentItem::get_probability(int variant) {
  return probabilities[variant];
}

/**
 * @brief Returns whether this item can be assigned to an item key
 * and then used explicitely by pressing this item key.
 * @return true if this item can be assigned
 */
bool EquipmentItem::can_be_assigned() {
  return allow_assigned;
}

/**
 * @brief Returns whether this item disappears after a few seconds
 * when it is dropped by an enemy or a destructible entity.
 * @return true if this item can disappear
 */
bool EquipmentItem::can_disappear() {
  return disappears;
}

/**
 * @brief Returns whether this item is brandished when the hero picks it
 * on the ground.
 * @return true if the hero brandishes the item after he picks it.
 */
bool EquipmentItem::is_brandished_when_picked() {
  return brandish_when_picked;
}

/**
 * @brief Returns the sound to play when this item is picked on the ground.
 * @return the sound to play when this item is picked
 */
const std::string& EquipmentItem::get_sound_when_picked() {
  return sound_when_picked;
}

/**
 * @brief Returns the sound to play when this item is brandished.
 * @return the sound to play when this item is brandished
 */
const std::string& EquipmentItem::get_sound_when_brandished() {
  return sound_when_brandished;
}

/**
 * @brief Returns the size of the shadow to display when this item
 * is on the ground.
 * @return the size of the shadow
 */
EquipmentItem::ShadowSize EquipmentItem::get_shadow_size() {
  return shadow_size;
}

/**
 * @brief Returns the name identifying this type in Lua.
 * @return The name identifying this type in Lua.
 */
const std::string& EquipmentItem::get_lua_type_name() const {
  return Script::item_module_name;
}
