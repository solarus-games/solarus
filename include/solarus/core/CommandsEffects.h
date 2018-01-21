/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_COMMANDS_EFFECT_H
#define SOLARUS_COMMANDS_EFFECT_H

#include "solarus/core/Common.h"
#include "solarus/core/EnumInfo.h"
#include <string>
#include <vector>

namespace Solarus {

/**
 * \brief Associate to each game key its current effect on the game.
 *
 * This class associates to each game key (the action key,
 * the sword key, the pause key, and the two equipment item keys)
 * the current effect of pressing that key.
 *
 * When the user presses one of those keys, the engine calls
 * this class to determine what should happen (e.g. swing the sword,
 * open a chest, save the game, use an item, etc.).
 * This class is also used by the HUD to display the five icons
 * representing those keys.
 *
 * TODO make a single enum for all commands
 */
class CommandsEffects {

  public:

    /**
     * \brief What happens when the user presses the Action key.
     */
    enum ActionKeyEffect {
      ACTION_KEY_NONE,     /**< nothing happens when the user presses the Action key and the action icon is not shown */
      ACTION_KEY_NEXT,     /**< display the next message in a dialog */
      ACTION_KEY_LOOK,     /**< look at something */
      ACTION_KEY_OPEN,     /**< open a door or a chest */
      ACTION_KEY_LIFT,     /**< lift an object */
      ACTION_KEY_THROW,    /**< throw an object */
      ACTION_KEY_GRAB,     /**< grab an object */
      ACTION_KEY_SPEAK,    /**< speak with an NPC */
      ACTION_KEY_SWIM      /**< swim faster */
    };

    /**
     * \brief What happens when the user presses the Attack key.
     */
    enum AttackKeyEffect {
      ATTACK_KEY_NONE,     /**< nothing happens when the user presses the Attack key */
      ATTACK_KEY_SWORD     /**< swing the sword (this is the case most of the time) */
    };

    /**
     * \brief What happens when the user presses the Pause key.
     * TODO these values are not used but they should be.
     */
    enum PauseKeyEffect {
      PAUSE_KEY_NONE,      /**< nothing happens when the user presses the Pause key */
      PAUSE_KEY_PAUSE,     /**< open the pause menu */
      PAUSE_KEY_RETURN     /**< close the pause menu */
    };

    CommandsEffects();

    // action key

    ActionKeyEffect get_action_key_effect();
    void set_action_key_effect(ActionKeyEffect action_key_effect);
    bool is_action_key_enabled();
    void set_action_key_enabled(bool enable);
    void save_action_key_effect();
    void restore_action_key_effect();
    bool is_action_key_acting_on_facing_entity();

    // sword key

    AttackKeyEffect get_sword_key_effect();
    void set_sword_key_effect(AttackKeyEffect sword_key_effect);
    bool is_sword_key_enabled();
    void set_sword_key_enabled(bool enable);
    void save_sword_key_effect();
    void restore_sword_key_effect();

    // pause key

    PauseKeyEffect get_pause_key_effect();
    void set_pause_key_effect(PauseKeyEffect pause_key_effect);
    bool is_pause_key_enabled();
    void set_pause_key_enabled(bool enable);
    void save_pause_key_effect();
    void restore_pause_key_effect();

    // item keys
    bool are_item_keys_enabled();
    void set_item_keys_enabled(bool enable);

    // all keys
    void set_all_keys_enabled(bool enable);

  private:

    ActionKeyEffect action_key_effect;       /**< current effect of the action key */
    ActionKeyEffect action_key_effect_saved; /**< a previous effect of the action key */
    bool action_key_enabled;                 /**< indicates that the action key is currently enabled */

    AttackKeyEffect sword_key_effect;         /**< current effect of the sword key */
    AttackKeyEffect sword_key_effect_saved;   /**< a previous effect of the sword key */
    bool sword_key_enabled;                  /**< indicates that the sword key is currently enabled */

    PauseKeyEffect pause_key_effect;         /**< current effect of the pause key */
    PauseKeyEffect pause_key_effect_saved;   /**< a previous effect of the pause key */
    bool pause_key_enabled;                  /**< indicates that the pause key is currently enabled */

    bool item_keys_enabled;

};

template <>
struct SOLARUS_API EnumInfoTraits<CommandsEffects::ActionKeyEffect> {
  static const std::string pretty_name;

  static const EnumInfo<CommandsEffects::ActionKeyEffect>::names_type names;
};

template <>
struct SOLARUS_API EnumInfoTraits<CommandsEffects::AttackKeyEffect> {
  static const std::string pretty_name;

  static const EnumInfo<CommandsEffects::AttackKeyEffect>::names_type names;
};

template <>
struct SOLARUS_API EnumInfoTraits<CommandsEffects::PauseKeyEffect> {
  static const std::string pretty_name;

  static const EnumInfo<CommandsEffects::PauseKeyEffect>::names_type names;
};

}

#endif

