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

#ifndef SOLARUS_QUEST_RESOURCE_LIST_H
#define SOLARUS_QUEST_RESOURCE_LIST_H

#include "Common.h"
#include <map>
#include <string>

namespace Solarus {

/**
 * \brief This class reads the quest resource list from file project_db.dat
 * and stores them.
 */
class QuestResourceList {

  public:

    /**
     * \brief Types of existing resources in a quest.
     */
    enum ResourceType {
      RESOURCE_MAP,
      RESOURCE_TILESET,
      RESOURCE_SPRITE,
      RESOURCE_MUSIC,
      RESOURCE_SOUND,
      RESOURCE_ITEM,
      RESOURCE_ENEMY,
      RESOURCE_ENTITY,
      RESOURCE_LANGUAGE,
      RESOURCE_FONT,
      RESOURCE_NB
    };

    static void initialize();

    static bool exists(ResourceType resource_type, const std::string& id);
    static const std::map<std::string, std::string>& get_elements(ResourceType resource_type);

  private:

    QuestResourceList() = delete;

};

}

#endif

