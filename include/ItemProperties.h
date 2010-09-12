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

    enum ShadowSize {
      SHADOW_NONE,
      SHADOW_SMALL,
      SHADOW_BIG
    };

  private:

    std::string name;				/**< name that identifies this item */
    int savegame_variable;			/**< savegame variable that stores the possession state */
    int nb_variants;				/**< number of variants of this item */
    int counter_savegame_variable;		/**< savegame variable that stores the counter associated to this item
						 * or -1 if there is no counter */
    int fixed_limit;				/**< limit of the counter associated to this item, or 0 */
    std::string limited_counter_name;		/**< name of an item whose counter is limited by this item (or an empty string) */
    std::string changed_counter_name;		/**< name of an item whose counter is changed by this item (or an empty string) */
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

    ItemProperties(IniFile *ini);
    ~ItemProperties(void);

    const std::string & get_name(void);
    bool is_saved(void);
    int get_savegame_variable(void);
    int get_nb_variants(void);
    int get_counter_savegame_variable(void);
    bool has_fixed_limit(void);
    int get_fixed_limit(void);
    const std::string & get_limited_counter_name(void);
    const std::string & get_changed_counter_name(void);
    bool has_amount(void);
    bool get_amount(int variant = 1);
    bool get_probability(int variant = 1);
    bool can_be_assigned(void);
    bool can_disappear(void);
    bool is_brandished_when_picked(void);
    const SoundId & get_sound_when_picked(void);
    const SoundId & get_sound_when_brandished(void);
    ShadowSize get_shadow_size(void);
};

#endif

