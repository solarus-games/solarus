/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "entities/Pickable.h"
#include "entities/Hero.h"
#include "entities/Boomerang.h"
#include "entities/Hookshot.h"
#include "movements/FallingOnFloorMovement.h"
#include "movements/FollowMovement.h"
#include "lua/LuaContext.h"
#include "lowlevel/System.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Sound.h"
#include "Game.h"
#include "Map.h"
#include "Sprite.h"
#include "EquipmentItem.h"

/**
 * @brief Creates a pickable item with the specified subtype.
 * @param name Unique name identifying the entity on the map or an empty string.
 * @param layer layer of the pickable item to create on the map
 * @param x x coordinate of the pickable item to create
 * @param y y coordinate of the pickable item to create
 * @param treasure the treasure to give when the item is picked
 */
Pickable::Pickable(const std::string& name, Layer layer,
    int x, int y, const Treasure& treasure):
  Detector(COLLISION_RECTANGLE | COLLISION_SPRITE, name, layer, x, y, 0, 0),
  treasure(treasure),
  given_to_player(false),
  shadow_sprite(NULL),
  shadow_xy(Rectangle(x, y)),
  appear_date(System::now()),
  entity_followed(NULL) {

}

/**
 * @brief Destructor.
 */
Pickable::~Pickable() {

  delete shadow_sprite;
}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType Pickable::get_type() {
  return PICKABLE;
}

/**
 * @brief Creates a pickable item with the specified subtype.
 *
 * This method acts like a constructor, except that it can return NULL in several cases:
 * - the treasure is saved and the player already has it,
 * or:
 * - the treasure is empty,
 * or:
 * - the item cannot be obtained by the hero yet.
 *
 * @param game the current game
 * @param name Unique name identifying the entity on the map or an empty string.
 * @param layer layer of the pickable item to create on the map
 * @param x x coordinate of the pickable item to create
 * @param y y coordinate of the pickable item to create
 * @param treasure the treasure to give
 * @param falling_height to make the item fall when it appears
 * @param force_persistent true to make the item stay forever (otherwise, the properties of the item
 * decide if it disappears after some time)
 * @return the pickable item created, or NULL
 */
Pickable* Pickable::create(Game& game, const std::string& name,
    Layer layer, int x, int y, Treasure treasure,
    FallingHeight falling_height, bool force_persistent) {

  treasure.ensure_obtainable();

  // Don't create anything if there is no treasure to give.
  if (treasure.is_empty()) {
    return NULL;
  }

  Pickable *pickable = new Pickable(name, layer, x, y, treasure);

  // Set the item properties.
  pickable->falling_height = falling_height;
  pickable->will_disappear = !force_persistent && treasure.get_item().get_can_disappear();

  // Initialize the pickable item.
  pickable->initialize_sprites();
  pickable->initialize_movement();

  return pickable;
}

/**
 * @brief Returns whether entities of this type can be obstacles for other entities.
 * @return \c true if this type of entity can be obstacle for other entities.
 */
bool Pickable::can_be_obstacle() {
  return false;
}

/**
 * @brief Creates the sprite of this pickable item,
 * depending on its subtype.
 *
 * Pickable items represented with two sprites:
 * the item itself and, for some items, its shadow.
 */
void Pickable::initialize_sprites() {

  // Shadow sprite.
  delete shadow_sprite;
  EquipmentItem& item = treasure.get_item();
  const std::string& animation = item.get_shadow();

  bool has_shadow = false;
  if (!animation.empty()) {
    shadow_sprite = new Sprite("entities/shadow");
    has_shadow = shadow_sprite->has_animation(animation);
  }

  if (!has_shadow) {
    // No shadow or no such shadow animation.
    delete shadow_sprite;
    shadow_sprite = NULL;
  }
  else {
    shadow_sprite->set_current_animation(animation);
  }

  // Main sprite.
  create_sprite("entities/items");
  Sprite& item_sprite = get_sprite();
  item_sprite.set_current_animation(treasure.get_item_name());
  item_sprite.set_current_direction(treasure.get_variant() - 1);
  item_sprite.enable_pixel_collisions();

  // Set the origin point and the size of the entity.
  set_bounding_box_from_sprite();

  uint32_t now = System::now();

  if (falling_height != FALLING_NONE) {
    allow_pick_date = now + 700;  // The player will be allowed to take the item after 0.7 seconds.
    can_be_picked = false;
  }
  else {
    can_be_picked = true;
  }

  // Initialize the item removal.
  if (will_disappear) {
    blink_date = now + 8000;       // The item blinks after 8s.
    disappear_date = now + 10000;  // The item disappears after 10s.
  }
}

/**
 * @brief Notifies this entity that it has just been added to a map.
 * @param map the map
 */
void Pickable::set_map(Map& map) {

  MapEntity::set_map(map);

  if (map.is_started()) {
    // notify the Lua item
    get_equipment().get_item(treasure.get_item_name()).notify_pickable_appeared(*this);
  }
  // otherwise, notify_map_started() will do the job
}

/**
 * @brief Notifies this entity that its map has just become active.
 */
void Pickable::notify_map_started() {

  MapEntity::notify_map_started();

  // notify the Lua item.
  get_equipment().get_item(treasure.get_item_name()).notify_pickable_appeared(*this);
}

/**
 * @brief Initializes the movement of the item (if it is falling),
 * depending on its subtype.
 */
void Pickable::initialize_movement() {

  if (is_falling()) {
    set_movement(new FallingOnFloorMovement(falling_height));
  }
}

/**
 * @brief Returns whether the entity is currently falling.
 * @return true if the entity is currently falling
 */
bool Pickable::is_falling() {
  return get_falling_height() != FALLING_NONE;
}

/**
 * @brief Returns the height this pickable item falls from when it appears.
 * @return the falling height
 */
FallingHeight Pickable::get_falling_height() {
  return falling_height;
}

/**
 * @brief Returns the treasure the player receives if he picks this item.
 * @return the treasure
 */
const Treasure& Pickable::get_treasure() {
  return treasure;
}

/**
 * @brief Returns the entity (if any) followed by this pickable item.
 * @return the entity followed or NULL
 */
MapEntity* Pickable::get_entity_followed() {
  return entity_followed;
}

/**
 * @brief Notifies this pickable item that its movement has just changed.
 */
void Pickable::notify_movement_changed() {

  if (is_on_map()) {
    // Notify the Lua equipment item.
    get_equipment().get_item(treasure.get_item_name()).notify_movement_changed(*this);
  }
}

/**
 * @brief This function is called by the engine when an entity overlaps the pickable item.
 *
 * If the entity is the player, we give him the item, and the map is notified
 * to destroy it.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Pickable::notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping.is_hero()
      && can_be_picked
      && !get_game().is_dialog_enabled()) {
    remove_from_map();
    give_item_to_player();
  }
  else if (entity_followed == NULL) {

    if (entity_overlapping.get_type() == BOOMERANG) {
      Boomerang& boomerang = static_cast<Boomerang&>(entity_overlapping);
      if (!boomerang.is_going_back()) {
        boomerang.go_back();
      }
      entity_followed = &boomerang;
    }
    else if (entity_overlapping.get_type() == HOOKSHOT) {
      Hookshot& hookshot = static_cast<Hookshot&>(entity_overlapping);
      if (!hookshot.is_going_back()) {
        hookshot.go_back();
      }
      entity_followed = &hookshot;
    }

    if (entity_followed != NULL) {
      clear_movement();
      set_movement(new FollowMovement(entity_followed, 0, 0, true));
      falling_height = FALLING_NONE;
      set_blinking(false);
    }
  }
}

/**
 * @brief Notifies this entity that another sprite is overlapping it.
 *
 * This function is called by check_collision(MapEntity*, Sprite*) when another entity's
 * sprite overlaps a sprite of this detector.
 *
 * @param other_entity the entity overlapping this detector
 * @param other_sprite the sprite of other_entity that is overlapping this detector
 * @param this_sprite the sprite of this detector that is overlapping the other entity's sprite
 */
void Pickable::notify_collision(MapEntity& other_entity, Sprite& other_sprite,
    Sprite& this_sprite) {

  // taking the item with the sword
  if (other_entity.is_hero()
      && other_sprite.contains("sword")
      && can_be_picked) {

    remove_from_map();
    give_item_to_player();
  }
}

/**
 * @brief Gives the item to the player.
 */
void Pickable::give_item_to_player() {

  if (given_to_player) {
    // Just to be sure.
    return;
  }
  given_to_player = true;

  EquipmentItem& item = treasure.get_item();

  // play the sound
  const std::string& sound_id = item.get_sound_when_picked();
  if (!sound_id.empty()) {
    Sound::play(sound_id);
  }

  // give the item
  if (item.get_brandish_when_picked()) {
    get_hero().start_treasure(treasure, LUA_REFNIL);
  }
  else {
    treasure.give_to_player();
  }
}

/**
 * @brief Sets whether the pickable item is blinking.
 * @param blinking true to make it blink, false to make it stop blinking
 */
void Pickable::set_blinking(bool blinking) {

  uint32_t blink_delay = blinking ? 75 : 0;

  get_sprite().set_blinking(blink_delay);

  if (shadow_sprite != NULL) {
    shadow_sprite->set_blinking(blink_delay);
  }
}

/**
 * @brief This function is called by the map when the game is suspended or resumed.
 *
 * This is a redefinition of MapEntity::set_suspended() to suspend the timer
 * which makes the pickable item disappear after a few seconds.
 *
 * @param suspended true to suspend the entity, false to resume it
 */
void Pickable::set_suspended(bool suspended) {

  MapEntity::set_suspended(suspended); // suspend the animation and the movement

  if (shadow_sprite != NULL) {
    shadow_sprite->set_suspended(suspended);
  }

  if (!suspended) {
    // suspend the timers

    uint32_t now = System::now();

    if (!can_be_picked && when_suspended != 0) { // TODO make a better implementation of the when_suspended stuff
      allow_pick_date = now + (allow_pick_date - when_suspended);
    }

    if (will_disappear) {

      // the game is being resumed
      // recalculate the blinking date and the disappearing date
      if (when_suspended != 0) {
        blink_date = now + (blink_date - when_suspended);
        disappear_date = now + (disappear_date - when_suspended);
      }
    }
  }
}

/**
 * @brief Updates the pickable item.
 *
 * This function is called repeatedly by the map.
 * This is a redefinition of MapEntity::update() to make
 * the item blink and then disappear after an amount of time.
 */
void Pickable::update() {

  // update the animations and the movement
  MapEntity::update();

  // update the shadow
  if (shadow_sprite != NULL) {
    shadow_sprite->update();
  }

  shadow_xy.set_x(get_x());
  if (!is_falling()) {
    shadow_xy.set_y(get_y());
  }

  if (entity_followed != NULL && entity_followed->is_being_removed()) {

    if (entity_followed->get_type() == BOOMERANG ||
        entity_followed->get_type() == HOOKSHOT) {
      // The pickable may have been dropped by the boomerang/hookshot
      // not exactly on the hero so let's fix this.
      if (get_distance(get_hero()) < 16) {
        remove_from_map();
        give_item_to_player();
      }
    }
    entity_followed = NULL;
  }

  if (!is_suspended()) {

    // check the timer
    uint32_t now = System::now();

    // wait 0.7 second before allowing the hero to take the item
    if (!can_be_picked && now >= allow_pick_date) {
      can_be_picked = true;
      get_hero().check_collision_with_detectors(false);
    }
    else {
      // make the item blink and then disappear
      if (will_disappear) {

        if (now >= blink_date && !get_sprite().is_blinking() && entity_followed == NULL) {
          set_blinking(true);
        }

        if (now >= disappear_date) {
          remove_from_map();
        }
      }
    }
  }
}

/**
 * @brief Draws the pickable item on the map.
 *
 * This is a redefinition of MapEntity::draw_on_map
 * to draw the shadow independently of the item movement.
 */
void Pickable::draw_on_map() {

  if (!is_drawn()) {
    return;
  }

  // draw the shadow
  if (shadow_sprite != NULL) {
    get_map().draw_sprite(*shadow_sprite, shadow_xy.get_x(), shadow_xy.get_y());
  }

  // draw the sprite
  MapEntity::draw_on_map();
}

/**
 * @brief Returns the name identifying this type in Lua.
 * @return The name identifying this type in Lua.
 */
const std::string& Pickable::get_lua_type_name() const {
  return LuaContext::entity_pickable_module_name;
}

