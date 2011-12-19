/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#ifndef SOLARUS_DOOR_H
#define SOLARUS_DOOR_H

#include "Common.h"
#include "entities/Detector.h"

/**
 * @brief A door that may be open or closed.
 *
 * Depending on the type of door, it can be open with a key,
 * with an explosion or by the script of the map.
 */
class Door: public Detector {

  public:

    /**
     * @brief The different kinds of doors.
     */
    enum Subtype {
      CLOSED             = 0,         /**< usual closed door */
      SMALL_KEY          = 1,         /**< a small key is required to open the door */
      SMALL_KEY_BLOCK    = 2,         /**< a block to open with a small key (this does not look like a door) */
      BIG_KEY            = 3,         /**< the big key is required to open the door (only in a dungeon) */
      BOSS_KEY           = 4,         /**< the boss key is required to open the door (only in a dungeon) */
      WEAK               = 5,         /**< a weak wall to blast with an explosion */
      VERY_WEAK          = 6,         /**< same as WEAK but more visible */
      // 7 is obsolete
      WEAK_BLOCK         = 8          /**< a block to blast with an explosion */
    };

  private:

    static const std::string animations[];             /**< sprite animation name of each subtype */
    static const MessageId key_required_message_ids[]; /**< id of the message shown for each subtype */

    // properties
    Subtype subtype;                  /**< subtype of door */
    int savegame_variable;            /**< variable where the door state is saved */

    // state
    bool door_open;                   /**< indicates that this door is open */
    bool changing;                    /**< indicates that the door is being open or closed */
    bool initialized;                 /**< true if update() was called at least once */
    uint32_t next_hint_sound_date;    /**< if the player has the ability to detect weak walls,
                                       * indicates when a hint sound is played next time */

    void set_opening();
    void set_closing();

    bool requires_key();
    bool requires_small_key();
    bool requires_explosion();
    bool can_open();
    void update_dynamic_tiles();

  public:

    Door(const std::string &name, Layer layer, int x, int y,
	int direction, Subtype subtype, int savegame_variable);
    ~Door();
    static CreationFunction parse;

    EntityType get_type();

    bool is_obstacle_for(MapEntity &other);
    void set_suspended(bool suspended);
    void update();
    void display_on_map();
    SoundId get_sword_tapping_sound();
    void action_key_pressed();
    void notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode);
    void notify_collision(MapEntity &entity, Sprite &this_sprite, Sprite &other_sprite);
    void notify_collision_with_explosion(Explosion &explosion, Sprite &sprite_overlapping);

    bool is_open();
    void open();
    void close();
    void set_open(bool open);
    bool is_changing();
};

#endif

