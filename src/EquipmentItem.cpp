/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/EquipmentItem.h"
#include "solarus/entities/Pickable.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/Equipment.h"
#include "solarus/Game.h"
#include "solarus/Savegame.h"
#include <algorithm>

namespace Solarus {

/**
 * \brief Creates the description of an item.
 * \param equipment The equipment object that stores all item descriptions.
 */
EquipmentItem::EquipmentItem(Equipment& equipment):
  equipment(equipment),
  name(""),
  savegame_variable(""),
  max_amount(1000),
  obtainable(true),
  assignable(false),
  can_disappear(false),
  brandish_when_picked(true),
  sound_when_picked("picked_item"),
  sound_when_brandished("treasure"),
  shadow("big") {

}

/**
 * \brief Returns the equipment object this item belongs to.
 * \return The equipment.
 */
Equipment& EquipmentItem::get_equipment() {
  return equipment;
}

/**
 * \brief If this equipment item is currently running in a game, return that game.
 * \return A game or nullptr.
 */
Game* EquipmentItem::get_game() {
  return equipment.get_game();
}

/**
 * \brief Returns the savegame.
 * \return The savegame.
 */
Savegame& EquipmentItem::get_savegame() {
  return equipment.get_savegame();
}

/**
 * \brief Returns the savegame.
 * \return The savegame.
 */
const Savegame& EquipmentItem::get_savegame() const {
  return equipment.get_savegame();
}

/**
 * \brief Returns the shared Lua context.
 * \return The Lua context where all scripts are run.
 */
LuaContext& EquipmentItem::get_lua_context() {
  return get_savegame().get_lua_context();
}

/**
 * \brief Returns the name identifying the item.
 * \return The name of this item.
 */
const std::string& EquipmentItem::get_name() const {
  return name;
}

/**
 * \brief Sets the name identifying the item.
 * \param name The name of this item.
 */
void EquipmentItem::set_name(const std::string& name) {
  this->name = name;
}

/**
 * \brief Returns whether the item possession state is saved.
 * \return true if the item is saved.
 */
bool EquipmentItem::is_saved() const {
  return !get_savegame_variable().empty();
}

/**
 * \brief Returns the savegame variable where the item possession state
 * is saved.
 * \return The savegame variable that stores the item possession state,
 * or an empty string if the item is not saved.
 */
const std::string& EquipmentItem::get_savegame_variable() const {
  return savegame_variable;
}

/**
 * \brief Sets the savegame variable where the item possession state is saved.
 * \param savegame_variable The savegame variable that stores the item
 * possession state, or an empty string.
 */
void EquipmentItem::set_savegame_variable(const std::string& savegame_variable) {
  this->savegame_variable = savegame_variable;
}

/**
 * \brief Returns whether this item has an amount associated to it.
 * \return true if this item has an amount.
 */
bool EquipmentItem::has_amount() const {
  return !get_amount_savegame_variable().empty();
}

/**
 * \brief If this item has an amount, returns the savegame variable
 * that stores the value of this amount.
 * \return The savegame variable of the amount, or an empty string if there
 * is no amount associated to this item.
 */
const std::string& EquipmentItem::get_amount_savegame_variable() const {
  return amount_savegame_variable;
}

/**
 * \brief Sets the savegame variable that stores the value of the amount
 * of this item (if any).
 * \param amount_savegame_variable The savegame variable of the amount
 * or an empty string.
 */
void EquipmentItem::set_amount_savegame_variable(
    const std::string& amount_savegame_variable) {
  this->amount_savegame_variable = amount_savegame_variable;
}

/**
 * \brief Returns whether this item can be obtained by the player.
 * \return true if this item can be obtained.
 */
bool EquipmentItem::is_obtainable() const {
  return obtainable;
}

/**
 * \brief Sets whether this item can be obtained by the player.
 * \param obtainable true to allow the player to obtain this item.
 */
void EquipmentItem::set_obtainable(bool obtainable) {
  this->obtainable = obtainable;
}

/**
 * \brief Returns whether this item can be assigned to an item key
 * and then used explicitly by pressing this item key.
 * \return true if this item can be assigned.
 */
bool EquipmentItem::is_assignable() const {
  return assignable;
}

/**
 * \brief Sets whether this item can be assigned to an item key
 * and then used explicitly by pressing this item key.
 * \param assignable true if this item can be assigned.
 */
void EquipmentItem::set_assignable(bool assignable) {
  this->assignable = assignable;
}

/**
 * \brief Returns whether this item disappears after a few seconds
 * when it is dropped by an enemy or a destructible entity.
 * \return true if this item can disappear.
 */
bool EquipmentItem::get_can_disappear() const {
  return can_disappear;
}

/**
 * \brief Returns whether this item disappears after a few seconds
 * when it is dropped by an enemy or a destructible entity.
 * \param can_disappear true if this item can disappear.
 */
void EquipmentItem::set_can_disappear(bool can_disappear) {
  this->can_disappear = can_disappear;
}

/**
 * \brief Returns whether this item is brandished when the hero picks it
 * on the ground.
 * \return true if the hero brandishes the item after he picks it.
 */
bool EquipmentItem::get_brandish_when_picked() const {
  return brandish_when_picked;
}

/**
 * \brief Sets whether this item is brandished when the hero picks it
 * on the ground.
 * \param brandish_when_picked true to make the hero brandish the item after
 * he picks it.
 */
void EquipmentItem::set_brandish_when_picked(bool brandish_when_picked) {
  this->brandish_when_picked = brandish_when_picked;
}

/**
 * \brief Returns the sound to play when this item is picked on the ground.
 * \return The sound to play when this item is picked
 * or an empty string.
 */
const std::string& EquipmentItem::get_sound_when_picked() const {
  return sound_when_picked;
}

/**
 * \brief Sets the sound to play when this item is picked on the ground.
 * \param sound_when_picked The sound to play when this item is picked
 * or an empty string.
 */
void EquipmentItem::set_sound_when_picked(
    const std::string& sound_when_picked) {
  this->sound_when_picked = sound_when_picked;
}

/**
 * \brief Returns the sound to play when this item is brandished.
 * \return The sound to play when this item is brandished or an empty string.
 */
const std::string& EquipmentItem::get_sound_when_brandished() const {
  return sound_when_brandished;
}

/**
 * \brief Sets the sound to play when this item is brandished.
 * \param sound_when_brandished The sound to play when this item is brandished or an empty string.
 */
void EquipmentItem::set_sound_when_brandished(
    const std::string& sound_when_brandished) {
  this->sound_when_brandished = sound_when_brandished;
}

/**
 * \brief Returns the animation of the shadow sprite to display when this item
 * is on the ground.
 * \return The animation of the shadow sprite, or an empty string.
 */
const std::string& EquipmentItem::get_shadow() const {
  return shadow;
}

/**
 * \brief Sets the animation of the shadow sprite to display when this item is
 * on the ground.
 * \param shadow The animation of the shadow sprite or an empty string.
 */
void EquipmentItem::set_shadow(const std::string& shadow) {
  this->shadow = shadow;
}

/**
 * \brief This function is be called repeatedly by the game.
 */
void EquipmentItem::update() {

  get_lua_context().item_on_update(*this);
}

/**
 * \brief This function is called when the game is suspended or resumed.
 * \param suspended true if the game is suspended, false if it is resumed
 */
void EquipmentItem::set_suspended(bool suspended) {

  get_lua_context().item_on_suspended(*this, suspended);
}

/**
 * \brief This function is called when the equipment object starts.
 */
void EquipmentItem::initialize() {

  // Read the Lua script of this item if any.
  get_lua_context().run_item(*this);
}

/**
 * \brief Starts this item.
 *
 * When this function is called, all equipment items are initialized.
 */
void EquipmentItem::start() {

  get_lua_context().item_on_started(*this);
}

/**
 * \brief This function is called before the item is destroyed.
 */
void EquipmentItem::exit() {

  get_lua_context().item_on_finished(*this);
}

/**
 * \brief This function is called when a map becomes active.
 * \param map The map.
 */
void EquipmentItem::notify_map_changed(Map& map) {

  get_lua_context().item_on_map_changed(*this, map);
}

/**
 * \brief Notifies this item that its possession state has just changed.
 * \param variant The new possession state.
 */
void EquipmentItem::notify_variant_changed(int variant) {

  get_lua_context().item_on_variant_changed(*this, variant);
}

/**
 * \brief Notifies the script that the amount of this item has just changed.
 * \param amount The new amount.
 */
void EquipmentItem::notify_amount_changed(int amount) {

  get_lua_context().item_on_amount_changed(*this, amount);
}

/**
 * \brief Notifies the script that the hero is using this equipment item.
 */
void EquipmentItem::notify_using() {

  get_lua_context().item_on_using(*this);
}

/**
 * \brief Notifies this item that a built-in ability was used.
 * \param ability An ability.
 */
void EquipmentItem::notify_ability_used(Ability ability) {

  get_lua_context().item_on_ability_used(*this, ability);
}

/**
 * \brief Notifies the script that a pickable instance of this item has
 * appeared on the map.
 * \param pickable The pickable treasure.
 */
void EquipmentItem::notify_pickable_appeared(Pickable& pickable) {

  get_lua_context().item_on_pickable_created(*this, pickable);
}

/**
 * \brief Returns the possession state of this item.
 *
 * The item must be saved.
 *
 * \return The possessed variant of this item.
 */
int EquipmentItem::get_variant() const {

  Debug::check_assertion(is_saved(),
      std::string("The item '") + get_name() + "' is not saved");

  return get_savegame().get_integer(get_savegame_variable());
}

/**
 * \brief Sets the possession state of this item.
 *
 * This function only makes sense if the item is saved.
 *
 * \param variant The possessed variant of this item.
 */
void EquipmentItem::set_variant(int variant) {

  Debug::check_assertion(is_saved(),
      std::string("The item '") + get_name() + "' is not saved");

  // Set the possession state in the savegame.
  get_savegame().set_integer(get_savegame_variable(), variant);

  // If we are removing the item, unassign it.
  if (variant == 0) {
    int slot = equipment.get_item_slot(*this);
    if (slot != 0) {
      equipment.set_item_assigned(slot, nullptr);
    }
  }

  // Notify the script.
  notify_variant_changed(variant);
}

/**
 * \brief Returns the current value of the amount associated to this item.
 * \return The player's current amount of this item.
 */
int EquipmentItem::get_amount() const {

  Debug::check_assertion(has_amount(),
      std::string("The item '") + get_name() + "' has no amount");

  return get_savegame().get_integer(get_amount_savegame_variable());
}

/**
 * \brief Sets the current value of the amount associated to this item.
 * \param amount The player's new amount of this item.
 */
void EquipmentItem::set_amount(int amount) {

  Debug::check_assertion(has_amount(),
      std::string("The item '") + get_name() + "' has no amount");

  amount = std::max(0, std::min(get_max_amount(), amount));
  get_savegame().set_integer(get_amount_savegame_variable(), amount);

  notify_amount_changed(amount);
}

/**
 * \brief Returns the maximum value of the amount associated to this item.
 * \return The maximum amount of this item.
 */
int EquipmentItem::get_max_amount() const {

  Debug::check_assertion(has_amount(),
      std::string("The item '") + get_name() + "' has no amount");

  return max_amount;
}

/**
 * \brief Sets the maximum value of the amount associated to this item.
 * \param max_amount The maximum amount of this item.
 */
void EquipmentItem::set_max_amount(int max_amount) {

  Debug::check_assertion(has_amount(),
      std::string("The item '") + get_name() + "' has no amount");

  this->max_amount = max_amount;

  // If the max amount is reduced, make sure the current amount does not exceed
  // the new maximum.
  if (get_amount() > max_amount) {
    set_amount(max_amount);
  }
}

/**
 * \brief Returns the name identifying this type in Lua.
 * \return The name identifying this type in Lua.
 */
const std::string& EquipmentItem::get_lua_type_name() const {
  return LuaContext::item_module_name;
}

}

