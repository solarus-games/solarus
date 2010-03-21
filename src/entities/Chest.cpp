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
#include "entities/Chest.h"
#include "entities/Hero.h"
#include "Treasure.h"
#include "KeysEffect.h"
#include "Game.h"
#include "DialogBox.h"
#include "Savegame.h"
#include "Sprite.h"
#include "DungeonEquipment.h"
#include "Savegame.h"
#include "Map.h"
#include "MapScript.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/System.h"

/**
 * Creates a new chest with the specified treasure.
 * @param name name identifying this chest
 * @param layer layer of the chest to create on the map
 * @param x x coordinate of the chest to create
 * @param y y coordinate of the chest to create
 * @param big_chest true to make a big chest, false to make a normal chest
 * @param treasure the treasure in the chest (will be deleted automatically)
 */
Chest::Chest(const std::string &name, Layer layer, int x, int y,
	     bool big_chest, Treasure *treasure):
  Detector(COLLISION_FACING_POINT, name, layer, x, y, 16, 16),
  big_chest(big_chest), hidden(false) {

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
 * Creates an instance from an input stream.
 * The input stream must respect the syntax of this entity type.
 * @param game the game that will contain the entity created
 * @param is an input stream
 * @param layer the layer
 * @param x x coordinate of the entity
 * @param y y coordinate of the entity
 * @return the instance created
 */
MapEntity * Chest::parse(Game *game, std::istream &is, Layer layer, int x, int y) {

  std::string name;
  int big_chest, treasure_content, treasure_amount, treasure_savegame_variable;

  FileTools::read(is, name);
  FileTools::read(is, big_chest);
  FileTools::read(is, treasure_content);
  FileTools::read(is, treasure_amount);
  FileTools::read(is, treasure_savegame_variable);

  return new Chest(name, Layer(layer), x, y, (big_chest != 0),
      new Treasure(game, Treasure::Content(treasure_content), treasure_amount, treasure_savegame_variable));
}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType Chest::get_type() {
  return CHEST;
}

/**
 * Returns whether an entity of this type should be displayed above
 * the hero and other entities when it is in front of them.
 * @return true if this entity is displayed at the same level as the hero
 */
bool Chest::is_displayed_in_y_order(void) {
  return big_chest;
}

/**
 * Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true
 */
bool Chest::is_obstacle_for(MapEntity *other) {
  return !is_hidden();
}

/**
 * Creates the chest sprite depending on its size and the savegame.
 */
void Chest::initialize_sprite(void) {

  // create the sprite
  create_sprite("entities/chest");
  Sprite *sprite = get_sprite();

  // set its animation
  std::string animation = big_chest ? "big_" : "small_";
  animation += is_open() ? "open" : "closed";
  sprite->set_current_animation(animation);

  // set the entity size
  if (big_chest) {
    set_origin(0, -8);
    set_size(32, 16);
  }
  else {
    set_size(16, 16);
  }
}

/**
 * Returns whether the chest is hidden.
 * @return true if the chest is hidden
 */
bool Chest::is_hidden(void) {
  return hidden;
}

/**
 * Sets whether the chest is hidden.
 * @param hidden true to hide the chest, false to unhide it
 */
void Chest::set_hidden(bool hidden) {
  this->hidden = hidden;

  if (!hidden) {
    Hero *hero = game->get_hero();
    if (overlaps(hero)) {
      hero->avoid_collision(this, 3);
    }
  }
}

/**
 * Returns whether the player has found the treasure in this chest.
 * @return true if the chest is open
 */
bool Chest::is_open(void) {
  return open;
}

/**
 * Sets whether the chest is open.
 * If you open the chest, its sprite is updated but the treasure is not given
 * to the player in this function.
 * If you close the chest, its sprite is updated but the treasure is not restored
 * (the chest will then be empty).
 * @param open true to open the chest, false to close it
 */
void Chest::set_open(bool open) {

  if (open != this->open) {

    this->open = open;

    if (open) {
      // open the chest
      get_sprite()->set_current_animation(big_chest ? "big_open" : "small_open");
    }
    else {
      get_sprite()->set_current_animation(big_chest ? "big_closed" : "small_closed");
      treasure_given = false;

      if (treasure == NULL) {
	treasure = new Treasure(game, Treasure::NONE, -1);
      }
    }
  }
}

/**
 * This function is called by the engine when an entity overlaps the chest.
 * This is a redefinition of Detector::notify_collision().
 * If the entity is the hero, and if he is facing north, we allow him to
 * open (or try to open) the chest.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Chest::notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (is_suspended()) {
    return;
  }

  if (entity_overlapping->is_hero() && !is_hidden()) {

    Hero *hero = (Hero*) entity_overlapping;
    KeysEffect *keys_effect = game->get_keys_effect();

    if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
	&& hero->get_state() == Hero::FREE
	&& hero->get_animation_direction() == 1
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

  if (is_open() && !suspended) {

    if (!treasure_given && System::now() >= treasure_date) {

      Hero *hero = game->get_hero();

      if (treasure->get_content() != Treasure::NONE) {
	// give a treasure to the player

	Treasure *t = treasure;
	treasure = NULL;
	game->give_treasure(t); // from now the game handles the treasure
	treasure_given = true;
      }
      else { // the chest is empty

	// mark the chest as found in the savegame
	int savegame_variable = treasure->get_savegame_variable();
	if (savegame_variable != -1) {
	  game->get_savegame()->set_boolean(savegame_variable, true);
	}

	treasure_given = true;

	// restore the control
	delete treasure;
	treasure = NULL;

	if (!map->get_script()->event_open_empty_chest(get_name())) {

	  // the script does not define any behavior:
	  // by default, we tell the player the chest is empty
	  game->play_sound("wrong");
	  game->get_dialog_box()->start_message_sequence("_empty_chest");
	  hero->unfreeze();
	}
      }
    }
  }

  MapEntity::update();
}

/**
 * This function is called when the player presses the action key
 * when the hero is facing this detector, and the action icon lets him do this.
 * The hero opens the chest if possible.
 */
void Chest::action_key_pressed(void) {

  KeysEffect *keys_effect = game->get_keys_effect();
  Hero *hero = game->get_hero();
  DungeonEquipment *dungeon_equipment = game->get_dungeon_equipment();

  if (!is_hidden() && hero->get_state() == Hero::FREE) { // don't open a chest while pushing

    if (!big_chest || dungeon_equipment->has_big_key()) {
      game->play_sound("chest_open");
      set_open(true);
      treasure_date = System::now() + 300;

      keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
      hero->freeze();
    }
    else {
      game->play_sound("wrong");
      game->get_dialog_box()->start_message_sequence("_big_key_required");
    }
  }
}

/**
 * Displays the entity on the map.
 */
void Chest::display_on_map(void) {
  if (!is_hidden()) {
    Detector::display_on_map();
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
    treasure_date = System::now() + (treasure_date - when_suspended);
  }
}
