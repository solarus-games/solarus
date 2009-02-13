#include "MapLoader.h"
#include "Map.h"
#include "MapScript.h"
#include "FileTools.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "movements/FallingOnFloorMovement.h"
#include "entities/MapEntities.h"
#include "entities/Tileset.h"
#include "entities/Teletransporter.h"
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
  std::ostringstream oss;
  oss << "maps/map" << std::setfill('0') << std::setw(4) << id << ".zsd";
  string file_name = FileTools::data_file_add_prefix(oss.str());

  // open the map file
  std::ifstream map_file(file_name.c_str());
  if (!map_file) {
    DIE("Cannot load map '" << id << "': unable to open map file '" << file_name << "'");
  }

  // parse the map file
  string line;
  TilesetId tileset_id;

  // first line: map general info
  // syntax: width height world floor x y small_keys_variable tileset_id music_id
  if (!std::getline(map_file, line)) {
    DIE("Cannot load map '" << id << "': the file '" << file_name << "' is empty");
  }

  std::istringstream iss0(line);
  iss0 >> map->location.w >> map->location.h >> map->world >> map->floor >> map->location.x
       >> map->location.y >> map->small_keys_variable >> tileset_id >> map->music_id;

  map->width8 = map->location.w / 8;
  map->height8 = map->location.h / 8;

  map->tileset = ResourceManager::get_tileset(tileset_id);
  if (!map->tileset->is_loaded()) {
    map->tileset->load();
  }

  // create the lists of entities and initialize obstacle_tiles
  MapEntities *entities = map->get_entities();
  entities->obstacle_tiles_size = map->width8 * map->height8;
  for (int layer = 0; layer < MapEntity::LAYER_NB; layer++) {

    entities->obstacle_tiles[layer] = new MapEntity::Obstacle[entities->obstacle_tiles_size];
    for (int i = 0; i < entities->obstacle_tiles_size; i++) {
      entities->obstacle_tiles[layer][i] = MapEntity::OBSTACLE_NONE;
    }
  }

  // read the entities
  int entity_type, layer, x, y, width, height, direction;
  string entity_name;

  while (std::getline(map_file, line)) {

    std::istringstream iss(line);
    iss >> entity_type >> layer >> x >> y;

    switch (entity_type) {

    case MapEntity::TILE:
      {
	int tile_id;
	iss >> width >> height >> tile_id;
	entities->add_tile(tile_id, (MapEntity::Layer) layer, x, y, width, height);
	break;
      }

    case MapEntity::DESTINATION_POINT:
      {
	int is_visible;
	iss >> entity_name >> direction >> is_visible;
	entities->add_destination_point(entity_name, (MapEntity::Layer) layer, x, y, direction, (is_visible != 0));
	break;
      }

    case MapEntity::TELETRANSPORTER:
      {
	int subtype, transition_style;
	MapId destination_map_id;
	string destination_point_name;
	iss >> width >> height >> entity_name >> subtype >> transition_style
	    >> destination_map_id >> destination_point_name;
	entities->add_teletransporter(entity_name, (MapEntity::Layer) layer, x, y, width, height,
				      (Teletransporter::Subtype) subtype, (Transition::Style) transition_style,
				      destination_map_id, destination_point_name);
	break;
      }

    case MapEntity::PICKABLE_ITEM:
      {
	int pickable_item_type, savegame_variable;
	iss >> pickable_item_type >> savegame_variable;
	entities->add_pickable_item((MapEntity::Layer) layer, x, y, (PickableItem::ItemType) pickable_item_type,
				    savegame_variable, FallingOnFloorMovement::NONE, false);
	break;
      }

    case MapEntity::DESTRUCTIBLE_ITEM:
      {
	int destructible_item_type, pickable_item_type, savegame_variable;
	iss >> destructible_item_type >> pickable_item_type >> savegame_variable;
	entities->add_destructible_item((MapEntity::Layer) layer, x, y,
					(DestructibleItem::ItemType) destructible_item_type,
					(PickableItem::ItemType) pickable_item_type, savegame_variable);
	break;
      }

    case MapEntity::CHEST:
      {
	int big_chest, treasure_content, treasure_amount, treasure_savegame_variable;
	iss >> entity_name >> big_chest >> treasure_content >> treasure_amount >> treasure_savegame_variable;
	entities->add_chest(entity_name, (MapEntity::Layer) layer, x, y,
			    (big_chest != 0), treasure_content,
			    treasure_amount, treasure_savegame_variable);
	break;
      }

    case MapEntity::JUMP_SENSOR:
      {
	int jump_length;
	iss >> width >> height >> entity_name >> direction >> jump_length;
	entities->add_jump_sensor(entity_name, (MapEntity::Layer) layer, x, y,
				  width, height, direction, jump_length);
	break;
      }

    case MapEntity::ENEMY:
      {
	int enemy_type, rank, savegame_variable, pickable_item_type, pickable_item_savegame_variable;
	iss >> entity_name >> direction >> enemy_type >> rank >> savegame_variable >> pickable_item_type >> savegame_variable;
	entities->add_enemy(entity_name, (MapEntity::Layer) layer, x, y, direction,
			    (Enemy::EnemyType) enemy_type, (Enemy::Rank) rank, savegame_variable,
			    (PickableItem::ItemType) pickable_item_type, pickable_item_savegame_variable);
	break;
      }

    default:
      DIE("Error while loading map '" << id << "': unknown entity type '" << entity_type << "'");

    }
  }

  // load the script
  map->script = new MapScript(map);
  map->camera = new Camera(map);
}
