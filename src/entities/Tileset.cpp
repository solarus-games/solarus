/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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
#include "entities/ParallaxTilePattern.h"
#include "entities/ScrollingTilePattern.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <iomanip>

/**
 * @brief Constructor.
 * @param id id of the tileset to create
 */
Tileset::Tileset(TilesetId id):
  id(id), max_tile_id(0),
  tiles_image(NULL), entities_image(NULL) {
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

  // compute the file name, depending on the id
  std::ostringstream oss;
  oss << "tilesets/tileset" << std::setfill('0') << std::setw(4) << id << ".dat";

  // open the tileset file
  std::string file_name = oss.str();
  std::istream &tileset_file = FileTools::data_file_open(file_name);

  // parse the tileset file
  std::string line;

  // first line: tileset general info
  if (!std::getline(tileset_file, line)) {
    Debug::die(StringConcat() << "Empty file '" << file_name << "'");
  }

  int r, g, b;

  std::istringstream iss(line);
  FileTools::read(iss, r);
  FileTools::read(iss, g);
  FileTools::read(iss, b);
  background_color = Color(r, g, b);

  // read the tile patterns
  int tile_pattern_id, animation, obstacle, default_layer;
  while (std::getline(tileset_file, line)) {

    iss.str(line);
    iss.clear();
    FileTools::read(iss, tile_pattern_id);
    FileTools::read(iss, animation);
    FileTools::read(iss, obstacle);
    FileTools::read(iss, default_layer);

    int width, height;

    if (animation != 1) { // simple tile pattern, parallax scrolling or scrolling

      int x, y;

      FileTools::read(iss, x);
      FileTools::read(iss, y);
      FileTools::read(iss, width);
      FileTools::read(iss, height);

      TilePattern *pattern;
      if (animation == 0) {
	pattern = new SimpleTilePattern(Obstacle(obstacle), x, y, width, height);
      }
      else if (animation == 2) {
	pattern = new ParallaxTilePattern(Obstacle(obstacle), x, y, width, height);
      }
      else if (animation == 3) {
        pattern = new ScrollingTilePattern(Obstacle(obstacle), x, y, width, height);
      }
      else {
	Debug::die(StringConcat() << "Unknown tile pattern animation: " << animation);
      }
      add_tile_pattern(tile_pattern_id, pattern);
    }
    else { // animated tile pattern
      int sequence, x1, y1, x2, y2, x3, y3;

      FileTools::read(iss, sequence);
      FileTools::read(iss, width);
      FileTools::read(iss, height);
      FileTools::read(iss, x1);
      FileTools::read(iss, y1);
      FileTools::read(iss, x2);
      FileTools::read(iss, y2);
      FileTools::read(iss, x3);
      FileTools::read(iss, y3);
      add_tile_pattern(tile_pattern_id, new AnimatedTilePattern(Obstacle(obstacle),
	    AnimatedTilePattern::AnimationSequence(sequence),
	    width, height, x1, y1, x2, y2, x3, y3));
    }
  }
  FileTools::data_file_close(tileset_file);

  // load the tileset images
  oss.str("");
  oss << "tilesets/tileset" << std::setfill('0') << std::setw(4) << id << "_tiles.png";
  tiles_image = new Surface(oss.str(), Surface::DIR_DATA);

  oss.str("");
  oss << "tilesets/tileset" << std::setfill('0') << std::setw(4) << id << "_entities.png";
  entities_image = new Surface(oss.str(), Surface::DIR_DATA);
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
Color & Tileset::get_background_color() {
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
Surface * Tileset::get_tiles_image() {
  return tiles_image;
}

/**
 * @brief Returns the image containing the skin-dependent dynamic entities for this tileset.
 * @return the image containing the skin-dependent dynamic entities for this tileset
 */
Surface * Tileset::get_entities_image() {
  return entities_image;
}

/**
 * @brief Returns a tile pattern from this tileset.
 * @param id id of the tile pattern to get
 * @return the tile pattern with this id
 */
TilePattern& Tileset::get_tile_pattern(int id) {

  TilePattern *tile_pattern =  tile_patterns[id];
  Debug::assert(tile_pattern != NULL, StringConcat() << "There is not tile pattern with id '" << id << "' in this tileset'");
  return *tile_pattern;
}

