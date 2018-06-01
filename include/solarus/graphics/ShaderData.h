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
#ifndef SOLARUS_SHADER_DATA_H
#define SOLARUS_SHADER_DATA_H

#include "solarus/core/Common.h"
#include "solarus/lua/LuaData.h"

namespace Solarus {

/**
 * \brief Stores the content of a shader data file.
 */
class SOLARUS_API ShaderData : public LuaData {

  public:

    ShaderData();

    const std::string& get_vertex_file() const;
    void set_vertex_file(const std::string& vertex_file);

    const std::string& get_fragment_file() const;
    void set_fragment_file(const std::string& fragment_file);

    bool import_from_lua(lua_State* l) override;
    bool export_to_lua(std::ostream& out) const override;

  private:

    static int l_vertex_shader(lua_State* l);
    static int l_fragment_shader(lua_State* l);

    std::string vertex_file;    /**< Source file of the vertex shader. */
    std::string fragment_file;  /**< Source file of the fragment shader. */
};

}

#endif
