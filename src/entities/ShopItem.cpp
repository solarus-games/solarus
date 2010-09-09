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
#include "entities/ShopItem.h"
#include "entities/Hero.h"
#include "Game.h"
#include "Map.h"
#include "MapScript.h"
#include "KeysEffect.h"
#include "Treasure.h"
#include "Sprite.h"
#include "DialogBox.h"
#include "Equipment.h"
#include "Savegame.h"
#include "lowlevel/TextSurface.h"
#include "lowlevel/FileTools.h"

/**
 * @brief Creates a new shop item with the specified treasure and price.
 * @param name the name identifying this entity
 * @param layer layer of the entity to create
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 * @param treasure the treasure that the hero can buy (will be deleted automatically)
 * @param price the treasure's price in rupees
 * @param message_id id of the message describing the item when the player watches it
 */
ShopItem::ShopItem(const std::string &name, Layer layer, int x, int y,
		   Treasure *treasure, int price, const MessageId &message_id):
  Detector(COLLISION_FACING_POINT, name, layer, x, y, 32, 32),
  treasure(treasure), price(price), message_id(message_id),
  is_looking_item(false), is_asking_question(false) {

  std::ostringstream oss;
  oss << price;

  price_digits = new TextSurface(x + 12, y + 21, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_TOP);
  price_digits->set_text(oss.str());

  rupee_icon_sprite = new Sprite("entities/rupee_icon");
}

/**
 * @brief Destructor.
 */
ShopItem::~ShopItem(void) {
  delete treasure;
  delete rupee_icon_sprite;
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
MapEntity * ShopItem::parse(Game *game, std::istream &is, Layer layer, int x, int y) {

  std::string name, treasure_name;
  int treasure_variant, treasure_savegame_variable, price;
  MessageId message_id;

  FileTools::read(is, name);
  FileTools::read(is, treasure_name);
  FileTools::read(is, treasure_variant);
  FileTools::read(is, treasure_savegame_variable);
  FileTools::read(is, price);
  FileTools::read(is, message_id);

  return create(game, name, Layer(layer), x, y,
      new Treasure(game, treasure_name, treasure_variant, treasure_savegame_variable),
      price, message_id);
}

/**
 * @brief Creates a new shop item with the specified treasure and price.
 * @param game the current game
 * @param name the name identifying this entity
 * @param layer layer of the entity to create
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 * @param treasure the treasure that the hero can buy (will be deleted automatically)
 * @param price the treasure's price in rupees
 * @param message_id id of the message describing the item when the player watches it
 */
ShopItem * ShopItem::create(Game *game, const std::string &name, Layer layer, int x, int y,
			    Treasure *treasure, int price, const MessageId &message_id) {

  // see if the item was not been already bought
  int savegame_variable = treasure->get_savegame_variable();
  if (savegame_variable != -1 && game->get_savegame()->get_boolean(savegame_variable)) {
    delete treasure;
    return NULL;
  }

  return new ShopItem(name, layer, x, y, treasure, price, message_id);
}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType ShopItem::get_type() {
  return SHOP_ITEM;
}

/**
 * @brief Returns true if this entity does not react to the sword.
 *
 * If true is returned, nothing will happen when the hero taps this entity with the sword.
 *
 * @return true if the sword is ignored
 */
bool ShopItem::is_sword_ignored(void) {
  return true;
}

/**
 * @brief Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true
 */
bool ShopItem::is_obstacle_for(MapEntity *other) {
  return true;
}

/**
 * @brief This function is called by the engine when an entity overlaps the shop item.
 *
 * If the entity is the hero, we allow him to buy the item.
 *
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void ShopItem::notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping->is_hero() && !game->is_suspended()) {

    Hero *hero = game->get_hero();
    KeysEffect *keys_effect = game->get_keys_effect();

    if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
	&& hero->is_free()) {

      // we show the 'look' icon
      keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_LOOK);
    }
  }
}

/**
 * @brief Notifies this entity that the player is interacting by pressing the action key.
 *
 * This function is called when the player presses the action key
 * when the hero is facing this detector, and the action icon lets him do this.
 * A dialog is shown to let the hero buy the item.
 */
void ShopItem::action_key_pressed(void) {

  Hero *hero = game->get_hero();
  KeysEffect *keys_effect = game->get_keys_effect();

  if (hero->is_free()
      && keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_LOOK) {

    game->get_dialog_box()->start_dialog(message_id);
    is_looking_item = true;
  }
}

/**
 * @brief Updates the entity.
 */
void ShopItem::update(void) {

  if (is_looking_item && !game->is_showing_message()) {

    // the description message has just finished
    std::string question_message_id = "_shop.question";
    game->get_dialog_box()->start_dialog(question_message_id);
    game->get_dialog_box()->set_variable(question_message_id, price);
    is_asking_question = true;
    is_looking_item = false;
  }
  else if (is_asking_question && !game->is_showing_message()) {

    // the question has just finished
    is_asking_question = false;
    int answer = game->get_dialog_box()->get_last_answer();

    if (answer == 0) {

      // the player wants to buy the item
      Equipment *equipment = game->get_equipment();

      if (equipment->get_money() < price) {
	// not enough rupees
	game->play_sound("wrong");
	game->get_dialog_box()->start_dialog("_shop.not_enough_money");
      }
      else if (treasure->is_amount_full()) {
	// the player already has the maximum amount of this item
	game->play_sound("wrong");
	game->get_dialog_box()->start_dialog("_shop.amount_full");
      }
      else {
	// give the treasure
	equipment->remove_money(price);

	int savegame_variable = treasure->get_savegame_variable();
	// TODO check memory (before r1414, a new Treasure was created here)
	game->give_treasure(treasure);
	treasure = NULL;
	if (savegame_variable != -1) {
	  remove_from_map();
	  game->get_savegame()->set_boolean(savegame_variable, true);
	}
	map->get_script()->event_shop_item_bought(get_name());
      }
    }
  }
}

/**
 * @brief Displays the entity on the map.
 */
void ShopItem::display_on_map(void) {

  Surface *map_surface = map->get_visible_surface();
  int x = get_x();
  int y = get_y();

  // display the treasure
  const Rectangle &camera_position = map->get_camera_position();
  treasure->display(map_surface, x + 8 - camera_position.get_x(), y - camera_position.get_y());

  // also display the price
  price_digits->display(map_surface);
  rupee_icon_sprite->display(map_surface, x, y + 22);
}

