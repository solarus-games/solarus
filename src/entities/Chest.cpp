#include "entities/Chest.h"
#include "entities/Link.h"
#include "Treasure.h"
#include "KeysEffect.h"
#include "ZSDX.h"
#include "Game.h"
#include "Savegame.h"
#include "Sprite.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "DungeonEquipment.h"
#include "Savegame.h"

/**
 * Creates a new chest with the specified treasure.
 * @param name name identifying this chest
 * @param layer layer of the chest to create on the map
 * @param x x coordinate of the chest to create
 * @param y y coordinate of the chest to create
 * @param big_chest true to make a big chest, false to make a normal chest
 * @param treasure the treasure in the chest (will be deleted automatically)
 */
Chest::Chest(string name, MapEntity::Layer layer, int x, int y, bool big_chest, Treasure *treasure):
  EntityDetector(EntityDetector::COLLISION_FACING_POINT, name, layer, x, y, 16, 16),
  big_chest(big_chest) {

  open = treasure_given = (treasure != NULL && treasure->is_found());
  if (open) {
    delete treasure;
    treasure = NULL;
  }
  this->treasure = treasure;

  initialize_sprite();
}

/**
 * Destructor.
 */
Chest::~Chest(void) {
  if (treasure != NULL && !treasure_given) {
    // delete the treasure only if the player didn't take it
    delete treasure;
  }
}

/**
 * Creates the chest sprite depending on its size and the savegame.
 */
void Chest::initialize_sprite(void) {

  // create the sprite
  create_sprite("entities/chest");  
  Sprite *sprite = get_last_sprite();

  // set its animation
  string animation = big_chest ? "big_" : "small_";
  animation += is_open() ? "open" : "closed";
  sprite->set_current_animation(animation);

  // set the entity size
  set_rectangle_from_sprite();
}

/**
 * Returns whether the player has found the treasure in this chest.
 * @return true if the chest is open
 */
bool Chest::is_open(void) {
  return open;
}

/**
 * This function is called by the engine when an entity overlaps the chest.
 * This is a redefinition of EntityDetector::entity_collision().
 * If the entity is the hero, and if he is facing north, we allow him to
 * open (or try to open) the chest.
 * @param entity_overlapping the entity overlapping the detector
 */
void Chest::entity_collision(MapEntity *entity_overlapping) {

  if (entity_overlapping->is_hero()) {

    Link *link = zsdx->game->get_link();
    KeysEffect *keys_effect = zsdx->game->get_keys_effect();
    
    if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
	&& link->get_state() == Link::FREE
	&& link->get_animation_direction() == 1
	&& !is_open()) {

      // we show the 'open' icon, even if this is a big chest and the player does not have the big key
      keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_OPEN);
    }
  }
}

/**
 * Updates the chest.
 * This function is called repeatedly by the map.
 * This is a redefinition of MapEntity::update()
 * the handle the chest opening.
 */
void Chest::update(void) {
  
  if (is_open()) {

    if (!treasure_given && SDL_GetTicks() >= treasure_date) {

      Link *link = zsdx->game->get_link();

      if (treasure->get_content() != Treasure::NONE) {
	Treasure *t = treasure;
	treasure = NULL;
	zsdx->game->give_treasure(t); // from now the game handles the treasure
      }
      else { // give nothing to the player

	// mark the chest as found in the savegame
	int savegame_index = treasure->get_savegame_index();
	if (savegame_index != -1) {
	  zsdx->game->get_savegame()->set_boolean(savegame_index, true);
	}

	// tell the player the chest is empty
	ResourceManager::get_sound("wrong")->play();
	zsdx->game->show_message("_empty_chest");

	// restore the control
	link->start_free();
	delete treasure;
	treasure = NULL;
      }

      treasure_given = true;
    }
  }
}

/**
 * This function is called when the player presses the action key
 * when Link is facing this detector, and the action icon lets him do this.
 * Link opens the chest if possible.
 */
void Chest::action_key_pressed(void) {

  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  Link *link = zsdx->game->get_link();
  DungeonEquipment *dungeon_equipment = zsdx->game->get_dungeon_equipment();

  if (link->get_state() == Link::FREE) { // don't open a chest while pushing

    if (!big_chest || dungeon_equipment->has_big_key()) {
      ResourceManager::get_sound("chest_open")->play();
      get_last_sprite()->set_current_animation(big_chest ? "big_open" : "small_open");
      open = true;
      treasure_date = SDL_GetTicks() + 300;
  
      keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
      link->freeze();
    }
    else {
      ResourceManager::get_sound("wrong")->play();
      zsdx->game->show_message("_big_key_required");
    }
  }
}

/**
 * This function is called by the map when the game is suspended or resumed.
 * This is a redefinition of MapEntity::set_suspended() to suspend the timer
 * of the chest being opened.
 * @param suspended true to suspend the entity, false to resume it
 */
void Chest::set_suspended(bool suspended) {

  MapEntity::set_suspended(suspended);

  if (!suspended) {
    // restore the timer    
    treasure_date = SDL_GetTicks() + (treasure_date - when_suspended);
  }
}
