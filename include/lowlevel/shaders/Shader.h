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
#ifndef SOLARUS_SHADER_H
#define SOLARUS_SHADER_H

#include "Common.h"
#include "lowlevel/Debug.h"
#include "lua/LuaContext.h"
#include "lua/LuaTools.h"


namespace solarus {

/**
 * \brief Represents a shader for a driver and sampler-independant uses.
 */
class Shader {

  public:
  
    Shader(const std::string& shader_name);
    virtual ~Shader();

    const std::string& get_name();
    double get_window_scale();

    static void set_shading_language_version(const std::string& version);
    static const std::string& get_sampler_type();

    virtual void render(Surface& quest_surface);

  protected:

    void load(const std::string& shader_name);
    virtual void register_callback(lua_State* l);

    static std::string shading_language_version; /**< The version of the shading language. */
    static std::string sampler_type;             /**< The sampler type of the shader. */

    std::string shader_name;                     /**< The name of the shader. */
    double window_scale;                         /**< Default scale of the window when the shader is being active,
                                                  * compared to the normal quest size. */

  private:

    void load_lua_file(const std::string& path);
};

}

#endif
