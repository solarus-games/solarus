#include "MapLoader.h"
#include "Map.h"
#include "FileTools.h"
#include "Tileset.h"
#include "ZSDX.h"
#include "GameResource.h"

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

  int id = (int) map->get_id();

  char file_name[20];
  sprintf(file_name, "maps/map%04d.zsd", id);
  ifstream map_file(FileTools::data_file_add_prefix(file_name));

  if (!map_file) {
    DIE("Cannot load map '" << id << "': unable to open map file '" << file_name << "'");
  }

  string line;
  TilesetId tileset_id;

  // first line: map general info
  if (!std::getline(map_file, line)) {
    DIE("Cannot load map '" << id << "': the file '" << file_name << "' is empty");
  }

  istringstream iss0(line);
  iss0 >> map->width >> map->height >> tileset_id >> map->music_id;

  map->width8 = map->width / 8;
  map->height8 = map->height / 8;
  map->obstacle_tiles_size = map->width8 * map->height8;

  map->tileset = zsdx->game_resource->get_tileset(tileset_id);
  if (!map->tileset->is_loaded()) {
    map->tileset->load();
  }

  map->all_entities = new list<MapEntity*>();
  map->entities_to_remove = new list<MapEntity*>();
  map->entrances = new vector<MapEntrance*>();
  map->entity_detectors = new list<EntityDetector*>();

  // create the lists of entities and initialize obstacle tile
  for (int layer = 0; layer < LAYER_NB; layer++) {

    map->tiles[layer] = new vector<TileOnMap*>();
    map->sprite_entities[layer] = new list<MapEntity*>();

    map->obstacle_tiles[layer] = new Obstacle[map->obstacle_tiles_size];
    for (int i = 0; i < map->obstacle_tiles_size; i++) {
      map->obstacle_tiles[layer][i] = OBSTACLE_NONE;
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
	map->add_new_tile(tile_id, (Layer) layer, x, y, width, height);
	break;
      }
      
    case ENTITY_ENTRANCE:
      {
	iss >> entity_name >> direction;
	map->add_entrance(entity_name, (Layer) layer, x, y, direction);
	break;
      }
      
    case ENTITY_EXIT:
      {
	int transition_type;
	MapId destination_map_id;
	string entrance_name;
	iss >> width >> height >> entity_name >> transition_type >> destination_map_id >> entrance_name;
	map->add_exit(entity_name, (Layer) layer, x, y, width, height,
		 (TransitionType) transition_type, destination_map_id, entrance_name);
	break;
      }

    case ENTITY_PICKABLE_ITEM:
      {
	int pickable_item_type;
	iss >> pickable_item_type;
	map->add_pickable_item((Layer) layer, x, y, (PickableItemType) pickable_item_type, true); // TODO false
	break;
      }

    case ENTITY_TRANSPORTABLE_ITEM:
      {

	break;
      }

    default:
      DIE("Error while loading map '" << id << "': unknown entity type '" << entity_type << "'");

    }
  }
}
