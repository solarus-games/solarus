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
#ifndef SOLARUS_ITEM_PROPERTIES_H
#define SOLARUS_ITEM_PROPERTIES_H

#include "Common.h"

/**
 * @brief Describes the static properties of an item defined in the quest.
 *
 * These properties are loaded from the file items.dat.
 */
class ItemProperties {

  public:

    /**
     * @brief Possible sizes of the shadow of an item placed on the ground.
     */
    enum ShadowSize {
      SHADOW_NONE,
      SHADOW_SMALL,
      SHADOW_BIG
    };

  private:

    std::string name;				/**< name that identifies this item */
    int savegame_variable;			/**< savegame variable that stores the possession state */
    int nb_variants;				/**< number of variants of this item */
    int initial_variant;			/**< initial possession state of this item */
    int counter_savegame_variable;		/**< savegame variable that stores the counter associated to this item
						 * or -1 if there is no counter */
    int fixed_limit;				/**< limit of the counter associated to this item, or 0 */
    std::string item_limiting;			/**< name of an item that limits the counter of this item (or an empty string) */
    std::string item_limited;			/**< name of an item whose counter is limited by this item (or an empty string) */
    std::string item_counter_changed;		/**< name of an item whose counter is changed by this item (or an empty string) */
    int *amounts;				/**< amount to consider when limited_counter_name or changed_counter_name
						 * is defined (for each variant) */
    int *probabilities;				/**< probability of getting this item when a pickable item is choosen
						 * randomly (for each variant) */
    bool allow_assigned;			/**< indicates that this item can be assigned to an item key an then
						 * be used explicitely */
    bool disappears;				/**< when the item is dropped on the ground, indicates that it will disappear 
						 * after some time */
    bool brandish_when_picked;			/**< when the item is picked on the ground, indicates that the hero
						 * brandishes it */
    SoundId sound_when_picked;			/**< sound to play when the item is picked on the ground */
    SoundId sound_when_brandished;		/**< sound to play when the item is brandished */
    ShadowSize shadow_size;			/**< size of the shadow when the item is on the ground */

  public:

    ItemProperties(Equipment &equipment, IniFile &ini);
    ~ItemProperties();

    const std::string& get_name();
    bool is_saved();
    int get_savegame_variable();
    int get_nb_variants();
    int get_initial_variant();
    bool has_counter();
    int get_counter_savegame_variable();
    bool has_fixed_limit();
    int get_fixed_limit();
    const std::string& get_item_limiting();
    const std::string& get_item_limited();
    const std::string& get_item_counter_changed();
    int get_amount(int variant = 1);
    int get_probability(int variant = 1);
    bool can_be_assigned();
    bool can_disappear();
    bool is_brandished_when_picked();
    const SoundId& get_sound_when_picked();
    const SoundId& get_sound_when_brandished();
    ShadowSize get_shadow_size();
};

#endif

