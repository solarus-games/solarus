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
#include "entities/ShopItem.h"
#include "entities/Hero.h"
#include "entities/MapEntities.h"
#include "ZSDX.h"
#include "Game.h"
#include "Map.h"
#include "MapScript.h"
#include "KeysEffect.h"
#include "Treasure.h"
#include "lowlevel/TextSurface.h"
#include "Sprite.h"
#include "DialogBox.h"
#include "Equipment.h"
#include "ResourceManager.h"
#include "lowlevel/Sound.h"
#include "Savegame.h"
#include "lowlevel/FileTools.h"

/**
 * Creates a new shop item with the specified treasure and price.
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
 * Destructor.
 */
ShopItem::~ShopItem(void) {

  delete treasure;
  delete rupee_icon_sprite;
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
MapEntity * ShopItem::parse(std::istream &is, Layer layer, int x, int y) {

  std::string name;
  int treasure, amount, savegame_variable, price;
  MessageId message_id;

  FileTools::read(is, name);
  FileTools::read(is, treasure);
  FileTools::read(is, amount);
  FileTools::read(is, savegame_variable);
  FileTools::read(is, price);
  FileTools::read(is, message_id);

  return create(name, Layer(layer), x, y, new Treasure(Treasure::Content(treasure), amount, savegame_variable),
      price, message_id);
}

/**
 * Creates a new shop item with the specified treasure and price.
 * @param name the name identifying this entity
 * @param layer layer of the entity to create
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 * @param treasure the treasure that the hero can buy (will be deleted automatically)
 * @param price the treasure's price in rupees
 * @param message_id id of the message describing the item when the player watches it
 */
ShopItem * ShopItem::create(const std::string &name, Layer layer, int x, int y,
			    Treasure *treasure, int price, const MessageId &message_id) {

  // see if the item was not been already bought
  int savegame_variable = treasure->get_savegame_variable();
  if (savegame_variable != -1 && zsdx->game->get_savegame()->get_boolean(savegame_variable)) {
    return NULL;
  }

  return new ShopItem(name, layer, x, y, treasure, price, message_id);
}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType ShopItem::get_type() {
  return SHOP_ITEM;
}

/**
 * Returns true if this entity does not react to the sword.
 * If true is returned, nothing will happen when the hero hits this entity with the sword.
 * @return true if the sword is ignored
 */
bool ShopItem::is_sword_ignored(void) {
  return true;
}

/**
 * Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true
 */
bool ShopItem::is_obstacle_for(MapEntity *other) {
  return true;
}

/**
 * This function is called by the engine when an entity overlaps the shop item.
 * If the entity is the hero, we allow him to buy the item.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void ShopItem::notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping->is_hero() && !zsdx->game->is_suspended()) {

    Hero *hero = zsdx->game->get_hero();
    KeysEffect *keys_effect = zsdx->game->get_keys_effect();

    if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
	&& hero->get_state() == Hero::FREE) {

      // we show the 'look' icon
      keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_LOOK);
    }
  }
}

/**
 * This function is called when the player presses the action key
 * when the hero is facing this detector, and the action icon lets him do this.
 * A dialog is shown to let the hero buy the item.
 */
void ShopItem::action_key_pressed(void) {

  Hero *hero = zsdx->game->get_hero();
  KeysEffect *keys_effect = zsdx->game->get_keys_effect();

  if (hero->get_state() == Hero::FREE
      && keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_LOOK) {

    zsdx->game->show_message(message_id);
    is_looking_item = true;
  }
}

/**
 * Updates the entity.
 */
void ShopItem::update(void) {

  Game *game = zsdx->game;

  if (is_looking_item && !game->is_showing_message()) {

    // the description message has just finished
    std::string question_message_id = "_shop.question";
    game->show_message(question_message_id);
    game->get_dialog_box()->set_variable(question_message_id, price);
    is_asking_question = true;
    is_looking_item = false;
  }
  else if (is_asking_question && !game->is_showing_message()) {

    // the question has just finished
    is_asking_question = false;
    int answer = game->get_dialog_last_answer();

    if (answer == 0) {

      // the player wants to buy the item
      Equipment *equipment = game->get_equipment();

      if (equipment->get_rupees() < price) {
	// not enough rupees
	ResourceManager::get_sound("wrong")->play();
	game->show_message("_shop.not_enough_money");
      }
      else if (treasure->is_amount_full()) {
	// the player already has the maximum amount of this item
	ResourceManager::get_sound("wrong")->play();
	game->show_message("_shop.amount_full");
      }
      else {
	// give the treasure
	equipment->remove_rupees(price);

	int savegame_variable = treasure->get_savegame_variable();
	game->give_treasure(new Treasure(treasure->get_content(),
					 treasure->get_amount(),
					 savegame_variable));
	if (savegame_variable != -1) {
	  map->get_entities()->remove_entity(this);
	  game->get_savegame()->set_boolean(savegame_variable, true);
	}
	map->get_script()->event_shop_item_bought(get_name());
      }
    }
  }
}

/**
 * Displays the entity on the map.
 */
void ShopItem::display_on_map(void) {

  SDL_Surface *map_surface = map->get_visible_surface();
  int x = get_x();
  int y = get_y();

  // display the treasure
  const SDL_Rect &camera_position = map->get_camera_position();
  treasure->display(map_surface, x + 8 - camera_position.x, y - camera_position.y);

  // also display the price
  price_digits->display(map_surface);
  rupee_icon_sprite->display(map_surface, x, y + 22);
}

