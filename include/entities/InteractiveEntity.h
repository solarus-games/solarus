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
#ifndef ZSDX_INTERACTIVE_ENTITY_H
#define ZSDX_INTERACTIVE_ENTITY_H

#include "Common.h"
#include "entities/Detector.h"
#include "KeysEffect.h"

/**
 * An interactive entity is an entity that triggers a message or an event
 * when the player presses the action key while facing it.
 *
 * Note that complex entities such as chests, pots and doors are not included
 * in this class because they have additional properties (e.g. some content
 * or some variables saved).
 */
class InteractiveEntity: public Detector {

 public:

  /**
   * The different kinds of interactions.
   * They indicate what happens when the player presses the action key in front of this entity.
   */
  enum Subtype {

    CUSTOM,                /**< no predetermined behavior: just displays the message specified or calls the script */
    NON_PLAYING_CHARACTER, /**< same thing except that the script can make the engine move an NPC */
    SIGN,                  /**< a sign with a message (TODO to make it destructible,
			    * an instance of Sign is actually created) */
    WATER_FOR_BOTTLE,      /**< a place where the hero can fill a bottle with water */
  };

 private:

  Subtype subtype;
  MessageId message_to_show;

  static const KeysEffect::ActionKeyEffect action_key_effects[];
  static const int animation_directions[];

  void initialize_sprite(SpriteAnimationSetId sprite_name, int initial_direction);
  void call_script(void);

 public:

  InteractiveEntity(const std::string &name, Layer layer, int x, int y, Subtype subtype,
		    SpriteAnimationSetId sprite_name, int initial_direction, MessageId message_to_show);
  ~InteractiveEntity(void);
  static CreationFunction parse;

  EntityType get_type(void);
  bool is_displayed_in_y_order(void);

  bool is_obstacle_for(MapEntity *other);
  bool is_npc_obstacle(InteractiveEntity *npc);
  bool is_enemy_obstacle(Enemy *enemy);
  bool is_sword_ignored(void);

  void notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
  void action_key_pressed(void);
  bool interaction_with_inventory_item(InventoryItem *item);

  void update(void);
  void display_on_map(void);

  void walk(std::string path, bool loop, bool with_collisions);
  void walk_random(void);
  void jump(int direction, int length, bool with_collisions);
  void just_moved(void);
  void set_sprite_direction(int direction);
};

#endif

