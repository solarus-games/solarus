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
#ifndef SOLARUS_QUEST_DATABASE_H
#define SOLARUS_QUEST_DATABASE_H

#include "solarus/core/Common.h"
#include "solarus/core/EnumInfo.h"
#include "solarus/core/ResourceType.h"
#include "solarus/lua/LuaData.h"
#include <iosfwd>
#include <map>
#include <string>

namespace Solarus {

template <>
struct SOLARUS_API EnumInfoTraits<ResourceType> {
  static const std::string pretty_name;

  static const EnumInfo<ResourceType>::names_type names;
};

/**
 * \brief Stores the list of resources and file information of a quest.
 *
 * This class stores the content of a quest database file
 * project_db.dat.
 * It does not create, remove or rename any file.
 */
class SOLARUS_API QuestDatabase : public LuaData {

  public:

    /**
     * Mapping of resource element ids to their description.
     */
    using ResourceMap = std::map<std::string, std::string>;

    /**
     * Information about a file of the quest.
     */
    struct FileInfo {
      std::string author;   /**< Author of a file or directory. */
      std::string license;  /**< License of a file or directory. */
    };

    QuestDatabase();

    void clear();

    bool resource_exists(ResourceType resource_type, const std::string& id) const;
    const ResourceMap& get_resource_elements(
        ResourceType resource_type
    ) const;
    ResourceMap& get_resource_elements(
        ResourceType resource_type
    );

    bool add(
        ResourceType resource_type,
        const std::string& id,
        const std::string& description
    );
    bool remove(
        ResourceType resource_type,
        const std::string& id
    );
    bool rename(
        ResourceType resource_type,
        const std::string& old_id,
        const std::string& new_id
    );
    std::string get_description(
        ResourceType resource_type,
        const std::string& id
    ) const;
    bool set_description(
        ResourceType resource_type,
        const std::string& id,
        const std::string& description
    );

    virtual bool import_from_lua(lua_State* l) override;
    virtual bool export_to_lua(std::ostream& out) const override;

  private:

    std::map<ResourceType, ResourceMap> resource_maps;
    std::map<std::string, FileInfo> files;

};

}

#endif

