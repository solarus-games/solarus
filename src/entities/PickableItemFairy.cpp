#include "entities/PickableItemFairy.h"
#include "movements/FairyMovement.h"
#include "ZSDX.h"
#include "Game.h"
#include "Equipment.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "Sprite.h"

/**
 * Creates a pickable fairy.
 * The layer is automatically set to LAYER_HIGH.
 * @param x x coordinate of the fairy to create
 * @param y y coordinate of the fairy to create
 */
PickableItemFairy::PickableItemFairy(int x, int y):
  PickableItem(LAYER_HIGH, x, y, PickableItem::FAIRY, 0) {

  set_layer_ignored(true); // detect collisions even if the fairy is on LAYER_HIGH
}

/**
 * Destructor.
 */
PickableItemFairy::~PickableItemFairy(void) {

}

/**
 * Initializes the fairy's movement.
 * This is a redefinition of PickableItem::initialize_movement()
 * to create the specific movement of a fairy.
 */
void PickableItemFairy::initialize_movement(void) {

  falling_height = FallingOnFloorMovement::NONE; // a fairy does not fall
  set_movement(new FairyMovement());
}

/**
 * Updates the fairy.
 * This function is called repeatedly by the map.
 * This is a redefinition of PickableItem::update() to update
 * the position of the shadow depending on the fairy's position.
 */
void PickableItemFairy::update(void) {

  PickableItem::update();

  shadow_x = get_x();
  shadow_y = get_y();

  FairyMovement *movement = (FairyMovement*) get_movement();
  if (movement->get_x_speed() >= 0) {
    get_sprite()->set_current_direction(0);
  }
  else {
    get_sprite()->set_current_direction(1);
  }
}

/**
 * Gives the item to the player.
 */
void PickableItemFairy::give_item_to_player(void) {

  // play the sound
  if (properties[type].sound != "") {
    Sound *sound = ResourceManager::get_sound(properties[type].sound);
    sound->play();
  }

  // give the fairy
  Equipment *equipment = zsdx->game->get_equipment();
  equipment->found_fairy();
}
