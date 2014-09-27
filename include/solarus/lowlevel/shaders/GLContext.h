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
#ifndef SOLARUS_GL_CONTEXT_H
#define SOLARUS_GL_CONTEXT_H

#include "Common.h"

#if SOLARUS_HAVE_OPENGL == 1

#include <SDL.h>
#include <SDL_opengl.h>


namespace Solarus {

/**
 * \brief This class provide initialization and quit functions for the GL driver context.
 */
class GLContext {

  public:
  
    static bool initialize();
    static void quit();

  private:
  
    static SDL_GLContext gl_context;             /**< The OpenGL context. */
};

}

#endif // SOLARUS_HAVE_OPENGL

#endif
