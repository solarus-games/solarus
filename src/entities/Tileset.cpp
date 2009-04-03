#include "entities/Tileset.h"
#include "entities/SimpleTilePattern.h"
#include "entities/AnimatedTilePattern.h"
#include "FileTools.h"
#include "ResourceManager.h"
#include <iomanip>
using namespace std;

/**
 * Constructor.
 * @param id id of the tileset to create
 */
Tileset::Tileset(TilesetId id):
id(id), nb_tile_patterns(0), max_tile_id(0), tileset_image(NULL) {
  
  for (int i = 0; i < 1024; i++) {
    tile_patterns[i] = NULL;
  }
}

/**
 * Destructor.
 */
Tileset::~Tileset(void) {
  if (is_loaded()) {
    unload(); // destroy the tiles
  }
}

/**
 * Adds a tile pattern in this tileset.
 * This function is called by load().
 * @param id id of this tile pattern (1 to 1024)
 * @param tile_pattern the tile pattern to add
 */
void Tileset::add_tile_pattern(int id, TilePattern *tile) {
  tile_patterns[id - 1] = tile;
  nb_tile_patterns++;

  max_tile_id = MAX(id, max_tile_id);
}

/**
 * Loads the tileset by creating all tile patterns.
 */
void Tileset::load(void) {

  // compute the file name, depending on the id
  std::ostringstream oss;
  oss << "tilesets/tileset" << std::setfill('0') << std::setw(4) << id << ".zsd";
  string file_name = FileTools::data_file_add_prefix(oss.str());

  // open the tileset file
  std::ifstream tileset_file(file_name.c_str());

  if (!tileset_file) {
    DIE("Cannot open file '" << file_name << "'");
  }

  // parse the tileset file
  string line;

  // first line: tileset general info
  if (!std::getline(tileset_file, line)) {
    DIE("Empty file '" << file_name << "'");
  }

  int r, g, b;

  std::istringstream iss0(line);
  iss0 >> r >> g >> b;
  background_color = Color::create(r, g, b);

  // read the tile patterns
  int tile_pattern_id, is_animated, obstacle, default_layer;
  while (std::getline(tileset_file, line)) {

    std::istringstream iss(line);
    iss >> tile_pattern_id >> is_animated >> obstacle >> default_layer;

    int width, height;

    if (!is_animated) {

      int x, y;

      iss >> x >> y >> width >> height;

      add_tile_pattern(tile_pattern_id,
		       new SimpleTilePattern((MapEntity::Obstacle) obstacle,
					     x, y, width, height));
    }
    else {
      int sequence, x1, y1, x2, y2, x3, y3;

      iss >> sequence >> width >> height >> x1 >> y1 >> x2 >> y2 >> x3 >> y3;
      add_tile_pattern(tile_pattern_id,
		       new AnimatedTilePattern((MapEntity::Obstacle) obstacle,
					       (AnimatedTilePattern::AnimationSequence) sequence,
					       width, height, x1, y1, x2, y2, x3, y3));
    }
  }

  // load the tileset image
  std::ostringstream oss2;
  oss2 << "tilesets/tileset" << std::setfill('0') << std::setw(4) << id << ".png";
  tileset_image = ResourceManager::load_image(oss2.str());
  
  if (tileset_image == NULL) {
    DIE("Cannot load the image '" << file_name << "'");
  }
}

/**
 * Destroys the tile patterns and frees the memory used
 * by the tileset image.
 */
void Tileset::unload(void) {
  int i;

  for (i = 0; i < max_tile_id; i++) {
    if (tile_patterns[i] != NULL) {
      delete tile_patterns[i];
    }
  }
  nb_tile_patterns = 0;

  SDL_FreeSurface(tileset_image);
  tileset_image = NULL;
}

/**
 * Returns the background color of this tileset.
 * @return the background color
 */
Uint32 Tileset::get_background_color(void) {
  return background_color;
}

/**
 * Returns whether this tileset is loaded.
 * @return true if this tileset is loaded
 */
bool Tileset::is_loaded(void) {
  return tileset_image != NULL;
}

/**
 * Returns the tileset image.
 * @return the tileset image
 */
SDL_Surface * Tileset::get_image(void) {
  return tileset_image;
}

/**
 * Returns a tile pattern from this tileset.
 * @param id id of the tile pattern to get
 * @return the tile pattern with this id
 */
TilePattern * Tileset::get_tile_pattern(int id) {
  return tile_patterns[id - 1];
}
