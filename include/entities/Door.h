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
#ifndef SOLARUS_DOOR_H
#define SOLARUS_DOOR_H

#include "Common.h"
#include "Detector.h"

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
      CLOSED,                         /**< usual closed door */
      SMALL_KEY,                      /**< a small key is required to open the door */
      SMALL_KEY_BLOCK,                /**< a block to open with a small key (this does not look like a door) */
      BIG_KEY,                        /**< the big key is required to open the door (only in a dungeon) */
      BOSS_KEY,                       /**< the boss key is required to open the door (only in a dungeon) */
      WEAK,                           /**< a weak wall, to blow with an explosion */
      VERY_WEAK,                      /**< same as WEAK but more visible */
      WEAK_INVISIBLE                  /**< same as WEAK but no visible sign of weakness */
    };

  private:

    static const std::string animations[]; /**< sprite animation name of each subtype */

    // properties
    Subtype subtype;           /**< subtype of door */
    int savegame_variable;     /**< variable where the door state is saved */

    // state
    bool door_open;            /**< indicates that this door is open */
    bool changing;             /**< indicates that the door is being open or closed */
    bool initialized;          /**< true if update() was called at least once */

    void set_opening(void);
    void set_closing(void);

    bool requires_key(void);
    bool requires_small_key(void);
    bool requires_bomb(void);
    bool can_open(void);
    void update_dynamic_tiles(void);

  public:

    Door(const std::string &name, Layer layer, int x, int y,
	int direction, Subtype subtype, int savegame_variable);
    ~Door(void);
    static CreationFunction parse;

    EntityType get_type(void);

    bool is_obstacle_for(MapEntity *other);
    void set_suspended(bool suspended);
    void update(void);
    void display_on_map(void);
    void notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
    void action_key_pressed(void);
    SoundId get_sword_tapping_sound(void);

    bool is_open(void);
    void open(void);
    void close(void);
    void set_open(bool open);
};

#endif

