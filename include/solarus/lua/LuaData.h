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
#ifndef SOLARUS_LUA_DATA_FILE_H
#define SOLARUS_LUA_DATA_FILE_H

#include "solarus/Common.h"
#include <string>

namespace Solarus {

/**
 * \brief Helper functions to load and save Lua data files.
 *
 * Data can be loaded from Lua to any object whose type provides
 * <tt>import_from_lua(lua_State* l)</tt>.
 *
 * Data can be saved as Lua for any object whose type provides
 * <tt>export_to_lua(std::ostream& out)</tt>.
 */
namespace LuaData {

template<typename Data>
bool import_from_buffer(Data& data, const std::string& buffer);

template<typename Data>
bool import_from_file(Data& data, const std::string& file_name);

template<typename Data>
bool import_from_quest_file(
    Data& data,
    const std::string& quest_file_name,
    bool language_specific = false
);

template<typename Data>
bool export_to_buffer(const Data& data, std::string& buffer);

template<typename Data>
bool export_to_file(const Data& data, const std::string& file_name);

}
}

#include "solarus/lua/LuaData.inl"

#endif

