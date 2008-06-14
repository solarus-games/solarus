#include "PickableItem.h"
#include "Sprite.h"
#include "Random.h"
#include "ZSDX.h"
#include "Game.h"
#include "Savegame.h"
#include "Equipment.h"
#include "Map.h"

// properties of each pickable item

/**
 * Animation set for each pickable item type.
 */
const SpriteAnimationsId sprite_animations_ids[] = {
  "", "entities/rupee", "entities/rupee", "entities/rupee",
  "entities/heart", "entities/magic", "entities/magic",
  "entities/fairy", "entities/bomb", "entities/bomb", "entities/bomb",
  "entities/arrow", "entities/arrow", "entities/arrow",
};

/**
 * Animation of the sprite for each pickable item type.
 */
const string animation_names[] = {
  "", "green", "blue", "red", "stopped", "small", "big",
  "normal", "1", "5", "10", "1", "5", "10", "small",
};

/**
 * Size of the shadow below each sprite:
 * false for a small shadow, true for a big shadow.
 * The bombs are the only pickable items with a big shadow.
 */
const bool big_shadows[] = {
  false, false, false, false, false, false, false,
  false, true, true, true, false, false, false, false,
};

/**
 * Height of the fall for each pickable item type
 * (except for the fairy and the heart which have
 * a special movement)
 */
/* TODO
const int fall_heights[] = {
  MOVEMENT_FALL_BIG, MOVEMENT_FALL_BIG, MOVEMENT_FALL_BIG,
  MOVEMENT_FALL_BIG
  }*/

/**
 * Creates a pickable item with the specified type.
 * The type must a normal one (not PICKABLE_ITEM_NONE or PICKABLE_ITEM_RANDOM).
 * @param layer layer of the pickable item to create on the map
 * @param x x coordinate of the pickable item to create
 * @param y y coordinate of the pickable item to create
 * @param type type of pickable item to create (must be a normal item)
 * @param falling true to make the item falling when it appears (ignored for a fairy)
 */
PickableItem::PickableItem(Layer layer, int x, int y, PickableItemType type, bool falling):
  EntityDetector("", layer, x, y, 0, 0),
  type(type), falling(falling), shadow_x(x), shadow_y(y) {

  initialize_sprites();

  // TODO  set_size, set_origin
  set_movement(NULL); // TODO sauf la fée
}

/**
 * Destructor.
 */
PickableItem::~PickableItem(void) {

}

/**
 * Creates a pickable item with the specified type.
 * This method acts like a constructor, except that it can return NULL in two cases:
 * - if the specified type is PICKABLE_ITEM_NONE
 * or:
 * - if the specified type is PICKABLE_ITEM_RANDOM and the random type chosen is PICKABLE_ITEM_NONE
 * @param layer layer of the pickable item to create on the map
 * @param x x coordinate of the pickable item to create
 * @param y y coordinate of the pickable item to create
 * @param type type of pickable item to create (can be a normal item, PICKABLE_ITEM_NONE
 * or PICKABLE_ITEM_RANDOM)
 * @param falling true to make the item falling when it appears (ignored for a fairy)
 * @return the pickable item created, or NULL depending on the type
 */
PickableItem * PickableItem::create(Layer layer, int x, int y, PickableItemType type, bool falling) {

  if (type == PICKABLE_ITEM_RANDOM) {
    // pick a type at random
    type = choose_random_type();
  }
  
  // create an object if the type is not PICKABLE_ITEM_NONE
  if (type != PICKABLE_ITEM_NONE) {
    return new PickableItem(layer, x, y, type, falling);
  }

  return NULL;
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

    // well, he does not need anything (we don't help him for the rupees)
    else {
      type = PICKABLE_ITEM_NONE;
    }
  }

  // the remaining 27.5% are distributed as follows:

  else if (r < 825) { type = PICKABLE_ITEM_HEART; }       // heart: 10%
  else if (r < 875) { type = PICKABLE_ITEM_RUPEE_1; }     // 1 rupee: 5%
  else if (r < 890) { type = PICKABLE_ITEM_RUPEE_5; }     // 5 rupees: 1.5%
  else if (r < 895) { type = PICKABLE_ITEM_RUPEE_20; }    // 20 rupees: 0.5%
  else if (r < 920) { type = PICKABLE_ITEM_SMALL_MAGIC; } // magic flask: 2.5%
  else if (r < 928) { type = PICKABLE_ITEM_BIG_MAGIC; }   // magic bottle: 0.8%
  else if (r < 930) { type = PICKABLE_ITEM_FAIRY; }       // fairy: 0.2%
  else {

    bool has_bombs = equipment->has_bombs();
    bool has_bow = equipment->has_bow();

    if (r < 950)      { type = has_bombs ? PICKABLE_ITEM_BOMB_1 : PICKABLE_ITEM_NONE; }
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

  // create the shadow (except for a fairy)
  if (type != PICKABLE_ITEM_FAIRY) {
    shadow_sprite = new Sprite("entities/shadow");

    if (big_shadows[type]) {
      shadow_sprite->set_current_animation("big");
    }
  }

  // create the sprite and set its animation
  create_sprite(sprite_animations_ids[type]);
  Sprite * item_sprite = get_last_sprite();
  item_sprite->set_current_animation(animation_names[type]);

  if (falling) {

    // special animation of the heart when falling
    if (type == PICKABLE_ITEM_HEART) {
      item_sprite->set_current_animation("falling");
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
