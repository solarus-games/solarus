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
#include "entities/PickableItem.h"
#include "entities/Hero.h"
#include "entities/Boomerang.h"
#include "entities/Hookshot.h"
#include "movements/FallingOnFloorMovement.h"
#include "movements/FollowMovement.h"
#include "Game.h"
#include "Map.h"
#include "Sprite.h"
#include "ItemProperties.h"
#include "lua/ItemScript.h"
#include "lowlevel/System.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Sound.h"

/**
 * @brief Creates a pickable item with the specified subtype.
 * @param layer layer of the pickable item to create on the map
 * @param x x coordinate of the pickable item to create
 * @param y y coordinate of the pickable item to create
 * @param treasure the treasure to give when the item is picked
 */
PickableItem::PickableItem(Layer layer, int x, int y, const Treasure &treasure):
  Detector(COLLISION_RECTANGLE | COLLISION_SPRITE, "", layer, x, y, 0, 0),
  treasure(treasure),
  layer_independent_collisions(false),
  shadow_xy(Rectangle(x, y)),
  appear_date(System::now()),
  entity_followed(NULL) {

}

/**
 * @brief Destructor.
 */
PickableItem::~PickableItem() {

  delete shadow_sprite;
}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType PickableItem::get_type() {
  return PICKABLE_ITEM;
}

/**
 * @brief Creates an instance from an input stream.
 *
 * The input stream must respect the syntax of this entity type.
 *
 * @param game the game that will contain the entity created
 * @param is an input stream
 * @param layer the layer
 * @param x x coordinate of the entity
 * @param y y coordinate of the entity
 * @return the instance created
 */
MapEntity* PickableItem::parse(Game &game, std::istream &is, Layer layer, int x, int y) {

  std::string treasure_name;
  int treasure_variant, treasure_savegame_variable;

  FileTools::read(is, treasure_name);
  FileTools::read(is, treasure_variant);
  FileTools::read(is, treasure_savegame_variable);

  return create(game, Layer(layer), x, y,
      Treasure(game, treasure_name, treasure_variant, treasure_savegame_variable),
      FALLING_NONE, true);
}

/**
 * @brief Creates a pickable item with the specified subtype.
 *
 * This method acts like a constructor, except that it can return NULL in several cases:
 * - the treasure is saved and the player already has it,
 * or:
 * - the treasure is empty ("_none"),
 * or:
 * - the treasure is random ("_random") and the random content chosen is "_none",
 * or:
 * - the item cannot be picked by the hero yet.
 *
 * @param game the current game
 * @param layer layer of the pickable item to create on the map
 * @param x x coordinate of the pickable item to create
 * @param y y coordinate of the pickable item to create
 * @param treasure the treasure to give, possibly "_none" or "_random"
 * @param falling_height to make the item fall when it appears
 * @param force_persistent true to make the item stay forever (otherwise, the properties of the item
 * decide if it disappears after some time)
 * @return the pickable item created, or NULL
 */
PickableItem* PickableItem::create(Game &game, Layer layer, int x, int y, Treasure treasure,
    FallingHeight falling_height, bool force_persistent) {

  treasure.decide_content();

  // don't create anything if there is no treasure to give
  if (treasure.is_empty()) {
    return NULL;
  }

  PickableItem *item = new PickableItem(layer, x, y, treasure);

  // set the item properties
  item->falling_height = falling_height;
  item->will_disappear = !force_persistent && treasure.get_item_properties().can_disappear();

  // initialize the item
  item->initialize_sprites();
  item->initialize_movement();

  return item;
}

/**
 * @brief Creates the sprite of this pickable item,
 * depending on its subtype.
 *
 * The pickable items are represented with two sprites:
 * the item itself and its shadow, except the fairy whose
 * shadow is part of its sprite.
 */
void PickableItem::initialize_sprites() {

  ItemProperties &properties = treasure.get_item_properties();

  // create the shadow
  switch (properties.get_shadow_size()) {

    case ItemProperties::SHADOW_SMALL:
      shadow_sprite = new Sprite("entities/shadow");
      shadow_sprite->set_current_animation("small");
      break;

    case ItemProperties::SHADOW_BIG:
      shadow_sprite = new Sprite("entities/shadow");
      shadow_sprite->set_current_animation("big");
      break;
    
    case ItemProperties::SHADOW_NONE:
      shadow_sprite = NULL;
      break;
  }

  // create the sprite and set its animation
  create_sprite("entities/items");
  Sprite &item_sprite = get_sprite();
  item_sprite.set_current_animation(treasure.get_item_name());
  item_sprite.set_current_direction(treasure.get_variant() - 1);
  item_sprite.enable_pixel_collisions();

  // set the origin point and the size of the entity
  set_bounding_box_from_sprite();

  uint32_t now = System::now();

  if (falling_height != FALLING_NONE) {

    allow_pick_date = now + 700;  // the player will be allowed to take the item after 0.7 seconds
    can_be_picked = false;
  }
  else {
    can_be_picked = true;
  }

  // initialize the sprite removal
  if (will_disappear) {
    blink_date = now + 8000;      // the item blinks at 8s
    disappear_date = now + 10000; // the item disappears at 10s
  }
}

/**
 * @brief Notifies this entity that it has just been added to a map.
 * @param map the map
 */
void PickableItem::set_map(Map& map) {

  MapEntity::set_map(map);

  if (map.is_started()) {
    // notify the item script
    get_equipment().get_item_script(treasure.get_item_name()).event_appear(*this);
  }
  // otherwise, notify_map_started() will do the job
}

/**
 * @brief Notifies this entity that its map has just become active.
 */
void PickableItem::notify_map_started() {

  MapEntity::notify_map_started();

  // notify the item script
  get_equipment().get_item_script(treasure.get_item_name()).event_appear(*this);
}

/**
 * @brief Initializes the movement of the item (if it is falling),
 * depending on its subtype.
 */
void PickableItem::initialize_movement() {

  if (is_falling()) {
    set_movement(new FallingOnFloorMovement(falling_height));
  }
}

/**
 * @brief Returns whether the entity is currently falling.
 * @return true if the entity is currently falling
 */
bool PickableItem::is_falling() {
  return get_falling_height() != FALLING_NONE;
}

/**
 * @brief Returns the height this pickable item falls from when it appears.
 * @return the falling height
 */
FallingHeight PickableItem::get_falling_height() {
  return falling_height;
}

/**
 * @brief Returns whether this entity can have collisions with entities even if
 * they are not on the same layer.
 * @return true if this entity can collide with entities that are on another layer
 */
bool PickableItem::has_layer_independent_collisions() {
  return layer_independent_collisions;
}

/**
 * @brief Sets whether this entity can have collisions with entities even if
 * they are not on the same layer.
 * @param independent true if this entity can collide with entities that are on another layer
 */
void PickableItem::set_layer_independent_collisions(bool independent) {
  this->layer_independent_collisions = independent;
}

/**
 * @brief Returns the treasure the player receives if he picks this item.
 * @return the treasure
 */
const Treasure& PickableItem::get_treasure() {
  return treasure;
}

/**
 * @brief Returns the entity (if any) followed by this pickable item.
 * @return the entity followed or NULL
 */
MapEntity* PickableItem::get_entity_followed() {
  return entity_followed;
}

/**
 * @brief Notifies this pickable item that its movement has just changed.
 */
void PickableItem::notify_movement_changed() {

  if (is_on_map()) {
    // notify the item script
    get_equipment().get_item_script(treasure.get_item_name()).event_movement_changed(*this);
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
void PickableItem::notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping.is_hero()
      && can_be_picked
      && !get_game().is_showing_message()) {
    remove_from_map();
    give_item_to_player();
  }
  else if (entity_followed == NULL) {
    
    if (entity_overlapping.get_type() == BOOMERANG) {
      Boomerang &boomerang = (Boomerang&) entity_overlapping;
      if (!boomerang.is_going_back()) {
        boomerang.go_back();
      }
      entity_followed = &boomerang;
    }
    else if (entity_overlapping.get_type() == HOOKSHOT) {
      Hookshot &hookshot = (Hookshot&) entity_overlapping;
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
void PickableItem::notify_collision(MapEntity &other_entity, Sprite &other_sprite, Sprite &this_sprite) {

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
void PickableItem::give_item_to_player() {

  ItemProperties &properties = treasure.get_item_properties();

  // play the sound
  Sound::play(properties.get_sound_when_picked());

  // give the item
  if (properties.is_brandished_when_picked()) {
    get_hero().start_treasure(treasure);
  }
  else {
    treasure.give_to_player();
  }
}

/**
 * @brief Sets whether the pickable item is blinking.
 * @param blinking true to make it blink, false to make it stop blinking
 */
void PickableItem::set_blinking(bool blinking) {

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
void PickableItem::set_suspended(bool suspended) {

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
void PickableItem::update() {

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

  if (!is_suspended()) {

    // check the timer
    uint32_t now = System::now();

    // wait 0.7 second before allowing the hero to take the item
    if (!can_be_picked && now >= allow_pick_date) {
      can_be_picked = true;
      get_map().check_collision_with_detectors(get_hero());
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
 * @brief Displays the pickable item on the map.
 *
 * This is a redefinition of MapEntity::display_on_map
 * to display the shadow independently of the item movement.
 */
void PickableItem::display_on_map() {

  // display the shadow
  if (shadow_sprite != NULL) {
    get_map().display_sprite(*shadow_sprite, shadow_xy.get_x(), shadow_xy.get_y());
  }

  // display the sprite
  MapEntity::display_on_map();
}

