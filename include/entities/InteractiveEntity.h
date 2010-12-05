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
#ifndef SOLARUS_INTERACTIVE_ENTITY_H
#define SOLARUS_INTERACTIVE_ENTITY_H

#include "Common.h"
#include "entities/Detector.h"
#include "KeysEffect.h"

/**
 * @brief An entity that triggers a message or an event
 * when the player presses the action key while facing it.
 *
 * Note that complex entities such as chests, pots and doors are not included
 * in this class because they have additional properties (e.g. some content
 * or some variables saved).
 *
 * TOOD: reimplement NPC as a subclass of InteractiveEntity, but keep using the same parse function
 */
class InteractiveEntity: public Detector {

  public:

    /**
     * @brief The different kinds of interactions.
     */
    enum Subtype {
      CUSTOM,                           /**< no predetermined behavior: just displays the message specified or calls a script */
      NON_PLAYING_CHARACTER             /**< same thing with additional specific NPC stuff */
    };

    /**
     * @brief Type of action done when an interaction occurs.
     */
    enum Behavior {
       BEHAVIOR_DIALOG,                 /**< start a dialog */
       BEHAVIOR_MAP_SCRIPT,             /**< call the map script */
       BEHAVIOR_ITEM_SCRIPT             /**< call an item script */
    };

  private:

    Subtype subtype;                    /**< subtpype of interactive entity */
    Behavior behavior;                  /**< type of action done when the player interacts with this entity */
    MessageId message_to_show;          /**< message to show when an interaction occurs, or an empty string */
    Script *script_to_call;             /**< map script or item script to call when an interarction occurs, or NULL */

    void initialize_sprite(SpriteAnimationSetId &sprite_name, int initial_direction);
    void call_script();

  public:

    InteractiveEntity(Game &game, const std::string &name, Layer layer, int x, int y, Subtype subtype,
	SpriteAnimationSetId sprite_name, int initial_direction, const std::string &behavior_string);
    ~InteractiveEntity();
    static CreationFunction parse;

    EntityType get_type();
    bool is_displayed_in_y_order();

    bool is_obstacle_for(MapEntity &other);
    bool is_hero_obstacle(Hero &hero);
    bool is_npc_obstacle(InteractiveEntity &npc);
    bool is_enemy_obstacle(Enemy &enemy);
    bool is_sword_ignored();

    void notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode);
    void action_key_pressed();
    bool interaction_with_inventory_item(InventoryItem &item);
    void notify_position_changed();

    void update();
};

#endif

