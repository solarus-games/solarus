#include "entities/PickableItem.h"
#include "entities/PickableItemHeart.h"
#include "entities/PickableItemFairy.h"
#include "entities/Link.h"
#include "entities/MapEntities.h"
#include "movements/MovementFalling.h"
#include "Sprite.h"
#include "Random.h"
#include "ZSDX.h"
#include "Game.h"
#include "Savegame.h"
#include "Equipment.h"
#include "DungeonEquipment.h"
#include "Map.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "Treasure.h"

/**
 * Properties of each type of pickable item.
 */
const PickableItem::Properties PickableItem::properties[] = {

  {"", "", false, ""}, // none

  {"entities/rupee", "green", false, "picked_rupee"},  // 1 rupee
  {"entities/rupee", "blue", false, "picked_rupee"},   // 5 rupees
  {"entities/rupee", "red", false, "picked_rupee"},    // 20 rupees
  {"entities/heart", "small_stopped", false, "picked_item"}, // heart
  {"entities/magic", "small", false, "picked_item"},   // small magic
  {"entities/magic", "big", false, "picked_item"},     // big magic
  {"entities/fairy", "normal", true, "picked_item"},   // fairy
  {"entities/bomb", "1", true, "picked_item"},         // 1 bomb
  {"entities/bomb", "5", true, "picked_item"},         // 5 bombs
  {"entities/bomb", "10", true, "picked_item"},        // 10 bombs
  {"entities/arrow", "1", false, "picked_item"},       // 1 arrow
  {"entities/arrow", "5", false, "picked_item"},       // 5 arrows
  {"entities/arrow", "10", false, "picked_item"},      // 10 arrows

  {"entities/key", "small_key", false, "picked_small_key"}, // small key
  {"entities/key", "big_key", true, ""},                    // big key
  {"entities/key", "boss_key", true, ""},                   // boss key
  {"entities/heart", "piece_of_heart", true, ""},           // piece of heart
  {"entities/heart", "heart_container", true, ""},          // heart container
};

/**
 * Creates a pickable item with the specified type.
 * The type must a normal one (not NONE or RANDOM).
 * @param layer layer of the pickable item to create on the map
 * @param x x coordinate of the pickable item to create
 * @param y y coordinate of the pickable item to create
 * @param type type of pickable item to create (must be a normal item)
 * @param savegame_variable index of the savegame boolean variable storing
 * the possession state of this item,
 * only for pickable items that are saved (a key, a piece of heart...)
 */
PickableItem::PickableItem(Layer layer, int x, int y, PickableItem::ItemType type, int savegame_variable):
  Detector(COLLISION_RECTANGLE, "", layer, x, y, 0, 0),
  type(type), savegame_variable(savegame_variable),
  shadow_x(x), shadow_y(y), appear_date(SDL_GetTicks()) {

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
 * Creates a pickable item with the specified type.
 * This method acts like a constructor, except that it can return NULL in three cases:
 * - if the specified type is NONE
 * or:
 * - if the specified type is RANDOM and the random type chosen is NONE
 * or:
 * - if the specified type corresponds to a saved item (that the player can obtain only once)
 *   and the player already has found the item
 * Furthermore, the dynamic type of the object returned might be PickableItem (for a simple item)
 * or one of its subclasses (for more complex items).
 * @param layer layer of the pickable item to create on the map (ignored for a fairy)
 * @param x x coordinate of the pickable item to create
 * @param y y coordinate of the pickable item to create
 * @param type type of pickable item to create (can be a normal item, NONE
 * or RANDOM)
 * @param savegame_variable index of the savegame boolean variable storing
 * the possession state of this item,
 * only for pickable items that are saved (a key, a piece of heart...)
 * @param falling_height to make the item fall when it appears (ignored for a fairy)
 * @param will_disappear true to make the item disappear after an amout of time
 * @return the pickable item created, or NULL depending on the type
 */
PickableItem * PickableItem::create(Layer layer, int x, int y, PickableItem::ItemType type,
				    int savegame_variable, MovementFalling::FallingHeight falling_height, bool will_disappear) {

  if (type == RANDOM) {
    // pick a type at random
    type = choose_random_type();
  }

  // don't create anything if the type is NONE
  if (type == NONE) {
    return NULL;
  }

  if (type >= SMALL_KEY && type <= BOSS_KEY && !zsdx->game->get_current_map()->is_in_dungeon()) {
    DIE("Illegal pickable item type " << type << " in a dungeon");
  }

  // don't create anything if this is an item already found
  if (type > ARROW_10 && zsdx->game->get_savegame()->get_boolean(savegame_variable)) {
    return NULL;
  }

  PickableItem *item;

  switch (type) {

    // special class for the heart
  case HEART:
    item = new PickableItemHeart(layer, x, y);
    break;

    // special class for the fairy
  case FAIRY:
    item = new PickableItemFairy(x, y);
    break;

    // other items: no special class, but directly PickableItem
  default:
    item = new PickableItem(layer, x, y, type, savegame_variable);
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
 * Chooses a type of item randomly.
 * This function is called when the type of item is RANDOM.
 *
 * Some items have a low probability (fairies, big bottles of magic...)
 * while other have a high probability (hearts, rupees...).
 * The function can return NONE (this is actually the case
 * most of the time).
 * If the player is running out of hearts, magic, bombs or arrows,
 * the probability of getting what he needs is increased.
 *
 * @return a type of item (can be NONE or a normal item)
 */
PickableItem::ItemType PickableItem::choose_random_type(void) {

  PickableItem::ItemType type;

  int r = Random::get_number(1000);

  Equipment *equipment = zsdx->game->get_equipment();

  if (r < 625) {
    // give the player nothing with probability 62.5%
    type = NONE;
  }

  else if (r < 725) { // with probability 10%
    // give the player what he needs, or nothing if he doesn't need anything
    
    // does he need hearts?
    if (equipment->needs_hearts()) {
      type = HEART;
    }

    // does he need magic?
    else if (equipment->needs_magic()) {
      type = SMALL_MAGIC;
    }

    // does he need bombs?
    else if (equipment->needs_bombs()) {
      type = BOMB_5;
    }

    // does he need arrows?
    else if (equipment->needs_arrows()) {
      type = ARROW_5;
    }

    // well, he does not need anything
    else {
      type = NONE;
    }
  }

  // the remaining 27.5% are distributed as follows:

  else if (r < 825) { type = HEART; }       // heart: 10%
  else if (r < 875) { type = RUPEE_1; }     // 1 rupee: 5%
  else if (r < 890) { type = RUPEE_5; }     // 5 rupees: 1.5%
  else if (r < 895) { type = RUPEE_20; }    // 20 rupees: 0.5%
  else if (r < 897) { type = FAIRY; }       // fairy: 0.2%
  else {

    bool has_bombs = equipment->has_inventory_item(InventoryItem::BOMBS);
    bool has_bow = equipment->has_inventory_item(InventoryItem::BOW);
    bool has_magic = equipment->get_max_magic() > 0;

    if (r < 922)      { type = has_magic ? SMALL_MAGIC : NONE; }
    else if (r < 930) { type = has_magic ? BIG_MAGIC : NONE; }
    else if (r < 950) { type = has_bombs ? BOMB_1 : NONE; }
    else if (r < 960) { type = has_bombs ? BOMB_5 : NONE; }
    else if (r < 965) { type = has_bombs ? BOMB_10 : NONE; }
    else if (r < 985) { type = has_bow ? ARROW_1 : NONE; }
    else if (r < 995) { type = has_bow ? ARROW_5 : NONE; }
    else              { type = has_bow ? ARROW_10 : NONE; }
  }

  return type;
}

/**
 * Creates the sprite of this pickable item,
 * depending on its type.
 * The pickable items are represented with two sprites:
 * the item itself and its shadow, except the fairy whose
 * shadow is part of its sprite.
 */
void PickableItem::initialize_sprites(void) {

  // create the shadow
  shadow_sprite = new Sprite("entities/shadow");

  if (properties[type].big_shadow) {
    shadow_sprite->set_current_animation("big");
  }

  // create the sprite and set its animation
  create_sprite(properties[type].animation_set_id);
  Sprite *item_sprite = get_last_sprite();
  item_sprite->set_current_animation(properties[type].animation_name);

  // set the origin point and the size of the entity
  set_rectangle_from_sprite();

  // initialize the sprite removal
  if (will_disappear) {
    Uint32 now = SDL_GetTicks();
    can_be_picked = false;
    allow_pick_date = now + 700;  // the player can take the item after 0.7s
    blink_date = now + 8000;      // the item blinks at 8s
    disappear_date = now + 10000; // the item disappears at 10s
  }
  else {
    can_be_picked = true;
  }
}

/**
 * Initializes the movement of the item (if it is falling),
 * depending on its type.
 */
void PickableItem::initialize_movement(void) {

  if (is_falling()) {
    set_movement(new MovementFalling(falling_height));
  }
}

/**
 * Returns whether the entity is falling when it appears.
 * @return true if the entity is falling when it appears
 */
bool PickableItem::is_falling(void) {
  return falling_height != MovementFalling::NONE;
}

/**
 * This function is called by the engine when an entity overlaps the pickable item.
 * This is a redefinition of Detector::collision().
 * If the entity is the player, we give him the item, and the map is notified
 * to destroy it.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void PickableItem::collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping->is_hero() && can_be_picked) {

    map->get_entities()->remove_pickable_item(this);
    give_item_to_player();
  }
}

/**
 * Gives the item to the player.
 */
void PickableItem::give_item_to_player(void) {

  // play the sound
  if (properties[type].sound != "") {
    Sound *sound = ResourceManager::get_sound(properties[type].sound);
    sound->play();
  }

  // give the item
  Game *game = zsdx->game;
  Equipment *equipment = game->get_equipment();
  Treasure *treasure;

  switch (type) {

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
    game->get_savegame()->set_boolean(savegame_variable, true);
    break;

  case BIG_KEY:
    treasure = new Treasure(Treasure::BIG_KEY, savegame_variable);
    game->give_treasure(treasure);
    break;

  case BOSS_KEY:
    treasure = new Treasure(Treasure::BOSS_KEY, savegame_variable);
    game->give_treasure(treasure);
    break;

  case PIECE_OF_HEART:
    treasure = new Treasure(Treasure::PIECE_OF_HEART, savegame_variable);
    game->give_treasure(treasure);
    break;

  case HEART_CONTAINER:
    treasure = new Treasure(Treasure::HEART_CONTAINER, savegame_variable);
    game->give_treasure(treasure);
    break;

  default:
    DIE("Unknown pickable item type '" << type << "'");
    break;

  }
}

/**
 * Sets whether the pickable item is blinking.
 * @param blinking true to make it blink, false to make it stop blinking
 */
void PickableItem::set_blinking(bool blinking) {

  Uint32 blink_delay = blinking ? 75 : 0;

  Sprite *item_sprite = get_sprite(0);

  item_sprite->set_blinking(blink_delay);
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

  if (will_disappear) {
    // suspend the timer
    Uint32 now = SDL_GetTicks();
    
    if (!suspended) {
      // the game is being resumed
      // recalculate the blinking date and the disappearing date
      if (when_suspended != 0) {
	blink_date = now + (blink_date - when_suspended);
	disappear_date = now + (disappear_date - when_suspended);
      }
    }
    else {
      // the game is being suspended
      set_blinking(false);
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

  // check the timer
  Uint32 now = SDL_GetTicks();

  // wait 0.7 second before allowing Link to take the item
  if (!can_be_picked && now >= allow_pick_date) {
    can_be_picked = true;
    map->check_collision_with_detectors(zsdx->game->get_link());
  }
  else {
    // make the item blink and then disappear
    if (will_disappear && !is_suspended()) {

      Sprite *item_sprite = get_sprite(0);
    
      if (now >= blink_date && !item_sprite->is_blinking()) {
	set_blinking(true);
      }
    
      if (now >= disappear_date) {
	map->get_entities()->remove_pickable_item(this);
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
