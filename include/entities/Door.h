/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
 * It can be open by one of the following means depending on your choice:
 * - by pressing the action key when a specified savegame variable is set,
 * - with an explosion if the door is set to be sensible to explosions,
 * - or explicitly by a Lua script.
 *
 * The state of a door can be saved.
 */
class Door: public Detector {

  public:

    Door(Game& game,
        const std::string& name,
        Layer layer, int x, int y,
        int direction,
        const std::string& sprite_name,
        const std::string& savegame_variable);
    ~Door();

    EntityType get_type();

    bool is_obstacle_for(MapEntity& other);
    void set_suspended(bool suspended);
    void update();
    void draw_on_map();
    std::string get_sword_tapping_sound();
    void notify_action_command_pressed();
    void notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode);
    void notify_collision(MapEntity& entity, Sprite& this_sprite, Sprite& other_sprite);
    void notify_collision_with_explosion(Explosion& explosion, Sprite& sprite_overlapping);

    // Properties.
    bool is_saved() const;
    const std::string& get_savegame_variable() const;
    bool is_savegame_variable_required() const;
    const std::string& get_savegame_variable_required() const;
    void set_savegame_variable_required(const std::string& savegame_variable_to_open);
    bool is_explosion_required() const;
    void set_explosion_required(bool explosion_required);
    const std::string& get_cannot_open_dialog_id() const;
    void set_cannot_open_dialog_id(const std::string& cannot_open_dialog_id);

    // State.
    bool can_open() const;
    bool is_open() const;
    void open();
    void close();
    void set_open(bool open);
    bool is_changing() const;

    virtual const std::string& get_lua_type_name() const;

  private:

    void set_opening();
    void set_closing();
    void update_dynamic_tiles();

    // Properties.
    const std::string savegame_variable;          /**< Boolean variable that saves the door state. */
    bool explosion_required;                      /**< Indicates that an explosion can open the door. */
    std::string savegame_variable_required;       /**< Boolean or integer saved variable (if any) that allows to open the door. */
    std::string cannot_open_dialog_id;            /**< Dialog to show if the door cannot be opened
                                                   * (only if \c savegame_variable_to_open is not empty). */

    // State.
    bool door_open;                               /**< Indicates that this door is open. */
    bool changing;                                /**< Indicates that the door is being opened or closed. */
    bool initialized;                             /**< \c true if update() was called at least once. */
    uint32_t next_hint_sound_date;                /**< If the player has the ability to detect weak walls,
                                                   * indicates when a hint sound is played next time. */
};

#endif

