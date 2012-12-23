/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "entities/Tileset.h"
#include "entities/SimpleTilePattern.h"
#include "entities/AnimatedTilePattern.h"
#include "entities/SelfScrollingTilePattern.h"
#include "entities/TimeScrollingTilePattern.h"
#include "entities/ParallaxScrollingTilePattern.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lua/LuaContext.h"
#include <lua.hpp>

/**
 * @brief Lua name of each ground type.
 */
static std::string ground_names[] = {
    "traversable",
    "wall",
    "wall_top_right",
    "wall_top_left",
    "wall_bottom_left",
    "wall_bottom_right",
    "empty",
    "water_top_right",
    "water_top_left",
    "water_bottom_left",
    "water_bottom_right",
    "deep_water",
    "shallow_water",
    "hole",
    "ladder",
    "prickles",
    "lava",
    ""  // Sentinel.
};

/**
 * @brief Constructor.
 * @param id id of the tileset to create
 */
Tileset::Tileset(const std::string& id):
  id(id),
  max_tile_id(0),
  tiles_image(NULL),
  entities_image(NULL) {
}

/**
 * @brief Destructor.
 */
Tileset::~Tileset() {
  if (is_loaded()) {
    unload(); // destroy the tiles
  }
}

/**
 * @brief Returns the id of this tileset.
 * @return the tileset id
 */
const std::string& Tileset::get_id() {
  return id;
}

/**
 * @brief Adds a tile pattern to this tileset.
 *
 * This function is called by load().
 *
 * @param id id of this tile pattern (1 to 1024)
 * @param tile_pattern the tile pattern to add
 */
void Tileset::add_tile_pattern(int id, TilePattern *tile_pattern) {

  tile_patterns[id] = tile_pattern;

  max_tile_id = std::max(id, max_tile_id);
}

/**
 * @brief Loads the tileset from its file by creating all tile patterns.
 */
void Tileset::load() {

  // open the tileset file
  std::string file_name = std::string("tilesets/") + id + ".dat";

  lua_State* l = lua_open();
  size_t size;
  char* buffer;
  FileTools::data_file_open_buffer(file_name, &buffer, &size);
  luaL_loadbuffer(l, buffer, size, file_name.c_str());
  FileTools::data_file_close_buffer(buffer);

  lua_pushlightuserdata(l, this);
  lua_setfield(l, LUA_REGISTRYINDEX, "tileset");
  lua_register(l, "background_color", l_background_color);
  lua_register(l, "tile_pattern", l_tile_pattern);
  if (lua_pcall(l, 0, 0, 0) != 0) {
    Debug::die(StringConcat() << "Failed to load tileset file '"
        << file_name << "': " << lua_tostring(l, -1));
    lua_pop(l, 1);
  }

  lua_close(l);

  // load the tileset images
  file_name = std::string("tilesets/") + id + ".tiles.png";
  tiles_image = new Surface(file_name, Surface::DIR_DATA);

  file_name = std::string("tilesets/") + id + ".entities.png";
  entities_image = new Surface(file_name, Surface::DIR_DATA);
}

/**
 * @brief Destroys the tile patterns and frees the memory used
 * by the tileset image.
 */
void Tileset::unload() {

  std::map<int, TilePattern*>::iterator it;
  for (it = tile_patterns.begin(); it != tile_patterns.end(); it++) {
    delete it->second;
  }
  tile_patterns.clear();

  delete tiles_image;
  tiles_image = NULL;

  delete entities_image;
  entities_image = NULL;
}

/**
 * @brief Returns the background color of this tileset.
 * @return the background color
 */
Color& Tileset::get_background_color() {
  return background_color;
}

/**
 * @brief Returns whether this tileset is loaded.
 * @return true if this tileset is loaded
 */
bool Tileset::is_loaded() {
  return tiles_image != NULL;
}

/**
 * @brief Returns the image containing the tiles of this tileset.
 * @return the tiles image
 */
Surface& Tileset::get_tiles_image() {
  return *tiles_image;
}

/**
 * @brief Returns the image containing the skin-dependent dynamic entities for this tileset.
 * @return the image containing the skin-dependent dynamic entities for this tileset
 */
Surface& Tileset::get_entities_image() {
  return *entities_image;
}

/**
 * @brief Returns a tile pattern from this tileset.
 * @param id id of the tile pattern to get
 * @return the tile pattern with this id
 */
TilePattern& Tileset::get_tile_pattern(int id) {

  TilePattern* tile_pattern =  tile_patterns[id];
  Debug::check_assertion(tile_pattern != NULL, StringConcat() << "There is not tile pattern with id '" << id << "' in this tileset'");
  return *tile_pattern;
}

/**
 * @brief Changes the tiles images, the entities images and the background color of
 * this tileset.
 * @param other another tileset whose images and background color will be copied
 * into this tileset
 */
void Tileset::set_images(Tileset& other) {

  delete tiles_image;
  tiles_image = new Surface(other.get_tiles_image());
  delete entities_image;
  entities_image = new Surface(other.get_entities_image());
  background_color = other.get_background_color();
}

/**
 * @brief Function called by Lua to set the background color of the tileset.
 *
 * - Argument 1 (table): background color (must be an array of 3 integers).
 *
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Tileset::l_background_color(lua_State* l) {

  lua_getfield(l, LUA_REGISTRYINDEX, "tileset");
  void* p = lua_touserdata(l, -1);
  Tileset* tileset = static_cast<Tileset*>(p);
  lua_pop(l, 1);

  luaL_checktype(l, 1, LUA_TTABLE);
  lua_rawgeti(l, 1, 1);
  lua_rawgeti(l, 1, 2);
  lua_rawgeti(l, 1, 3);
  Color color(luaL_checkinteger(l, -3),
    luaL_checkinteger(l, -2),
    luaL_checkinteger(l, -1));
  lua_pop(l, 3);

  tileset->background_color = color;

  return 0;
}

/**
 * @brief Function called by Lua to add a tile pattern to the tileset.
 *
 * - Argument 1 (table): A table describing the tile pattern to create.
 *
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Tileset::l_tile_pattern(lua_State* l) {

  lua_getfield(l, LUA_REGISTRYINDEX, "tileset");
  Tileset* tileset = static_cast<Tileset*>(lua_touserdata(l, -1));
  lua_pop(l, 1);

  int id = -1, default_layer = -1, width = 0, height = 0;
  int x[] = { -1, -1, -1, -1 };
  int y[] = { -1, -1, -1, -1 };
  Obstacle ground = OBSTACLE_NONE;
  std::string scrolling;
  int i = 0, j = 0;

  // Traverse the table.
  // TODO rewrite using check_string_field() and others.
  lua_settop(l, 1);
  lua_pushnil(l);
  while (lua_next(l, 1) != 0) {

    const std::string& key = luaL_checkstring(l, 2);
    if (key == "id") {
      id = luaL_checkinteger(l, 3);
    }
    else if (key == "ground") {
      ground = LuaContext::check_enum<Obstacle>(l, 3, ground_names);
    }
    else if (key == "default_layer") {
      default_layer = luaL_checkinteger(l, 3);
    }
    else if (key == "x") {
      if (lua_isnumber(l, 3)) {
        // Single frame.
        x[0] = luaL_checkinteger(l, 3);
        i = 1;
      }
      else {
        // Multi-frame.
        lua_pushnil(l);
        while (lua_next(l, 3) != 0 && i < 4) {
          x[i] = luaL_checkinteger(l, 5);
          ++i;
          lua_pop(l, 1);
        }
      }
    }
    else if (key == "y") {
      if (lua_isnumber(l, 3)) {
        // Single frame.
        y[0] = luaL_checkinteger(l, 3);
        j = 1;
      }
      else {
        // Multi-frame.
        lua_pushnil(l);
        while (lua_next(l, 3) != 0 && j < 4) {
          y[j] = luaL_checkinteger(l, 5);
          ++j;
          lua_pop(l, 1);
        }
      }
    }
    else if (key == "width") {
      width = luaL_checkinteger(l, 3);
    }
    else if (key == "height") {
      height = luaL_checkinteger(l, 3);
    }
    else if (key == "scrolling") {
      scrolling = luaL_checkstring(l, 3);
    }
    else {
      luaL_error(l, (StringConcat() << "Unknown key '" << key << "'").c_str());
    }
    lua_pop(l, 1);
  }

  // Check data.
  if (id == -1) {
    luaL_argerror(l, 1, "Missing id for this tile pattern");
  }

  if (default_layer == -1) {
    luaL_argerror(l, 1, "Missing default layer for this tile pattern");
  }

  if (width == 0) {
    luaL_argerror(l, 1, "Missing width for this tile pattern");
  }

  if (height == 0) {
    luaL_argerror(l, 1, "Missing height for this tile pattern");
  }

  if (i != 1 && i != 3 && i != 4) {
    luaL_argerror(l, 1, "Invalid number of frames for x");
  }
  if (j != 1 && j != 3 && j != 4) {
    luaL_argerror(l, 1, "Invalid number of frames for y");
  }
  if (i != j) {
    luaL_argerror(l, 1, "The length of x and y must match");
  }

  // Create the tile pattern.
  TilePattern* tile_pattern = NULL;
  if (i == 1) {
    // Single frame.
    if (scrolling.empty()) {
      tile_pattern = new SimpleTilePattern(ground, x[0], y[0], width, height);
    }
    else if (scrolling == "parallax") {
      tile_pattern = new ParallaxScrollingTilePattern(ground, x[0], y[0], width, height);
    }
    else if (scrolling == "self") {
      tile_pattern = new SelfScrollingTilePattern(ground, x[0], y[0], width, height);
    }
  }
  else {
    // Multi-frame.
    if (scrolling == "self") {
      luaL_argerror(l, 1, "Multi-frame is not supported for self-scrolling tiles");
    }
    bool parallax = scrolling == "parallax";
    AnimatedTilePattern::AnimationSequence sequence = (i == 3) ?
        AnimatedTilePattern::ANIMATION_SEQUENCE_012 : AnimatedTilePattern::ANIMATION_SEQUENCE_0121;
    tile_pattern = new AnimatedTilePattern(ground, sequence, width, height,
        x[0], y[0], x[1], y[1], x[2], y[2], parallax);
  }

  tileset->add_tile_pattern(id, tile_pattern);

  return 0;
}
