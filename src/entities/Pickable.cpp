/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/entities/Pickable.h"
#include "solarus/entities/Boomerang.h"
#include "solarus/entities/Hero.h"
#include "solarus/entities/Hookshot.h"
#include "solarus/hero/HeroSprites.h"
#include "solarus/movements/FallingOnFloorMovement.h"
#include "solarus/movements/FollowMovement.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Sound.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/movements/FallingOnFloorMovement.h"
#include "solarus/movements/FollowMovement.h"
#include "solarus/Equipment.h"
#include "solarus/EquipmentItem.h"
#include "solarus/Game.h"
#include "solarus/Map.h"
#include "solarus/Sprite.h"
#include <lua.hpp>
#include <sstream>

namespace Solarus {

/**
 * \brief Creates a pickable item with the specified subtype.
 * \param name Name identifying the entity on the map or an empty string.
 * \param layer layer of the pickable item to create on the map
 * \param xy Coordinates of the pickable item to create.
 * \param treasure the treasure to give when the item is picked
 */
Pickable::Pickable(
    const std::string& name,
    Layer layer,
    const Point& xy,
    const Treasure& treasure
):
  Detector(COLLISION_OVERLAPPING | COLLISION_SPRITE, name, layer, xy, Size(0, 0)),
  treasure(treasure),
  given_to_player(false),
  shadow_sprite(),
  falling_height(FALLING_NONE),
  will_disappear(false),
  shadow_xy(xy),
  appear_date(System::now()),
  allow_pick_date(0),
  can_be_picked(true),
  blink_date(0),
  disappear_date(0),
  entity_followed(nullptr) {

}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Pickable::get_type() const {
  return EntityType::PICKABLE;
}

/**
 * \brief Creates a pickable item with the specified subtype.
 *
 * This method acts like a constructor, except that it can return nullptr in several cases:
 * - the treasure is saved and the player already has it,
 * or:
 * - the treasure is empty,
 * or:
 * - the item cannot be obtained by the hero yet,
 * or:
 * - the animation of the item is missing in sprite 'entities/items'.
 *
 * \param game the current game
 * \param name Name identifying the entity on the map or an empty string.
 * \param layer layer of the pickable treasure to create on the map
 * \param xy Coordinates of the pickable treasure to create
 * \param treasure the treasure to give
 * \param falling_height to make the treasure fall when it appears
 * \param force_persistent true to make the treasure stay forever
 * (otherwise, the properties of the treasure
 * decide if it disappears after some time)
 * \return the pickable item created, or nullptr
 */
std::shared_ptr<Pickable> Pickable::create(
    Game& /* game */,
    const std::string& name,
    Layer layer,
    const Point& xy,
    Treasure treasure,
    FallingHeight falling_height,
    bool force_persistent
) {
  treasure.ensure_obtainable();

  // Don't create anything if there is no treasure to give.
  if (treasure.is_found() || treasure.is_empty()) {
    return nullptr;
  }

  std::shared_ptr<Pickable> pickable = std::make_shared<Pickable>(
      name, layer, xy, treasure
  );

  // Set the item properties.
  pickable->falling_height = falling_height;
  pickable->will_disappear = !force_persistent && treasure.get_item().get_can_disappear();

  // Initialize the pickable item.
  if (!pickable->initialize_sprites()) {
    return nullptr;  // No valid sprite: don't create the pickable.
  }
  pickable->initialize_movement();

  return pickable;
}

/**
 * \brief Returns whether entities of this type can be obstacles for other entities.
 * \return \c true if this type of entity can be obstacle for other entities.
 */
bool Pickable::can_be_obstacle() const {
  return false;
}

/**
 * \brief Creates the sprite of this pickable treasure.
 *
 * Pickable treasures are represented with two sprites:
 * the treasure itself and, for some items, a shadow.
 *
 * \return \c true in case of success, \c false if the animation corresponding
 * to the treasure is missing.
 */
bool Pickable::initialize_sprites() {

  // Shadow sprite.
  shadow_sprite = nullptr;
  EquipmentItem& item = treasure.get_item();
  const std::string& animation = item.get_shadow();

  bool has_shadow = false;
  if (!animation.empty()) {
    shadow_sprite = std::make_shared<Sprite>("entities/shadow");
    has_shadow = shadow_sprite->has_animation(animation);
  }

  if (!has_shadow) {
    // No shadow or no such shadow animation.
    shadow_sprite = nullptr;
  }
  else {
    shadow_sprite->set_current_animation(animation);
  }

  // Main sprite.
  const std::string item_name = treasure.get_item_name();
  create_sprite("entities/items");
  Sprite& item_sprite = get_sprite();

  if( !item_sprite.has_animation(item_name)) {
    std::ostringstream oss;
    oss << "Cannot create pickable treasure '" << item_name
        << "': Sprite 'entities/items' has no animation '"
        << item_name << "'";
    Debug::error(oss.str());
    return false;
  }

  item_sprite.set_current_animation(item_name);
  int direction = treasure.get_variant() - 1;
  if (direction < 0 || direction >= item_sprite.get_nb_directions()) {
    std::ostringstream oss;
    oss << "Pickable treasure '" << item_name
        << "' has variant " << treasure.get_variant()
        << " but sprite 'entities/items' only has "
        << item_sprite.get_nb_directions() << " variant(s) in animation '"
        << item_name << "'";
    Debug::error(oss.str());
    direction = 0;  // Fallback.
  }
  item_sprite.set_current_direction(direction);
  item_sprite.enable_pixel_collisions();

  // Set the origin point and the size of the entity.
  set_size(16, 16);
  set_origin(8, 13);

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

  return true;
}

/**
 * \copydoc MapEntity::notify_created
 */
void Pickable::notify_created() {

  Detector::notify_created();

  // This entity and the map are now both ready. Notify the Lua item.
  EquipmentItem& item = get_equipment().get_item(treasure.get_item_name());
  item.notify_pickable_appeared(*this);
}

/**
 * \brief Initializes the movement of the item (if it is falling),
 * depending on its subtype.
 */
void Pickable::initialize_movement() {

  if (is_falling()) {
    set_movement(std::make_shared<FallingOnFloorMovement>(falling_height));
  }
}

/**
 * \brief Returns whether the entity is currently falling.
 * \return true if the entity is currently falling
 */
bool Pickable::is_falling() const {
  return get_falling_height() != FALLING_NONE;
}

/**
 * \brief Returns the height this pickable item falls from when it appears.
 * \return the falling height
 */
FallingHeight Pickable::get_falling_height() const {
  return falling_height;
}

/**
 * \brief Returns the treasure the player receives if he picks this item.
 * \return the treasure
 */
const Treasure& Pickable::get_treasure() const {
  return treasure;
}

/**
 * \brief Returns the entity (if any) followed by this pickable item.
 * \return the entity followed or nullptr
 */
MapEntityPtr Pickable::get_entity_followed() {
  return entity_followed;
}

/**
 * \brief This function is called by the engine when an entity overlaps the pickable item.
 *
 * If the entity is the player, we give him the item, and the map is notified
 * to destroy it.
 * \param entity_overlapping the entity overlapping the detector
 * \param collision_mode the collision mode that detected the collision
 */
void Pickable::notify_collision(MapEntity& entity_overlapping, CollisionMode /* collision_mode */) {

  if (entity_overlapping.is_hero()) {
    try_give_item_to_player();
  }
  else if (entity_followed == nullptr) {

    MapEntityPtr shared_entity_overlapping =
        std::static_pointer_cast<MapEntity>(entity_overlapping.shared_from_this());
    if (entity_overlapping.get_type() == EntityType::BOOMERANG) {
      Boomerang& boomerang = static_cast<Boomerang&>(entity_overlapping);
      if (!boomerang.is_going_back()) {
        boomerang.go_back();
      }
      entity_followed = shared_entity_overlapping;
    }
    else if (entity_overlapping.get_type() == EntityType::HOOKSHOT) {
      Hookshot& hookshot = static_cast<Hookshot&>(entity_overlapping);
      if (!hookshot.is_going_back()) {
        hookshot.go_back();
      }
      entity_followed = shared_entity_overlapping;
    }

    if (entity_followed != nullptr) {
      clear_movement();
      set_movement(std::make_shared<FollowMovement>(
          entity_followed, 0, 0, true
      ));
      falling_height = FALLING_NONE;
      set_blinking(false);
    }
  }
}

/**
 * \copydoc Detector::notify_collision(MapEntity&, Sprite&, Sprite&)
 */
void Pickable::notify_collision(
    MapEntity& other_entity,
    Sprite& /* this_sprite */,
    Sprite& other_sprite
) {
  // taking the item with the sword
  if (other_entity.is_hero()) {
    Hero& hero = static_cast<Hero&>(other_entity);
    if (other_sprite.get_animation_set_id() == hero.get_hero_sprites().get_sword_sprite_id()) {
      try_give_item_to_player();
    }
  }
}

/**
 * \brief Gives the item to the player.
 */
void Pickable::try_give_item_to_player() {

  EquipmentItem& item = treasure.get_item();

  if (!can_be_picked
      || given_to_player
      || get_game().is_dialog_enabled()
      || !get_hero().can_pick_treasure(item)) {
    return;
  }

  given_to_player = true;

  remove_from_map();

  // play the sound
  const std::string& sound_id = item.get_sound_when_picked();
  if (!sound_id.empty()) {
    Sound::play(sound_id);
  }

  // give the item
  if (item.get_brandish_when_picked()) {
    // The treasure is brandished.
    // on_obtained() will be called after the dialog.
    get_hero().start_treasure(treasure, ScopedLuaRef());
  }
  else {
    treasure.give_to_player();

    // Call on_obtained() immediately since the treasure is not brandished.
    get_lua_context().item_on_obtained(item, treasure);
    get_lua_context().map_on_obtained_treasure(get_map(), treasure);
  }
}

/**
 * \brief Sets whether the pickable treasure is blinking.
 * \param blinking true to make it blink, false to make it stop blinking
 */
void Pickable::set_blinking(bool blinking) {

  uint32_t blink_delay = blinking ? 75 : 0;

  get_sprite().set_blinking(blink_delay);

  if (shadow_sprite != nullptr) {
    shadow_sprite->set_blinking(blink_delay);
  }
}

/**
 * \brief This function is called by the map when the game is suspended or resumed.
 *
 * This is a redefinition of MapEntity::set_suspended() to suspend the timer
 * which makes the pickable item disappear after a few seconds.
 *
 * \param suspended true to suspend the entity, false to resume it
 */
void Pickable::set_suspended(bool suspended) {

  Detector::set_suspended(suspended); // suspend the animation and the movement

  if (shadow_sprite != nullptr) {
    shadow_sprite->set_suspended(suspended);
  }

  if (!suspended) {
    // suspend the timers

    uint32_t now = System::now();

    if (!can_be_picked && get_when_suspended() != 0) {
      allow_pick_date = now + (allow_pick_date - get_when_suspended());
    }

    if (will_disappear) {

      // the game is being resumed
      // recalculate the blinking date and the disappearing date
      if (get_when_suspended() != 0) {
        blink_date = now + (blink_date - get_when_suspended());
        disappear_date = now + (disappear_date - get_when_suspended());
      }
    }
  }
}

/**
 * \brief Updates the pickable item.
 *
 * This function is called repeatedly by the map.
 * This is a redefinition of MapEntity::update() to make
 * the item blink and then disappear after an amount of time.
 */
void Pickable::update() {

  // update the animations and the movement
  Detector::update();

  // update the shadow
  if (shadow_sprite != nullptr) {
    shadow_sprite->update();
  }

  shadow_xy.x = get_x();
  if (!is_falling()) {
    shadow_xy.y = get_y();
  }

  if (entity_followed != nullptr && entity_followed->is_being_removed()) {

    if (entity_followed->get_type() == EntityType::BOOMERANG ||
        entity_followed->get_type() == EntityType::HOOKSHOT) {
      // The pickable may have been dropped by the boomerang/hookshot
      // not exactly on the hero so let's fix this.
      if (get_distance(get_hero()) < 16) {
        try_give_item_to_player();
      }
    }
    entity_followed = nullptr;
  }

  if (!is_suspended()) {

    // check the timer
    uint32_t now = System::now();

    // wait 0.7 second before allowing the hero to take the item
    if (!can_be_picked && now >= allow_pick_date) {
      can_be_picked = true;
      get_hero().check_collision_with_detectors();
    }
    else {
      // make the item blink and then disappear
      if (will_disappear) {

        if (now >= blink_date && !get_sprite().is_blinking() && entity_followed == nullptr) {
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
 * \brief Draws the pickable item on the map.
 *
 * This is a redefinition of MapEntity::draw_on_map
 * to draw the shadow independently of the item movement.
 */
void Pickable::draw_on_map() {

  if (!is_drawn()) {
    return;
  }

  // draw the shadow
  if (shadow_sprite != nullptr) {
    get_map().draw_sprite(*shadow_sprite, shadow_xy);
  }

  // draw the sprite
  Detector::draw_on_map();
}

}
