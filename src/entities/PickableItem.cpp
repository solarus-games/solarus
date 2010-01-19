/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "entities/PickableItem.h"
#include "entities/PickableItemHeart.h"
#include "entities/PickableItemFairy.h"
#include "entities/Hero.h"
#include "entities/MapEntities.h"
#include "entities/Boomerang.h"
#include "movements/FallingOnFloorMovement.h"
#include "movements/FollowMovement.h"
#include "Sprite.h"
#include "Game.h"
#include "Savegame.h"
#include "Equipment.h"
#include "DungeonEquipment.h"
#include "Map.h"
#include "ResourceManager.h"
#include "Treasure.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Random.h"
#include "lowlevel/System.h"
#include "lowlevel/FileTools.h"

/**
 * Features of each subtype of pickable item.
 */
const PickableItem::Features PickableItem::features[] = {

  {"", "", false, "", false, false, false}, // none

  {"entities/rupee", "green", false, "picked_rupee", true, false, false},        // 1 rupee
  {"entities/rupee", "blue", false, "picked_rupee", true, false, false},         // 5 rupees
  {"entities/rupee", "red", false, "picked_rupee", true, false, false},          // 20 rupees
  {"entities/heart", "small_stopped", false, "picked_item", true, false, false}, // heart
  {"entities/magic", "small", false, "picked_item", true, false, false},         // small magic
  {"entities/magic", "big", false, "picked_item", true, false, false},           // big magic
  {"entities/fairy", "normal", true, "picked_item", true, false, false},         // fairy
  {"entities/pickable_bomb", "1", true, "picked_item", true, false, false},      // 1 bomb
  {"entities/pickable_bomb", "5", true, "picked_item", true, false, false},      // 5 bombs
  {"entities/pickable_bomb", "10", true, "picked_item", true, false, false},     // 10 bombs
  {"entities/pickable_arrow", "1", false, "picked_item", true, false, false},    // 1 arrow
  {"entities/pickable_arrow", "5", false, "picked_item", true, false, false},    // 5 arrows
  {"entities/pickable_arrow", "10", false, "picked_item", true, false, false},   // 10 arrows

  {"entities/key", "small_key", false, "picked_small_key", false, true, false},  // small key
  {"entities/key", "big_key", true, "", false, true, true},                      // big key
  {"entities/key", "boss_key", true, "", false, true, true},                     // boss key
  {"entities/heart", "piece_of_heart", true, "", false, true, false},            // piece of heart
  {"entities/heart", "heart_container", true, "", false, true, false},           // heart container
};

/**
 * Creates a pickable item with the specified subtype.
 * The subtype cannot be NONE.
 * @param layer layer of the pickable item to create on the map
 * @param x x coordinate of the pickable item to create
 * @param y y coordinate of the pickable item to create
 * @param subtype subtype of pickable item to create (must be a normal item)
 * @param savegame_variable index of the savegame boolean variable storing
 * the possession state of this item,
 * only for pickable items that are saved (a key, a piece of heart...)
 */
PickableItem::PickableItem(Layer layer, int x, int y, PickableItem::Subtype subtype, int savegame_variable):
  Detector(COLLISION_RECTANGLE, "", layer, x, y, 0, 0),
  subtype(subtype), savegame_variable(savegame_variable),
  shadow_x(x), shadow_y(y), appear_date(System::now()), is_following_boomerang(false) {

}

/**
 * Destructor.
 */
PickableItem::~PickableItem(void) {

  if (shadow_sprite != NULL) {
    delete shadow_sprite;
  }
}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType PickableItem::get_type() {
  return PICKABLE_ITEM;
}

/**
 * Creates an instance from an input stream.
 * The input stream must respect the syntax of this entity type.
 * @param is an input stream
 * @param layer the layer
 * @param x x coordinate of the entity
 * @param y y coordinate of the entity
 * @return the instance created
 */
MapEntity * PickableItem::parse(std::istream &is, Layer layer, int x, int y) {

  int subtype, savegame_variable;

  FileTools::read(is, subtype);
  FileTools::read(is, savegame_variable);

  return create(Layer(layer), x, y, Subtype(subtype), savegame_variable, FALLING_NONE, false);
}

/**
 * Creates a pickable item with the specified subtype.
 * This method acts like a constructor, except that it can return NULL if the specified subtype is NONE.
 * Furthermore, the dynamic type of the object returned might be PickableItem (for a simple item)
 * or one of its subclasses (for more complex items).
 * @param layer layer of the pickable item to create on the map (ignored for a fairy)
 * @param x x coordinate of the pickable item to create
 * @param y y coordinate of the pickable item to create
 * @param subtype subtype of pickable item to create (can be a normal item, NONE
 * or RANDOM)
 * @param savegame_variable index of the savegame boolean variable storing
 * the possession state of this item,
 * only for pickable items that are saved (a key, a piece of heart...)
 * @param falling_height to make the item fall when it appears (ignored for a fairy)
 * @param will_disappear true to make the item disappear after an amout of time
 * @return the pickable item created, or NULL depending on the subtype
 */
PickableItem * PickableItem::create(Layer layer, int x, int y, PickableItem::Subtype subtype,
				    int savegame_variable, FallingHeight falling_height, bool will_disappear) {

  // don't create anything if the subtype is NONE
  if (subtype == NONE) {
    return NULL;
  }

  // check the savegame variable
  if (savegame_variable < -1) {
    DIE("Invalid savegame variable: " << savegame_variable);
  }

  if (must_be_saved(subtype) && savegame_variable == -1) {
    DIE("This subtype of pickable item must be saved: " << subtype);
  }

  PickableItem *item;

  switch (subtype) {

    // special class for the heart
  case HEART:
    item = new PickableItemHeart(layer, x, y);
    break;

    // special class for the fairy
  case FAIRY:
    item = new PickableItemFairy(x, y);
    break;

    // other items: no special class, directly PickableItem
  default:
    item = new PickableItem(layer, x, y, subtype, savegame_variable);
    break;
  }

  // set the item properties
  item->savegame_variable = savegame_variable; 
  item->falling_height = falling_height;
  item->will_disappear = will_disappear;

  // initialize the item
  item->initialize_sprites();
  item->initialize_movement();

  return item;
}

/**
 * Once the entity is created, this function is called to check whether
 * the entity created can be added to the specified map and in the current game state.
 * @param map the map where this entity is about to be added
 * @return true if the entity can be added, false otherwise
 */
bool PickableItem::can_be_added(Map *map) {

  Savegame *savegame = map->get_game()->get_savegame();

  if (subtype == RANDOM) {
    // now is the time to pick the random subtype
    subtype = choose_random_subtype(savegame->get_equipment());
  }

  return
    subtype != NONE &&                                                      // the item should exist
    (savegame_variable == -1 || !savegame->get_boolean(savegame_variable)) && // don't add an item already found
    !is_subtype_locked(savegame->get_equipment());                             // the item should be available for the player

}

/**
 * Returns whether this item's subtype is locked,
 * e.g. whether the player has not unlocked it yet in its equipment
 * @param equipment the player's equipment
 * @return true if the subtype is locked
 */
bool PickableItem::is_subtype_locked(Equipment *equipment) {

  bool has_bombs = equipment->has_inventory_item(INVENTORY_BOMBS);
  bool has_bow = equipment->has_inventory_item(INVENTORY_BOW);
  bool has_magic = equipment->get_max_magic() > 0;

  return
    (subtype >= BOMB_1 && subtype <= BOMB_10 && !has_bombs) ||
    (subtype >= ARROW_1 && subtype <= ARROW_10 && !has_bow) ||
    (subtype >= SMALL_MAGIC && subtype <= BIG_MAGIC && !has_magic);
}

/**
 * Chooses a subtype of item randomly among all subtypes
 * (event the ones that the player cannot get yet).
 * This function is called when the subtype of item is RANDOM.
 *
 * Some items have a low probability (fairies, big bottles of magic...)
 * while other have a high probability (hearts, rupees...).
 * The function can return NONE (this is actually the case
 * most of the time).
 * If the player is running out of hearts, magic, bombs or arrows,
 * the probability of getting what he needs is increased.
 *
 * @param equipment the player's equipment
 * @return a subtype of item (can be NONE or a normal item)
 */
PickableItem::Subtype PickableItem::choose_random_subtype(Equipment *equipment) {

  PickableItem::Subtype subtype;

  int r = Random::get_number(1000);

  if (r < 665) {
    // give the player nothing with probability 66.5%
    subtype = NONE;
  }

  else if (r < 765) { // with probability 10%
    // give the player what he needs, or nothing if he doesn't need anything
    
    // does he need hearts?
    if (equipment->needs_hearts()) {
      subtype = HEART;
    }

    // does he need magic?
    else if (equipment->needs_magic()) {
      subtype = SMALL_MAGIC;
    }

    // does he need bombs?
    else if (equipment->needs_bombs()) {
      subtype = BOMB_5;
    }

    // does he need arrows?
    else if (equipment->needs_arrows()) {
      subtype = ARROW_5;
    }

    // well, he does not need anything
    else {
      subtype = NONE;
    }
  }

  // the remaining 23.5% are distributed as follows:

  else if (r < 865) { subtype = HEART; }       // heart:       10%
  else if (r < 915) { subtype = RUPEE_1; }     // 1 rupee:     5%
  else if (r < 930) { subtype = RUPEE_5; }     // 5 rupees:    1.5%
  else if (r < 935) { subtype = RUPEE_20; }    // 20 rupees:   0.5%
  else if (r < 937) { subtype = FAIRY; }       // fairy:       0.2%
  else if (r < 962) { subtype = SMALL_MAGIC; } // small magic: 2.5%
  else if (r < 970) { subtype = BIG_MAGIC; }   // big magic:   0.8%
  else if (r < 978) { subtype = BOMB_1; }      // 1 bomb:      0.8%
  else if (r < 983) { subtype = BOMB_5; }      // 5 bombs:     0.5%
  else if (r < 985) { subtype = BOMB_10; }     // 10 bombs:    0.2%
  else if (r < 993) { subtype = ARROW_1; }     // 1 arrow:     0.8%
  else if (r < 998) { subtype = ARROW_5; }     // 5 arrows:    0.5%
  else              { subtype = ARROW_10; }    // 10 arrows:   0.2%

  return subtype;
}

/**
 * Returns whether this subtype of pickable item can disappear after a delay.
 * Items saved like keys and pieces of hearts cannot disappear.
 * @return true if this subtype of pickable item can disappear
 */
bool PickableItem::can_disappear(Subtype subtype) {

  if (subtype == RANDOM) {
    return true;
  }

  return features[subtype].can_disappear;
}

/**
 * Returns whether this subtype of pickable item must be saved.
 * @return true if this subtype of pickable item must be saved
 */
bool PickableItem::must_be_saved(Subtype subtype) {

  if (subtype == RANDOM) {
    return false;
  }

  return features[subtype].must_be_saved;
}

/**
 * Creates the sprite of this pickable item,
 * depending on its subtype.
 * The pickable items are represented with two sprites:
 * the item itself and its shadow, except the fairy whose
 * shadow is part of its sprite.
 */
void PickableItem::initialize_sprites(void) {

  // create the shadow
  shadow_sprite = new Sprite("entities/shadow");

  if (features[subtype].big_shadow) {
    shadow_sprite->set_current_animation("big");
  }

  // create the sprite and set its animation
  create_sprite(features[subtype].animation_set_id);
  Sprite *item_sprite = get_sprite();
  item_sprite->set_current_animation(features[subtype].animation_name);

  // set the origin point and the size of the entity
  set_bounding_box_from_sprite();

  uint32_t now = System::now();

  if (falling_height != FALLING_NONE) {
    allow_pick_date = now + 700;  // the player can take the item after 0.7s
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
 * Initializes the movement of the item (if it is falling),
 * depending on its subtype.
 */
void PickableItem::initialize_movement(void) {

  if (is_falling()) {
    set_movement(new FallingOnFloorMovement(falling_height));
  }
}

/**
 * Returns whether the entity is currently falling.
 * @return true if the entity is currently falling
 */
bool PickableItem::is_falling(void) {
  return falling_height != FALLING_NONE;
}

/**
 * This function is called by the engine when an entity overlaps the pickable item.
 * If the entity is the player, we give him the item, and the map is notified
 * to destroy it.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void PickableItem::notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping->is_hero() && can_be_picked) {

    map->get_entities()->remove_entity(this);
    give_item_to_player();
  }
  else if (entity_overlapping->get_type() == BOOMERANG && !is_following_boomerang) {

    Boomerang *boomerang = (Boomerang*) entity_overlapping;
    
    clear_movement();
    set_movement(new FollowMovement(entity_overlapping, 0, 0, false));
    is_following_boomerang = true;
    falling_height = FALLING_NONE;
    set_blinking(false);

    if (!boomerang->is_going_back()) {
      boomerang->go_back();
    }
  }
}

/**
 * Gives the item to the player.
 */
void PickableItem::give_item_to_player(void) {

  // some items can exist only in a dungeon
  if (features[subtype].must_be_in_dungeon && !game->get_current_map()->is_in_dungeon()) {
    DIE("Illegal pickable item subtype " << subtype << " in a dungeon");
  }

  // play the sound
  if (features[subtype].sound != "") {
    Sound *sound = ResourceManager::get_sound(features[subtype].sound);
    sound->play();
  }

  // give the item
  Equipment *equipment = game->get_equipment();
  Treasure *treasure = NULL;

  switch (subtype) {

  case RUPEE_1:
    equipment->add_rupees(1);
    break;

  case RUPEE_5:
    equipment->add_rupees(5);
    break;

  case RUPEE_20:
    equipment->add_rupees(20);
    break;

  case HEART:
    equipment->add_hearts(4);
    break;

  case SMALL_MAGIC:
    equipment->add_magic(6);
    break;

  case BIG_MAGIC:
    equipment->add_magic(42);
    break;

  case FAIRY:
    // done in the subclass
    break;

  case BOMB_1:
    equipment->add_bombs(1);
    break;

  case BOMB_5:
    equipment->add_bombs(5);
    break;

  case BOMB_10:
    equipment->add_bombs(10);
    break;

  case ARROW_1:
    equipment->add_arrows(1);
    break;

  case ARROW_5:
    equipment->add_arrows(5);
    break;

  case ARROW_10:
    equipment->add_arrows(10);
    break;

  case SMALL_KEY:
    equipment->add_small_key();
    break;

  case BIG_KEY:
    treasure = new Treasure(game, Treasure::BIG_KEY, savegame_variable);
    game->give_treasure(treasure);
    break;

  case BOSS_KEY:
    treasure = new Treasure(game, Treasure::BOSS_KEY, savegame_variable);
    game->give_treasure(treasure);
    break;

  case PIECE_OF_HEART:
    treasure = new Treasure(game, Treasure::PIECE_OF_HEART, savegame_variable);
    game->give_treasure(treasure);
    break;

  case HEART_CONTAINER:
    treasure = new Treasure(game, Treasure::HEART_CONTAINER, savegame_variable);
    game->give_treasure(treasure);
    break;

  default:
    DIE("Unknown pickable item subtype '" << subtype << "'");
    break;

  }

  if (savegame_variable != -1 && treasure == NULL) {
    // set the savegame variable unless the treasure class already did it
    game->get_savegame()->set_boolean(savegame_variable, true);
  }
}

/**
 * Sets whether the pickable item is blinking.
 * @param blinking true to make it blink, false to make it stop blinking
 */
void PickableItem::set_blinking(bool blinking) {

  uint32_t blink_delay = blinking ? 75 : 0;

  get_sprite()->set_blinking(blink_delay);
  shadow_sprite->set_blinking(blink_delay);
}

/**
 * This function is called by the map when the game is suspended or resumed.
 * This is a redefinition of MapEntity::set_suspended() to suspend the timer
 * which makes the pickable item disappear after a few seconds.
 * @param suspended true to suspend the entity, false to resume it
 */
void PickableItem::set_suspended(bool suspended) {

  MapEntity::set_suspended(suspended); // suspend the animation and the movement
  shadow_sprite->set_suspended(suspended);

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
 * Updates the pickable item.
 * This function is called repeatedly by the map.
 * This is a redefinition of MapEntity::update() to make
 * the item blink and then disappear after an amount of time.
 */
void PickableItem::update(void) {

  // update the animations and the movement
  MapEntity::update();

  // update the shadow
  shadow_sprite->update();

  if (!is_falling()) {
    shadow_x = get_x();
    shadow_y = get_y();
  }

  if (!is_suspended()) {

    // check the timer
    uint32_t now = System::now();

    // wait 0.7 second before allowing the hero to take the item
    if (!can_be_picked && now >= allow_pick_date) {
      can_be_picked = true;
      map->check_collision_with_detectors(game->get_hero());
    }
    else {
      // make the item blink and then disappear
      if (will_disappear) {

	if (now >= blink_date && !get_sprite()->is_blinking() && !is_following_boomerang) {
	  set_blinking(true);
	}
    
	if (now >= disappear_date) {
	  map->get_entities()->remove_entity(this);
	}
      }
    }
  }
}

/**
 * Displays the pickable item on the map.
 * This is a redefinition of MapEntity::display_on_map
 * to display the shadow independently of the item movement.
 */
void PickableItem::display_on_map(void) {

  // display the shadow
  map->display_sprite(shadow_sprite, shadow_x, shadow_y);

  // display the sprite
  MapEntity::display_on_map();
}

