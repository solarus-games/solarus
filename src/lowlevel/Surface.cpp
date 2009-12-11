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
#include "lowlevel/Surface.h"
#include "lowlevel/FileTools.h"

/**
 * Creates a surface with the specified size.
 * @param width the width in pixels
 * @param height the height in pixels
 */
Surface::Surface(int width, int height) {
  this->internal_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, 32, 0, 0, 0, 0);
}
    
/**
 * Creates a surface from the specified image file name.
 * @param file_name name of the image file to load, relative to the
 * data resource location.
 */
Surface::Surface(std::string file_name) {

  SDL_RWops *rw = FileTools::data_file_open_rw(file_name);
  this->internal_surface = IMG_Load_RW(rw, 0);
  FileTools::data_file_close_rw(rw);

  if (image == NULL) {
    DIE("Cannot load image '" << file_name << "'");
  }
}

/**
 * Creates a surface form the specified SDL surface.
 * This constructor must be used only by lowlevel classes that manipulate directly
 * SDL dependent surfaces.
 */
Surface::Surface(SDL_Surface *internal_surface) {
  this->internal_surface = internal_surface;
}

/**
 * Destructor.
 */
Surface::~Surface(void) {
  delete internal_surface;
}

/**
 * Returns the width of the surface.
 * @return the width in pixels
 */
int Surface::get_width(void) {
  return internal_surface->w;
}

/**
 * Returns the height of the surface.
 * @return the height in pixels
 */
int Surface::get_height(void) {
  return internal_surface->h;
}

/**
 * Sets the transparency color of this surface.
 * Pixels in that color will not be displayed.
 * @param color the transparency color to set
 */
void Surface::set_transparency_color(Color &color) {
  SDL_SetColorKey(internal_surface, SDL_SRCCOLORKEY, color.get_internal_color());
}

/**
 * Sets the opacity of this surface.
 * @param opacity the opacity (0 to 255)
 */
void Surface::set_opacity(int opacity) {

  // SDL has a special handling of the alpha value 128
  // which doesn't work well with my computer
  if (opacity == 128) {
    opacity = 127;
  }

  SDL_SetAlpha(internal_surface, SDL_SRCALPHA, opacity);
}

}

/**
 * Fills the entire surface with the specified color.
 * @param color a color
 */
void Surface::fill_with_color(Color &color) {
  SDL_FillRect(internal_surface, NULL, color.get_internal_color());
}

/**
 * Fills a rectangle of this surface with the specified color.
 * @param color a color
 * @param where the rectangle to fill
 */
void Surface::fill_with_color(Color &color, const Rectangle &where) {
  SDL_Rect where2 = where.get_internal_rect();
  SDL_FillRect(internal_surface, where2, color.get_internal_color());
}

/**
 * Blits this whole surface on another surface.
 * The top-left corner of this surface will be blitted on the other's surface top-left corner
 * @param destination the destination surface
 */
void Surface::blit(Surface *destination) {
  SDL_BlitSurface(internal_surface, NULL, destination, NULL);
}

/**
 * Blits this whole surface on a specified location of another surface.
 * @param dst the destination surface
 * @param dst_position the destination position where the current surface will be blitted on dst
 */
void Surface::blit(Surface *dst, const Rectangle &dst_position) {

  SDL_Rect dst_position2 = dst_position.get_internal_rect();
  SDL_BlitSurface(internal_surface, NULL, dst.get_internal_surface(), dst_position2);
}

/**
 * Blits a subarea of this surface on another surface.
 * The top-left corner of the source subarea will be blitted on the other's surface top-left corner
 * @param src_position the subrectangle of this surface to pick
 * @param dst the destination surface
 */
void Surface::blit(const Rectangle &src_position, Surface *dst) {

  SDL_Rect src_position2 = src_position.get_internal_rect();
  SDL_BlitSurface(internal_surface, src_position2, dst.get_internal_surface(), NULL);
}

/**
 * Blits a subarea of this surface on a specified location of another surface.
 * @param src_position the subrectangle of this surface to pick
 * @param dst the destination surface
 * @param dst_position the destination position where the current surface will be blitted on dst
 */
void Surface::blit(const Rectangle &src_position, Surface *dst, const Rectangle &dst_position) {

  SDL_Rect src_position2 = src_position.get_internal_rect();
  SDL_Rect dst_position2 = dst_position.get_internal_rect();
  SDL_BlitSurface(internal_surface, src_position2, dst.get_internal_surface(), dst_position2);
}

