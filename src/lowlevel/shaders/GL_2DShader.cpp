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
#include "solarus/lowlevel/shaders/GL_2DShader.h"

#if SOLARUS_HAVE_OPENGL == 1

#include "solarus/lowlevel/Surface.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Video.h"


namespace Solarus {
 
/**
 * \brief Initializes the GL 2D shader system.
 * \return \c true if GL 2D shaders are supported.
 */
bool GL_2DShader::initialize() {

  // TODO Force SDL to use 2D fetch textures, sampler and shaders if GL_ARB not supported.
  sampler_type = "sampler2D";

  return false;
}
  
/**
 * \brief Constructor.
 * \param shader_name The name of the shader to load.
 */
GL_2DShader::GL_2DShader(const std::string& shader_name): Shader(shader_name) {
}

/**
 * \brief Set up OpenGL rendering parameter.
 * This basically reset the projection matrix.
 */
void GL_2DShader::set_rendering_settings() {
}

/**
 * \brief Callback when parsing the lua file. Fill the loading shader with the result.
 * \param l The lua state.
 */
int GL_2DShader::l_shader(lua_State* l) {
}
  
/**
 * \brief Dummy method used to call the static lua callback for a specific shader implementation.
 * \param l The lua state.
 */
void GL_2DShader::register_callback(lua_State* l) {
  lua_register(l, "videomode", l_shader);
}
  
/**
 * \brief Draws the quest surface on the screen in a shader-allowed context.
 * It will perform the render using the OpenGL API directly.
 * \param quest_surface the surface to render on the screen
 */
void GL_2DShader::render(Surface& quest_surface) {
}

}

#endif // SOLARUS_HAVE_OPENGL
