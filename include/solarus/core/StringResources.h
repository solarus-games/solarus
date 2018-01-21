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
#ifndef SOLARUS_STRING_RESOURCES_H
#define SOLARUS_STRING_RESOURCES_H

#include "solarus/core/Common.h"
#include "solarus/lua/LuaData.h"
#include <map>

namespace Solarus {

/**
 * \brief Stores the content of a string data file.
 */
class SOLARUS_API StringResources : public LuaData {

  public:

    StringResources();

    void clear();

    const std::map<std::string, std::string>& get_strings() const;
    bool has_string(const std::string& key) const;
    const std::string& get_string(const std::string& key) const;
    std::string& get_string(const std::string& key);
    bool add_string(const std::string& key, const std::string& string);
    bool remove_string(const std::string& key);
    bool set_string_key(const std::string& old_key, const std::string& new_key);

    virtual bool import_from_lua(lua_State* l) override;
    virtual bool export_to_lua(std::ostream& out) const override;

  private:

    static int l_text(lua_State* l);

    std::map<std::string, std::string> strings;
};

}

#endif
