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
#ifndef SOLARUS_GL_CONTEXT_H
#define SOLARUS_GL_CONTEXT_H

#include "solarus/Common.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <string>

namespace Solarus {

namespace GLContext {


  bool initialize();
  void quit();

  const std::string& get_opengl_version();
  const std::string& get_shading_language_version();

}  // namespace GLContext

}  // namespace Solarus

#endif
