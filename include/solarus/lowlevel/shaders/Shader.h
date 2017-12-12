/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_SHADER_H
#define SOLARUS_SHADER_H

#include "solarus/Common.h"
#include "solarus/lowlevel/shaders/ShaderData.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/SurfacePtr.h"
#include "solarus/lua/ExportableToLua.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"
#include <string>


#define SOLARUS_HAVE_OPENGL // Temporary define. To determine at configure part

namespace Solarus {

/**
 * \brief Represents a shader for a driver and sampler-independant uses.
 */
class Shader : public ExportableToLua {

  public:

    explicit Shader(const std::string& shader_id);
    virtual ~Shader();

    const std::string& get_id() const;
    const ShaderData& get_data() const;

    virtual void render(const SurfacePtr& quest_surface) const;

    const std::string& get_lua_type_name() const override;

  protected:

    void set_data(const ShaderData& data);
    virtual void load();

  private:

    const std::string shader_id;  /**< The id of the shader (filename without extension). */
    ShaderData data;              /**< The loaded shader data file. */
};

}

#endif
