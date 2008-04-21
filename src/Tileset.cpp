/**
 * Abstract class for a tileset.
 * A tileset is an image with a set of elements (tiles)
 * one can use to compose a map.
 * See the directory images/tilesets.
 */

#include "Tileset.h"
#include "SimpleTile.h"
#include "AnimatedTile.h"
#include "FileTools.h"

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

  // read the file
  char file_name[40];
  sprintf(file_name, "tilesets/tileset%04d.zsd", (int) id);
  ifstream tileset_file(FileTools::data_file_add_prefix(file_name));

  if (!tileset_file) {
    DIE("Cannot open file '" << file_name << "'");
  }

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
  sprintf(file_name, "tilesets/tileset%04d.png", (int) id);
  tileset_image = FileTools::open_image(file_name);
  
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
