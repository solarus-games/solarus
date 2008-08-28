#include "entities/PickableItem.h"
#include "entities/PickableItemHeart.h"
#include "entities/PickableItemFairy.h"
#include "entities/Link.h"
#include "movements/MovementFalling.h"
#include "Sprite.h"
#include "Random.h"
#include "ZSDX.h"
#include "Game.h"
#include "Savegame.h"
#include "Equipment.h"
#include "Map.h"
#include "ResourceManager.h"
#include "Sound.h"

/**
 * This structure defines the properties of a pickable item type.
 */
struct PickableItemProperties {
  SpriteAnimationsId sprite_animations_id; /**< animation set used for this type of pickable item */
  string animation_name;                   /**< name of the animation */
  bool big_shadow;                         /**< true if the pickable item has a big shadow, false for a small shadow */
  SoundId sound;                           /**< the sound played when the player gets the item */
};

/**
 * Properties of each type of pickable item.
 */
static const PickableItemProperties properties[] = {
  {"", "", false, ""}, // none
  {"entities/rupee", "green", false, "picked_rupee"},  // 1 rupee
  {"entities/rupee", "blue", false, "picked_rupee"},   // 5 rupees
  {"entities/rupee", "red", false, "picked_rupee"},    // 20 rupees
  {"entities/heart", "stopped", false, "picked_item"}, // heart
  {"entities/magic", "small", false, "picked_item"},   // small magic
  {"entities/magic", "big", false, "picked_item"},     // big magic
  {"entities/fairy", "normal", true, "picked_item"},   // fairy
  {"entities/bomb", "1", true, "picked_item"},         // 1 bomb
  {"entities/bomb", "5", true, "picked_item"},         // 5 bombs
  {"entities/bomb", "10", true, "picked_item"},        // 10 bombs
  {"entities/arrow", "1", false, "picked_item"},       // 1 arrow
  {"entities/arrow", "5", false, "picked_item"},       // 5 arrows
  {"entities/arrow", "10", false, "picked_item"},      // 10 arrows
};

/**
 * Creates a pickable item with the specified type.
 * The type must a normal one (not PICKABLE_ITEM_NONE or PICKABLE_ITEM_RANDOM).
 * @param map the map
 * @param layer layer of the pickable item to create on the map
 * @param x x coordinate of the pickable item to create
 * @param y y coordinate of the pickable item to create
 * @param type type of pickable item to create (must be a normal item)
 * @param unique_id unique id of the item, for certain kinds of items only (a key, a piece of heart...)
 */
PickableItem::PickableItem(Map *map, Layer layer, int x, int y,
			   PickableItemType type, int unique_id):
  EntityDetector(COLLISION_WITH_ENTITY_RECTANGLE, "", layer, x, y, 0, 0),
  map(map), type(type), unique_id(unique_id),
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
 * This method acts like a constructor, except that it can return NULL in two cases:
 * - if the specified type is PICKABLE_ITEM_NONE
 * or:
 * - if the specified type is PICKABLE_ITEM_RANDOM and the random type chosen is PICKABLE_ITEM_NONE
 * Furthermore, the dynamic type of the object returned might be PickableItem (for a simple item)
 * or one of its subclasses (for more complex items).
 * @param map the map
 * @param layer layer of the pickable item to create on the map (ignored for a fairy)
 * @param x x coordinate of the pickable item to create
 * @param y y coordinate of the pickable item to create
 * @param type type of pickable item to create (can be a normal item, PICKABLE_ITEM_NONE
 * or PICKABLE_ITEM_RANDOM)
 * @param unique_id unique id of the item, for certain kinds of items only (a key, a piece of heart...)
 * @param falling_height to make the item fall when it appears (ignored for a fairy)
 * @param will_disappear true to make the item disappear after an amout of time
 * @return the pickable item created, or NULL depending on the type
 */
PickableItem * PickableItem::create(Map *map, Layer layer, int x, int y, PickableItemType type,
				    int unique_id, MovementFallingHeight falling_height, bool will_disappear) {

  if (type == PICKABLE_ITEM_RANDOM) {
    // pick a type at random
    type = choose_random_type();
  }

  // don't create anything if the type is PICKABLE_ITEM_NONE
  if (type == PICKABLE_ITEM_NONE) {
    return NULL;
  }

  PickableItem *item;

  switch (type) {

    // special class for the heart
  case PICKABLE_ITEM_HEART:
    item = new PickableItemHeart(map, layer, x, y);
    break;
    
    // special class for the fairy
  case PICKABLE_ITEM_FAIRY:
    item = new PickableItemFairy(map, x, y);
    break;
    
    // other items: no special class, but directly PickableItem
  default:
    item = new PickableItem(map, layer, x, y, type, unique_id);
    break;
  }

  // set the item properties
  item->unique_id = unique_id; 
  item->falling_height = falling_height;
  item->will_disappear = will_disappear;

  // initialize the item
  item->initialize_sprites();
  item->initialize_movement();

  return item;
}

/**
 * Chooses a type of item randomly.
 * This function is called when the type of item is PICKABLE_ITEM_RANDOM.
 *
 * Some items have a low probability (fairies, big bottles of magic...)
 * while other have a high probability (hearts, rupees...).
 * The function can return PICKABLE_ITEM_NONE (this is actually the case
 * most of the time).
 * If the player is running out of hearts, magic, bombs or arrows,
 * the probability of getting what he needs is increased.
 *
 * @return a type of item (can be PICKABLE_ITEM_NONE or a normal item)
 */
PickableItemType PickableItem::choose_random_type(void) {

  PickableItemType type;

  int r = Random::get_number(1000);

  Equipment *equipment = zsdx->game->get_savegame()->get_equipment();
    
  if (r < 625) {
    // give the player nothing with probability 62.5%
    type = PICKABLE_ITEM_NONE;
  }

  else if (r < 725) { // with probability 10%
    // give the player what he needs, or nothing if he doesn't need anything
    
    // does he need hearts?
    if (equipment->needs_hearts()) {
      type = PICKABLE_ITEM_HEART;
    }

    // does he need magic?
    else if (equipment->needs_magic()) {
      type = PICKABLE_ITEM_SMALL_MAGIC;
    }

    // does he need bombs?
    else if (equipment->needs_bombs()) {
      type = PICKABLE_ITEM_BOMB_5;
    }

    // does he need arrows?
    else if (equipment->needs_arrows()) {
      type = PICKABLE_ITEM_ARROW_5;
    }

    // well, he does not need anything
    else {
      type = PICKABLE_ITEM_NONE;
    }
  }

  // the remaining 27.5% are distributed as follows:

  else if (r < 825) { type = PICKABLE_ITEM_HEART; }       // heart: 10%
  else if (r < 875) { type = PICKABLE_ITEM_RUPEE_1; }     // 1 rupee: 5%
  else if (r < 890) { type = PICKABLE_ITEM_RUPEE_5; }     // 5 rupees: 1.5%
  else if (r < 895) { type = PICKABLE_ITEM_RUPEE_20; }    // 20 rupees: 0.5%
  else if (r < 897) { type = PICKABLE_ITEM_FAIRY; }       // fairy: 0.2%
  else {

    bool has_bombs = equipment->has_inventory_item(InventoryItem::BOMBS);
    bool has_bow = equipment->has_inventory_item(InventoryItem::BOW);
    bool has_magic = equipment->get_max_magic() > 0;

    if (r < 922)      { type = has_magic ? PICKABLE_ITEM_SMALL_MAGIC : PICKABLE_ITEM_NONE; }
    else if (r < 930) { type = has_magic ? PICKABLE_ITEM_BIG_MAGIC : PICKABLE_ITEM_NONE; }
    else if (r < 950) { type = has_bombs ? PICKABLE_ITEM_BOMB_1 : PICKABLE_ITEM_NONE; }
    else if (r < 960) { type = has_bombs ? PICKABLE_ITEM_BOMB_5 : PICKABLE_ITEM_NONE; }
    else if (r < 965) { type = has_bombs ? PICKABLE_ITEM_BOMB_10 : PICKABLE_ITEM_NONE; }
    else if (r < 985) { type = has_bow ? PICKABLE_ITEM_ARROW_1 : PICKABLE_ITEM_NONE; }
    else if (r < 995) { type = has_bow ? PICKABLE_ITEM_ARROW_5 : PICKABLE_ITEM_NONE; }
    else              { type = has_bow ? PICKABLE_ITEM_ARROW_10 : PICKABLE_ITEM_NONE; }
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
  create_sprite(properties[type].sprite_animations_id);
  Sprite * item_sprite = get_last_sprite();
  item_sprite->set_current_animation(properties[type].animation_name);

  // set the origin point and the size of the entity
  set_rectangle_from_sprite();

  // initialize the sprite removal
  if (will_disappear) {
    Uint32 now = SDL_GetTicks();
    blink_date = now + 8000; // blink at 8s
    disappear_date = now + 10000; // disappear at 10s
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
  return falling_height != MOVEMENT_FALLING_NONE;
}

/**
 * This function is called by the engine when an entity overlaps the pickable item.
 * This is a redefinition of EntityDetector::entity_collision().
 * If the entity is the player, we give him the item, and the map is notified
 * to destroy it.
 * @param entity_overlapping the entity overlapping the detector
 */
void PickableItem::entity_collision(MapEntity *entity_overlapping) {

  if (entity_overlapping->is_hero()
      && SDL_GetTicks() >= (Uint32) (appear_date + 700)) {
    // wait 0.7 second before allowing Link to take the item

    give_item_to_player();
    map->get_entities()->remove_pickable_item(this);
  }
}

/**
 * Gives the item to the player.
 */
void PickableItem::give_item_to_player(void) {

  // play the sound
  Sound *sound = ResourceManager::get_sound(properties[type].sound);
  sound->play();

  // give the item
  Equipment *equipment = zsdx->game->get_savegame()->get_equipment();

  switch (type) {

  case PICKABLE_ITEM_RUPEE_1:
    equipment->add_rupees(1);
    break;

  case PICKABLE_ITEM_RUPEE_5:
    equipment->add_rupees(5);
    break;

  case PICKABLE_ITEM_RUPEE_20:
    equipment->add_rupees(20);
    break;

  case PICKABLE_ITEM_HEART:
    equipment->add_hearts(4);
    break;

  case PICKABLE_ITEM_SMALL_MAGIC:
    equipment->add_magic(6);
    break;

  case PICKABLE_ITEM_BIG_MAGIC:
    equipment->add_magic(42);
    break;

  case PICKABLE_ITEM_FAIRY:
    // done in the subclass
    break;

  case PICKABLE_ITEM_BOMB_1:
    equipment->add_bombs(1);
    break;

  case PICKABLE_ITEM_BOMB_5:
    equipment->add_bombs(5);
    break;

  case PICKABLE_ITEM_BOMB_10:
    equipment->add_bombs(10);
    break;

  case PICKABLE_ITEM_ARROW_1:
    equipment->add_arrows(1);
    break;

  case PICKABLE_ITEM_ARROW_5:
    equipment->add_arrows(5);
    break;

  case PICKABLE_ITEM_ARROW_10:
    equipment->add_arrows(10);
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
      set_blinking(true);
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
  shadow_sprite->update_current_frame();

  // check the timer
  if (will_disappear && !is_suspended()) {
    Uint32 now = SDL_GetTicks();
    Sprite *item_sprite = get_sprite(0);
    
    if (now >= blink_date && !item_sprite->is_blinking()) {
      set_blinking(true);
    }
    
    if (now >= disappear_date) {
      map->get_entities()->remove_pickable_item(this);
    }
  }
}

/**
 * Displays the pickable item on the map.
 * This is a redefinition of MapEntity::display_on_map
 * to display the shadow independently of the item movement.
 */
void PickableItem::display_on_map(Map *map) {

  // display the shadow
  map->display_sprite(shadow_sprite, shadow_x, shadow_y);

  // display the sprite
  MapEntity::display_on_map(map);
}
