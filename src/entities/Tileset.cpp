#include "entities/Tileset.h"
#include "entities/SimpleTile.h"
#include "entities/AnimatedTile.h"
#include "FileTools.h"
#include "ResourceManager.h"
#include <iomanip>

/**
 * Constructor.
 * @param id id of the tileset to create
 */
Tileset::Tileset(TilesetId id):
id(id), nb_tiles(0), tileset_image(NULL) {
  
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
 * Creates a new tile in the tileset.
 * This function is called by load().
 * @param id id of this tile (1 to 1024)
 * @param tile the tile to add
 */
void Tileset::create_tile(int id, Tile *tile) {
  tiles[id - 1] = tile;
  nb_tiles++;
}

/**
 * Loads the tileset by creating all tiles.
 */
void Tileset::load(void) {

  // compute the file name, depending on the id
  ostringstream oss(ostringstream::out);
  oss << "tilesets/tileset" << setfill('0') << setw(4) << id << ".zsd";
  string file_name = FileTools::data_file_add_prefix(oss.str());

  // open the tileset file
  ifstream tileset_file(file_name.c_str());

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

  istringstream iss0(line);
  iss0 >> r >> g >> b;
  background_color = get_color(r, g, b);

  // read the tiles
  int tile_id, is_animated, obstacle, defaultLayer;
  while (std::getline(tileset_file, line)) {

    istringstream iss(line);
    iss >> tile_id >> is_animated >> obstacle >> defaultLayer;

    int width, height;

    if (!is_animated) {
      
      int x, y;

      iss >> x >> y >> width >> height;

      create_tile(tile_id, new SimpleTile((Obstacle) obstacle, x, y, width, height));
    }
    else {
      int sequence, x1, y1, x2, y2, x3, y3;

      iss >> sequence >> width >> height >> x1 >> y1 >> x2 >> y2 >> x3 >> y3;
      
      create_tile(tile_id, new AnimatedTile((Obstacle) obstacle, (TileAnimationSequence) sequence,
					    width, height, x1, y1, x2, y2, x3, y3));
    }
  }

  // load the tileset image
  ostringstream oss2(ostringstream::out);
  oss2 << "tilesets/tileset" << setfill('0') << setw(4) << id << ".png";
  tileset_image = ResourceManager::load_image(oss2.str());
  
  if (tileset_image == NULL) {
    DIE("Cannot load the image '" << file_name << "'");
  }
}

/**
 * Destroys the tiles and frees the memory used
 * by the tileset image.
 */
void Tileset::unload(void) {
  int i;

  for (i = 0; i < nb_tiles; i++) {
    delete tiles[i];
  }
  nb_tiles = 0;

  SDL_FreeSurface(tileset_image);
}
