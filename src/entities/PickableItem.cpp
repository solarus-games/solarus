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
 * @param treasure the treasure to give when the item is picked (cannot be NULL)
 */
PickableItem::PickableItem(Layer layer, int x, int y, const Treasure &treasure):
  Detector(COLLISION_RECTANGLE, "", layer, x, y, 0, 0), treasure(treasure),
  shadow_xy(Rectangle(x, y)), appear_date(System::now()), is_following_boomerang(false) {

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
      FALLING_NONE, false);
}

/**
 * @brief Creates a pickable item with the specified subtype.
 *
 * This method acts like a constructor, except that it can return NULL in several cases:
 * - the treasure is saved and the player already has it,
 * or:
 * - the treasure is empty ("_none"),
 * or:
 * - the item cannot be picked by the hero yet.
 *
 * @param game the current game
 * @param layer layer of the pickable item to create on the map
 * @param x x coordinate of the pickable item to create
 * @param y y coordinate of the pickable item to create
 * @param treasure the treasure to give, or NULL
 * @param falling_height to make the item fall when it appears
 * @param will_disappear true to make the item disappear after an amout of time
 * @return the pickable item created, or NULL depending on the subtype
 */
PickableItem * PickableItem::create(Game &game, Layer layer, int x, int y, const Treasure &treasure,
    FallingHeight falling_height, bool will_disappear) {

  // don't create anything if there is no treasure to give
  if (treasure.is_empty()) {
    return NULL;
  }

  PickableItem *item = new PickableItem(layer, x, y, treasure);

  // set the item properties
  item->falling_height = falling_height;
  item->will_disappear = will_disappear;

  // initialize the item
  item->initialize_sprites();
  item->initialize_movement();

  // notify the item script
  game.get_equipment().get_item_script(treasure.get_item_name()).event_appear(*item);

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
  return falling_height != FALLING_NONE;
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

  if (entity_overlapping.is_hero() && can_be_picked) {
    remove_from_map();
    give_item_to_player();
  }
  else if (entity_overlapping.get_type() == BOOMERANG && !is_following_boomerang) {

    Boomerang &boomerang = (Boomerang&) entity_overlapping;
    
    clear_movement();
    set_movement(new FollowMovement(&boomerang, 0, 0, true));
    is_following_boomerang = true;
    falling_height = FALLING_NONE;
    set_blinking(false);

    if (!boomerang.is_going_back()) {
      boomerang.go_back();
    }
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

    if (!can_be_picked) {
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

  if (!is_falling()) {
    shadow_xy.set_xy(get_xy());
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

	if (now >= blink_date && !get_sprite().is_blinking() && !is_following_boomerang) {
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

