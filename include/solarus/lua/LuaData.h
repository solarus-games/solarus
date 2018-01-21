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
#ifndef SOLARUS_LUA_DATA_FILE_H
#define SOLARUS_LUA_DATA_FILE_H

#include "solarus/core/Common.h"
#include <iosfwd>
#include <string>

struct lua_State;

namespace Solarus {

/**
 * \brief Abstract class for data the can be loaded and optionally saved as Lua.
 */
class SOLARUS_API LuaData {

  public:

    LuaData() = default;
    virtual ~LuaData() = default;

    virtual bool import_from_lua(lua_State* l) = 0;
    virtual bool export_to_lua(std::ostream& out) const;  // Optional.

    bool import_from_buffer(const std::string& buffer, const std::string& file_name);
    bool import_from_file(const std::string& file_name);
    bool import_from_quest_file(
        const std::string& quest_file_name,
        bool language_specific = false
    );

    bool export_to_buffer(std::string& buffer) const;
    bool export_to_file(const std::string& file_name) const;

    static std::string escape_string(std::string value);
    static std::string escape_multiline_string(std::string value);
    static std::string unescape_multiline_string(std::string value);

    void export_multiline_string(
        const std::string& field_name,
        const std::string& value,
        std::ostream& out) const;

};

}

#endif

