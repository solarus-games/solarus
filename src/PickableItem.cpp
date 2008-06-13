#include "PickableItem.h"
#include "Sprite.h"
#include "Random.h"

/* probas :

rien : 625/1000
objet dont le joueur manque sérieusement : 100 (ou rien s'il ne manque de rien)
coeur : 100
rubis vert : 50
petite fiole (ou rien si le joueur n'a pas d'objet magique) : 25
rubis bleu : 15
1 bombe (ou rien si le joueur n'a pas les bombes) : 20
1 flèche (ou rien si le joueur n'a pas l'arc) : 20
rubis rouge : 5
5 bombes : 10
10 bombes : 5
5 flèches : 10
10 flèches : 5
fée : 2
grande fiole : 8

*/


/**
 * Creates a pickable item with the specified type.
 * The type must a normal one (not PICKABLE_ITEM_NONE or PICKABLE_ITEM_RANDOM).
 * @param layer layer of the pickable item to create on the map
 * @param x x coordinate of the pickable item to create
 * @param y y coordinate of the pickable item to create
 * @param type type of pickable item to create (must be a normal item)
 */
PickableItem::PickableItem(Layer layer, int x, int y, PickableItemType type):
  EntityDetector("", layer, x, y, 0, 0), type(type) {

  // TODO  set_size, set_origin, set_sprite
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
 * @param type type of pickable item to create (can be a normal item, PICKABLE_ITEM_NONE or PICKABLE_ITEM_RANDOM)
 */
PickableItem * PickableItem::create(Layer layer, int x, int y, PickableItemType type) {

  if (type == PICKABLE_ITEM_RANDOM) {
    // TODO
    // int r = Random::get_number(1000);

  }
  
  if (type != PICKABLE_ITEM_NONE) {
    return new PickableItem(layer, x, y, type);
  }

  return NULL;
}
