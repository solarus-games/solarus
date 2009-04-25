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
#include "MapLoader.h"
#include "Map.h"
#include "MapScript.h"
#include "FileTools.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "movements/FallingHeight.h"
#include "entities/EntityType.h"
#include "entities/Obstacle.h"
#include "entities/Layer.h"
#include "entities/MapEntities.h"
#include "entities/Tileset.h"
#include "entities/Tile.h"
#include "entities/Teletransporter.h"
#include "entities/DestinationPoint.h"
#include "entities/PickableItem.h"
#include "entities/DestructibleItem.h"
#include "entities/Chest.h"
#include "entities/JumpSensor.h"
#include "entities/Enemy.h"
#include "entities/InteractiveEntity.h"
#include "entities/Block.h"
#include "entities/DynamicTile.h"
#include "entities/Switch.h"
#include "entities/CustomObstacle.h"
#include "entities/Sensor.h"
#include "entities/CrystalSwitch.h"
#include "entities/CrystalSwitchBlock.h"
#include "entities/ShopItem.h"
#include <iomanip>
using namespace std;

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
  int id = map->get_id();

  // compute the file name, depending on the id
  std::ostringstream oss;
  oss << "maps/map" << std::setfill('0') << std::setw(4) << id << ".zsd";
  const string &file_name = FileTools::data_file_add_prefix(oss.str());

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
  for (int layer = 0; layer < LAYER_NB; layer++) {

    entities->obstacle_tiles[layer] = new Obstacle[entities->obstacle_tiles_size];
    for (int i = 0; i < entities->obstacle_tiles_size; i++) {
      entities->obstacle_tiles[layer][i] = OBSTACLE_NONE;
    }
  }

  // read the entities
  int entity_type, subtype, layer, x, y, width, height, direction;
  string entity_name;

  while (std::getline(map_file, line)) {

    std::istringstream iss(line);
    iss >> entity_type >> layer >> x >> y;

    switch (entity_type) {

    case TILE:
      {
	int tile_pattern_id;
	iss >> width >> height >> tile_pattern_id;
	TilePattern *tile_pattern = map->get_tileset()->get_tile_pattern(tile_pattern_id);
	entities->add_tile(new Tile(tile_pattern, Layer(layer), x, y, width, height));
	break;
      }

    case DESTINATION_POINT:
      {
	int is_visible;
	iss >> entity_name >> direction >> is_visible;
	entities->add_entity(new DestinationPoint(entity_name, Layer(layer), x, y, direction, (is_visible != 0)));
	break;
      }

    case TELETRANSPORTER:
      {
	int transition_style;
	MapId destination_map_id;
	string destination_point_name;
	iss >> width >> height >> entity_name >> subtype >> transition_style
	    >> destination_map_id >> destination_point_name;
	entities->add_entity(new Teletransporter(entity_name, Layer(layer), x, y, width, height,
						 Teletransporter::Subtype(subtype),
						 Transition::Style(transition_style),
						 destination_map_id, destination_point_name));
	break;
      }

    case PICKABLE_ITEM:
      {
	int savegame_variable;
	iss >> subtype >> savegame_variable;
	entities->add_entity(PickableItem::create(Layer(layer), x, y, PickableItem::Subtype(subtype),
						  savegame_variable, FALLING_NONE, false));
	break;
      }

    case DESTRUCTIBLE_ITEM:
      {
	int pickable_item_subtype, savegame_variable;
	iss >> subtype >> pickable_item_subtype >> savegame_variable;
	entities->add_entity(new DestructibleItem(Layer(layer), x, y,
						  DestructibleItem::Subtype(subtype),
						  PickableItem::Subtype(pickable_item_subtype), savegame_variable));
	break;
      }

    case CHEST:
      {
	int big_chest, treasure_content, treasure_amount, treasure_savegame_variable;
	iss >> entity_name >> big_chest >> treasure_content >> treasure_amount >> treasure_savegame_variable;
	entities->add_entity(new Chest(entity_name, Layer(layer), x, y, (big_chest != 0),
				       new Treasure(Treasure::Content(treasure_content), treasure_amount,
						    treasure_savegame_variable)));
	break;
      }

    case JUMP_SENSOR:
      {
	int jump_length;
	iss >> width >> height >> entity_name >> direction >> jump_length;
	entities->add_entity(new JumpSensor(entity_name, Layer(layer), x, y,
					     width, height, direction, jump_length));
	break;
      }

    case ENEMY:
      {
	int rank, savegame_variable, pickable_item_type, pickable_item_savegame_variable;
	iss >> entity_name >> direction >> subtype >> rank >> savegame_variable >>
	  pickable_item_type >> pickable_item_savegame_variable;
	entities->add_entity(Enemy::create(Enemy::EnemyType(subtype), Enemy::Rank(rank),
					   savegame_variable,
					   entity_name, Layer(layer), x, y, direction, 
					   PickableItem::Subtype(pickable_item_type),
					   pickable_item_savegame_variable));
	break;
      }

    case INTERACTIVE_ENTITY:
      {
	SpriteAnimationSetId sprite_name;
	MessageId message_to_show;

	iss >> entity_name >> direction >> subtype >> sprite_name >> message_to_show;
	entities->add_entity(new InteractiveEntity(entity_name, Layer(layer), x, y,
						   InteractiveEntity::Subtype(subtype),
						   sprite_name, direction, message_to_show));
	break;
      }

    case BLOCK:
      {
	int maximum_moves;

	iss >> entity_name >> direction >> subtype >> maximum_moves;
	entities->add_entity(new Block(entity_name, Layer(layer), x, y, direction,
				       Block::Subtype(subtype), maximum_moves));
	break;
      }

    case DYNAMIC_TILE:
      {
	int tile_pattern_id, enabled;

	iss >> width >> height >> entity_name >> tile_pattern_id >> enabled;
	TilePattern *tile_pattern = map->get_tileset()->get_tile_pattern(tile_pattern_id);
	entities->add_entity(new DynamicTile(entity_name, tile_pattern, Layer(layer),
					     x, y, width, height, enabled != 0));
	break;
      }

    case SWITCH:
      {
	int needs_block, disabled_when_leaving;

	iss >> entity_name >> subtype >> needs_block >> disabled_when_leaving;
	entities->add_entity(new Switch(entity_name, Layer(layer), x, y,
					Switch::Subtype(subtype), needs_block != 0, disabled_when_leaving != 0));
	break;
      }

    case CUSTOM_OBSTACLE:
      {
	int stops_hero, stops_enemies, stops_npcs, stops_blocks;

	iss >> width >> height >> entity_name >> stops_hero >> stops_enemies >> stops_npcs >> stops_blocks;
	entities->add_entity(new CustomObstacle(entity_name, Layer(layer), x, y, width, height,
						stops_hero != 0, stops_enemies != 0, stops_npcs != 0, stops_blocks != 0));
	break;
      }

    case SENSOR:
      {
	iss >> width >> height >> entity_name >> subtype;
	entities->add_entity(new Sensor(entity_name, Layer(layer), x, y, width, height, Sensor::Subtype(subtype)));
	break;
      }

    case CRYSTAL_SWITCH:
      {
	entities->add_entity(new CrystalSwitch(Layer(layer), x, y));
	break;
      }

    case CRYSTAL_SWITCH_BLOCK:
      {
	iss >> width >> height >> subtype;
	entities->add_entity(new CrystalSwitchBlock(Layer(layer), x, y, width, height,
						    CrystalSwitchBlock::Subtype(subtype)));
	break;
      }

    case SHOP_ITEM:
      {
	int treasure, amount, price;
	MessageId message_id;
	iss >> entity_name >> treasure >> amount >> price >> message_id;
	ShopItem *shop_item = new ShopItem(entity_name, Layer(layer), x, y,
					   new Treasure(Treasure::Content(treasure), amount, -1),
					   price, message_id);
	entities->add_entity(shop_item);
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
