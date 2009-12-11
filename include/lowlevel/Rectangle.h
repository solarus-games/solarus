/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ZSDX_RECTANGLE_H
#define ZSDX_RECTANGLE_H

#include "Common.h"
#include <SDL/SDL.h>

/**
 * Represents a rectangle with its top-left corner, width and height.
 * This class can also be used to represent a single point
 * (then the width and the height can be set to zero or event just left unused).
 * This class basically encapsulates a library-dependent rectangle object.
 */
class Rectangle {

  private:

    SDL_Rect rect; /**< the SDL_Rect encapsulated */

  public:

    Rectangle(void);
    Rectangle(int x, int y);
    Rectangle(int x, int y, int width, int height);
    Rectangle(const Rectangle &other);
    ~Rectangle(void);

    inline int get_x(void) const       { return rect.x; }
    inline int get_y(void) const       { return rect.y; }
    inline int get_width(void)  const  { return rect.w; }
    inline int get_height(void) const  { return rect.h; }

    inline void set_x(int x)           { rect.x = x;      }
    inline void set_y(int y)           { rect.y = y;      }
    inline void set_width(int width)   { rect.w = width;  }
    inline void set_height(int height) { rect.h = height; }

    inline void set_xy(int x, int y)             { set_x(x); set_y(y);                              }
    inline void set_xy(const Rectangle &other)   { set_xy(other.get_x(), other.get_y());            }
    inline void set_size(int width, int height)  { set_width(width); set_height(height);            }
    inline void set_size(const Rectangle &other) { set_size(other.get_width(), other.get_height()); }

    inline void add_x(int dx)          { set_x(get_x() + dx);           }
    inline void add_y(int dy)          { set_y(get_y() + dy);           }
    inline void add_width(int dw)      { set_width(get_width() + dw);   }
    inline void add_height(int dh)     { set_height(get_height() + dh); }
    inline void add_xy(int dx, int dy) { add_x(dx); add_y(dy);          }

    bool contains(int x, int y) const;
    bool overlaps(const Rectangle &other) const;
    Rectangle get_center(void);

    // for low-level classes use only
    SDL_Rect * get_internal_rect(void);
};

#endif

