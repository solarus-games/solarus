/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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

#include "solarus/Common.h"
#include "solarus/entities/Detector.h"

namespace Solarus {

/**
 * \brief A door that may be open or closed.
 *
 * It can be opened by one of the following methods depending on your choice:
 * - by pressing the action key,
 * - by pressing the action key when a specific savegame variable is set,
 * - by pressing the action key when the player has a specific equipment item,
 * - with an explosion,
 * - or only explicitly by a Lua script.
 *
 * The state of a door can be saved.
 * However, transitional states (opening and closing) are not saved:
 * opening is saved as open and closing is saved as closed.
 *
 * TODO For opening method OPENING_BY_INTERACTION_IF_ITEM, allow to interact
 * with the item command.
 */
class Door: public Detector {

  public:

    /**
     * \brief The different possible ways of opening a door.
     *
     * Note that any kind door can always be opened manually from Lua.
     */
    enum class OpeningMethod {
      NONE,                                 /**< Can only be opened from Lua. */
      BY_INTERACTION,                       /**< Can be opened by pressing the action command. */
      BY_INTERACTION_IF_SAVEGAME_VARIABLE,  /**< Can be opened by pressing the action command, provided that a savegame variable has the correct value. */
      BY_INTERACTION_IF_ITEM,               /**< Can be opened by pressing the action command, provided that the player has a specific equipment item. */
      BY_EXPLOSION                          /**< Can be opened by an explosion. */
    };

    Door(Game& game,
        const std::string& name,
        Layer layer,
        const Point& xy,
        int direction,
        const std::string& sprite_name,
        const std::string& savegame_variable
    );

    virtual EntityType get_type() const override;

    virtual bool is_obstacle_for(MapEntity& other) override;
    virtual void set_suspended(bool suspended) override;
    virtual void update() override;
    virtual void draw_on_map() override;
    virtual std::string get_sword_tapping_sound() override;
    virtual bool notify_action_command_pressed() override;
    virtual void notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode) override;
    virtual void notify_collision(MapEntity& entity, Sprite& this_sprite, Sprite& other_sprite) override;
    virtual void notify_collision_with_explosion(Explosion& explosion, Sprite& sprite_overlapping) override;

    // Properties.
    bool is_saved() const;
    const std::string& get_savegame_variable() const;
    OpeningMethod get_opening_method() const;
    void set_opening_method(OpeningMethod opening_method);
    bool is_interaction_required() const;
    const std::string& get_opening_condition() const;
    void set_opening_condition(const std::string& opening_condition);
    bool is_opening_condition_consumed() const;
    void set_opening_condition_consumed(bool opening_condition_consumed);
    const std::string& get_cannot_open_dialog_id() const;
    void set_cannot_open_dialog_id(const std::string& cannot_open_dialog_id);

    // State.
    bool can_open() const;
    bool is_open() const;
    bool is_opening() const;
    bool is_closed() const;
    bool is_closing() const;
    bool is_changing() const;
    void open();
    void close();
    void set_open(bool open);

    static const std::map<OpeningMethod, std::string> opening_method_names;

  private:

    /**
     * Possible states of a door.
     */
    enum State {
      OPEN,
      OPENING,
      CLOSED,
      CLOSING
    };

    void set_opening();
    void set_closing();
    void update_dynamic_tiles();
    void consume_opening_condition();

    // Properties.
    const std::string savegame_variable;          /**< Boolean variable that saves the door state. */
    OpeningMethod opening_method;                 /**< How this door can be opened. */
    std::string opening_condition;                /**< Condition required to open the door: a savegame variable if
                                                   * the opening mode is \c OPENING_BY_INTERACTION_IF_SAVEGAME_VARIABLE,
                                                   * or an equipment item name if the opening mode is
                                                   * \c OPENING_BY_INTERACTION_IF_ITEM. */
    bool opening_condition_consumed;              /**< Indicates that the required savegame variable
                                                   * (in the case of \c OPENING_BY_INTERACTION_IF_SAVEGAME_VARIABLE)
                                                   * or the required item
                                                   * (in the case of \c OPENING_BY_INTERACTION_IF_ITEM)
                                                   * should be consumed when opening the door. */
    std::string cannot_open_dialog_id;            /**< Dialog to show if the door cannot be opened,
                                                   * or an empty string. */

    // State.
    State state;                                  /**< State of the door: open, opening, closed or closing. */
    bool initialized;                             /**< \c true if update() was called at least once. */
    uint32_t next_hint_sound_date;                /**< If the player has the ability to detect weak walls,
                                                   * indicates when a hint sound is played next time. */

};

}

#endif

