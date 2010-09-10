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
 * @brief Describes the properties of an item defined in the quest.
 *
 * These properties are loaded from the file quest.dat.
 */
class ItemProperties {

  public:

    bool can_be_assigned(void);

    bool has_amount(void);
    int get_counter_savegame_variable();
    bool can_disappear(void);
};

#endif

