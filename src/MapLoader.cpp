#include "MapLoader.h"
#include "Map.h"
#include "FileTools.h"
#include "ResourceManager.h"
#include "entities/Tileset.h"
#include <iomanip>

/**
 * Creates a map loader.
 */
MapLoader::MapLoader(void) {

}

/**
 * Destructor.
 */
MapLoader::~MapLoader(void) {

}

/**
 * Loads a map.
 * @param map the map to load
 */
void MapLoader::load_map(Map *map) {

  // get the id of the map
  int id = (int) map->get_id();

  // compute the file name, depending on the id
  ostringstream oss(ostringstream::out);
  oss << "maps/map" << setfill('0') << setw(4) << id << ".zsd";
  string file_name = FileTools::data_file_add_prefix(oss.str());

  // open the map file
  ifstream map_file(file_name.c_str());
  if (!map_file) {
    DIE("Cannot load map '" << id << "': unable to open map file '" << file_name << "'");
  }

  // parse the map file
  string line;
  TilesetId tileset_id;

  // first line: map general info
  if (!std::getline(map_file, line)) {
    DIE("Cannot load map '" << id << "': the file '" << file_name << "' is empty");
  }

  istringstream iss0(line);
  iss0 >> map->width >> map->height >> tileset_id >> map->music_id;

  map->dungeon = 0; // TODO

  map->width8 = map->width / 8;
  map->height8 = map->height / 8;

  map->tileset = ResourceManager::get_tileset(tileset_id);
  if (!map->tileset->is_loaded()) {
    map->tileset->load();
  }

  // create the lists of entities and initialize obstacle tile
  MapEntities *entities = map->get_entities();
  entities->obstacle_tiles_size = map->width8 * map->height8;
  for (int layer = 0; layer < LAYER_NB; layer++) {

    entities->obstacle_tiles[layer] = new Obstacle[entities->obstacle_tiles_size];
    for (int i = 0; i < entities->obstacle_tiles_size; i++) {
      entities->obstacle_tiles[layer][i] = OBSTACLE_NONE;
    }
  }

  // read the entities
  int entity_type, layer, x, y, width, height, direction;
  string entity_name;
  
  while (std::getline(map_file, line)) {

    istringstream iss(line);
    iss >> entity_type >> layer >> x >> y;

    switch (entity_type) {

    case ENTITY_TILE:
      {
	int tile_id;

	iss >> width >> height >> tile_id;
	entities->add_tile(tile_id, (Layer) layer, x, y, width, height);
	break;
      }
      
    case ENTITY_ENTRANCE:
      {
	iss >> entity_name >> direction;
	entities->add_entrance(entity_name, (Layer) layer, x, y, direction);
	break;
      }
      
    case ENTITY_EXIT:
      {
	int transition_type;
	MapId destination_map_id;
	string entrance_name;
	iss >> width >> height >> entity_name >> transition_type >> destination_map_id >> entrance_name;
	entities->add_exit(entity_name, (Layer) layer, x, y, width, height,
			   (TransitionType) transition_type, destination_map_id, entrance_name);
	break;
      }

    case ENTITY_PICKABLE_ITEM:
      {
	int pickable_item_type, unique_id;
	iss >> pickable_item_type >> unique_id;
	entities->add_pickable_item((Layer) layer, x, y, (PickableItemType) pickable_item_type,
				    unique_id, MOVEMENT_FALLING_NONE, false);
	break;
      }

    case ENTITY_TRANSPORTABLE_ITEM:
      {
	int transportable_item_type, pickable_item_type, unique_id;
	iss >> transportable_item_type >> pickable_item_type >> unique_id;
	entities->add_transportable_item((Layer) layer, x, y,
					 (TransportableItemType) transportable_item_type,
					 (PickableItemType) pickable_item_type, unique_id);
	break;
      }

    default:
      DIE("Error while loading map '" << id << "': unknown entity type '" << entity_type << "'");

    }
  }
}
