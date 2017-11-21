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
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/SurfacePtr.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"
#include <string>

namespace Solarus {

/**
 * \brief Represents a shader for a driver and sampler-independant uses.
 */
class Shader {

  public:

    explicit Shader(const std::string& shader_id);
    virtual ~Shader();

    static void reset_time();

    const std::string& get_name();
    double get_default_window_scale();

    virtual void render(const SurfacePtr& quest_surface) const;

  protected:

    void load(const std::string& shader_name);
    virtual void register_callback(lua_State* l);

    static std::string shading_language_version; /**< The version of the shading language. */
    static std::string sampler_type;             /**< The sampler type of the shader. */
    static int display_time;                     /**< Time since the current shader is displayed (without interruptions). */

    std::string shader_name;                     /**< The name of the shader. */
    double default_window_scale;                 /**< Default scale of the window when the shader is being active,
                                                  * compared to the normal quest size. */

  private:

    void load_lua_file(const std::string& path);
};

}

#endif
