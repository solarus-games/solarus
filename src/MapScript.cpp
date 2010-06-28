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
#include "MapScript.h"
#include "Map.h"
#include "Game.h"
#include "Sprite.h"
#include "InventoryItem.h"
#include "entities/EntityType.h"
#include "entities/Detector.h"
#include "entities/MapEntities.h"
#include "entities/InteractiveEntity.h"
#include "entities/Hero.h"
#include "entities/Chest.h"
#include "entities/Block.h"
#include "entities/DynamicTile.h"
#include "entities/Switch.h"
#include "entities/Door.h"
#include "entities/Sensor.h"
#include "entities/Enemy.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Music.h"
#include "lowlevel/Rectangle.h"
#include <lua.hpp>
#include <iomanip>

/**
 * Creates the script of a map.
 * @param map the map
 */
MapScript::MapScript(Map *map):
  Script(map->get_game()), map(map) {

}

/**
 * Desctructor.
 */
MapScript::~MapScript(void) {

}

/**
 * Tells the Lua context what C++ functions it can call.
 */
void MapScript::register_available_functions(void) {

  // functions global to all types of scripts
  Script::register_available_functions();

  // functions specific to map scripts
  lua_register(context, "hero_set_map", l_hero_set_map);
  lua_register(context, "hero_set_direction", l_hero_set_direction);
  lua_register(context, "hero_align_on_sensor", l_hero_align_on_sensor);
  lua_register(context, "hero_walk", l_hero_walk);
  lua_register(context, "hero_jump", l_hero_jump);
  lua_register(context, "hero_start_victory_sequence", l_hero_start_victory_sequence);
  lua_register(context, "hero_set_visible", l_hero_set_visible);
  lua_register(context, "npc_get_position", l_npc_get_position);
  lua_register(context, "npc_set_position", l_npc_set_position);
  lua_register(context, "npc_walk", l_npc_walk);
  lua_register(context, "npc_random_walk", l_npc_random_walk);
  lua_register(context, "npc_jump", l_npc_jump);
  lua_register(context, "npc_set_animation", l_npc_set_animation);
  lua_register(context, "npc_set_animation_ignore_suspend", l_npc_set_animation_ignore_suspend);
  lua_register(context, "npc_set_direction", l_npc_set_direction);
  lua_register(context, "npc_remove", l_npc_remove);
  lua_register(context, "chest_set_open", l_chest_set_open);
  lua_register(context, "chest_set_hidden", l_chest_set_hidden);
  lua_register(context, "chest_is_hidden", l_chest_is_hidden);
  lua_register(context, "tile_set_enabled", l_tile_set_enabled);
  lua_register(context, "tiles_set_enabled", l_tiles_set_enabled);
  lua_register(context, "tile_is_enabled", l_tile_is_enabled);
  lua_register(context, "reset_block", l_reset_block);
  lua_register(context, "reset_blocks", l_reset_blocks);
  lua_register(context, "interactive_entity_get_animation", l_interactive_entity_get_animation);
  lua_register(context, "interactive_entity_get_animation_delay", l_interactive_entity_get_animation_delay);
  lua_register(context, "interactive_entity_get_animation_frame", l_interactive_entity_get_animation_frame);
  lua_register(context, "interactive_entity_get_direction", l_interactive_entity_get_direction);
  lua_register(context, "interactive_entity_is_animation_paused", l_interactive_entity_is_animation_paused);
  lua_register(context, "interactive_entity_set_animation", l_interactive_entity_set_animation);
  lua_register(context, "interactive_entity_set_animation_delay", l_interactive_entity_set_animation_delay);
  lua_register(context, "interactive_entity_set_animation_frame", l_interactive_entity_set_animation_frame);
  lua_register(context, "interactive_entity_set_direction", l_interactive_entity_set_direction);
  lua_register(context, "interactive_entity_set_animation_paused", l_interactive_entity_set_animation_paused);
  lua_register(context, "interactive_entity_set_animation_ignore_suspend", l_interactive_entity_set_animation_ignore_suspend);
  lua_register(context, "interactive_entity_fade", l_interactive_entity_fade);
  lua_register(context, "interactive_entity_remove", l_interactive_entity_remove);
  lua_register(context, "shop_item_remove", l_shop_item_remove);
  lua_register(context, "switch_is_enabled", l_switch_is_enabled);
  lua_register(context, "switch_set_enabled", l_switch_set_enabled);
  lua_register(context, "switch_set_locked", l_switch_set_locked);
  lua_register(context, "enemy_is_dead", l_enemy_is_dead);
  lua_register(context, "enemies_are_dead", l_enemies_are_dead);
  lua_register(context, "enemy_set_enabled", l_enemy_set_enabled);
  lua_register(context, "boss_start_battle", l_boss_start_battle);
  lua_register(context, "boss_end_battle", l_boss_end_battle);
  lua_register(context, "miniboss_start_battle", l_miniboss_start_battle);
  lua_register(context, "miniboss_end_battle", l_miniboss_end_battle);
  lua_register(context, "sensor_remove", l_sensor_remove);
  lua_register(context, "door_open", l_door_open);
  lua_register(context, "door_close", l_door_close);
  lua_register(context, "door_is_open", l_door_is_open);
  lua_register(context, "door_set_open", l_door_set_open);
}

/**
 * Loads the script and starts it.
 * This function is called when the map starts.
 * @param destination_point_name name of the destination point where the hero is
 */
void MapScript::start(const std::string &destination_point_name) {

  this->hero = map->get_entities()->get_hero();

  // compute the file name, depending on the id of the map
  int id = (int) map->get_id();
  std::ostringstream oss;
  oss << "maps/map" << std::setfill('0') << std::setw(4) << id;

  // load the script
  load(oss.str());

  // notify the map
  event_map_started(destination_point_name);
}

/**
 * In any C++ function called by the Lua script (i.e. a function prefixed by "l_"),
 * the first instruction calls this function.
 * It checks the number of arguments provided to the C++ function called by the Lua script
 * and retrieves the current MapScript object.
 * @param context the Lua context
 * @param nb_arguments the number of arguments to check (if it is incorrect, the program stops)
 * @param map_script if not NULL, a pointer to the MapScript object will be copied there so that the static C++ function
 * called by the Lua script can access it
 */

void MapScript::called_by_script(lua_State *context, int nb_arguments, MapScript **map_script) {

  Script *script;
  Script::called_by_script(context, nb_arguments, &script);
  *map_script = (MapScript*) script;
}

// functions that can be called by the Lua script

/**
 * Sends the hero to a map.
 * Argument 1 (int): id of the destination map (can be the same one)
 * Argument 2 (string): name of the destination point on that map
 * Argument 3 (int): type of transition to play
 */
int MapScript::l_hero_set_map(lua_State *l) {

  MapScript *script;
  called_by_script(l, 3, &script);

  MapId map_id = lua_tointeger(l, 1);
  const std::string &destination_point_name = lua_tostring(l, 2);
  Transition::Style transition_style = Transition::Style(lua_tointeger(l, 3));

  script->game->set_current_map(map_id, destination_point_name, transition_style);

  return 0;
}

/**
 * Sets the direction of the hero's sprite.
 * Argument 1 (integer): the direction between 0 and 3
 */
int MapScript::l_hero_set_direction(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  int direction = lua_tointeger(l, 1);

  script->hero->set_animation_direction(direction);

  return 0;
}

/**
 * Places the hero on the exact position of a sensor's name.
 * Argument 1 (string): name of the sensor
 */
int MapScript::l_hero_align_on_sensor(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map->get_entities();
  Sensor *sensor = (Sensor*) entities->get_entity(SENSOR, name);
  script->hero->set_xy(sensor->get_xy());

  return 0;
}

/**
 * Makes the hero walk with respect to a path.
 * Argument 1 (string): the path (each character is a direction between '0' and '7')
 * Argument 2 (boolean): true to make the movement loop
 * Argument 3 (boolean): true to make the movement sensible to obstacles
 */
int MapScript::l_hero_walk(lua_State *l) {

  called_by_script(l, 3, NULL);

  /* TODO
  const std::string &path = lua_tostring(l, 1);
  bool loop = lua_toboolean(l, 2) != 0;
  bool with_collisions = lua_toboolean(l, 3) != 0;

  script->hero->walk(path, loop, with_collisions);
  */

  return 0;
}

/**
 * Makes the hero jump into a direction.
 * Argument 1 (integer): the jump direction, between 0 and 7
 * Argument 2 (integer): the jump length in pixels
 * Argument 3 (boolean): true to enable the collisions
 */
int MapScript::l_hero_jump(lua_State *l) {

  MapScript *script;
  called_by_script(l, 3, &script);

  int direction = lua_tointeger(l, 1);
  int length = lua_tointeger(l, 2);
  bool with_collisions = lua_toboolean(l, 3) != 0;

  script->hero->start_jumping(direction, length, with_collisions, false);

  return 0;
}

/**
 * Makes the hero brandish his sword meaning a victory
 * and plays the corresponding sound.
 */
int MapScript::l_hero_start_victory_sequence(lua_State *l) {
  
  MapScript *script;
  called_by_script(l, 0, &script);

  script->hero->start_victory();

  return 0;
}

/**
 * Hides or shows the hero.
 * Hiding the hero does not disable its movements, so when using this function
 * you will usually also need to freeze the hero.
 * Argument 1 (boolean): true to make the hero visible
 */
int MapScript::l_hero_set_visible(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  bool visible = lua_toboolean(l, 1) != 0;

  script->hero->set_visible(visible);

  return 0;
}

/**
 * Returns the position of an NPC.
 * Argument 1 (string): name of the NPC
 * Return value 1 (integer): x position
 * Return value 2 (integer): y position
 */
int MapScript::l_npc_get_position(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map->get_entities();
  InteractiveEntity *npc = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  const Rectangle &coordinates = npc->get_xy();

  lua_pushinteger(l, coordinates.get_x());
  lua_pushinteger(l, coordinates.get_y());

  return 2;
}

/**
 * Sets the position of an NPC.
 * Argument 1 (string): name of the NPC
 * Argument 2 (integer): x position to set
 * Argument 3 (integer): y position to set
 */
int MapScript::l_npc_set_position(lua_State *l) {

  MapScript *script;
  called_by_script(l, 3, &script);

  const std::string &name = lua_tostring(l, 1);
  int x = lua_tointeger(l, 2);
  int y = lua_tointeger(l, 3);

  MapEntities *entities = script->map->get_entities();
  InteractiveEntity *npc = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  npc->set_xy(x, y);

  return 0;
}

/**
 * Makes an NPC walk with respect to a path.
 * Argument 1 (string): name of the NPC to make move
 * Argument 2 (string): the path (each character is a direction between '0' and '7'
 * Argument 3 (boolean): true to make the movement loop
 * Argument 4 (boolean): true to enable the collisions
 */
int MapScript::l_npc_walk(lua_State *l) {

  MapScript *script;
  called_by_script(l, 4, &script);

  const std::string &name = lua_tostring(l, 1);
  const std::string &path = lua_tostring(l, 2);
  bool loop = lua_toboolean(l, 3) != 0;
  bool with_collisions = lua_toboolean(l, 4) != 0;

  MapEntities *entities = script->map->get_entities();
  InteractiveEntity *npc = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  npc->walk(path, loop, with_collisions);

  return 0;
}

/**
 * Makes an NPC walk randomly.
 * Argument 1 (string): name of the NPC to make move
 */
int MapScript::l_npc_random_walk(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map->get_entities();
  InteractiveEntity *npc = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  npc->walk_random();

  return 0;
}

/**
 * Makes an NPC jump into a direction.
 * The NPC's sprite must have an animation "jumping".
 * Argument 1 (string): name of the NPC to make move
 * Argument 2 (integer): the jump direction, between 0 and 7
 * Argument 3 (integer): the jump length in pixels
 * Argument 4 (boolean): true to enable the collisions
 */
int MapScript::l_npc_jump(lua_State *l) {

  MapScript *script;
  called_by_script(l, 4, &script);

  const std::string &name = lua_tostring(l, 1);
  int direction = lua_tointeger(l, 2);
  int length = lua_tointeger(l, 3);
  bool with_collisions = lua_toboolean(l, 4) != 0;

  MapEntities *entities = script->map->get_entities();
  InteractiveEntity *npc = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  npc->jump(direction, length, with_collisions);

  return 0;
}

/**
 * Sets the animation of an NPC's sprite.
 * Argument 1 (string): name of the NPC
 * Argument 2 (string): name of the animation to set
 */
int MapScript::l_npc_set_animation(lua_State *l) {
  return l_interactive_entity_set_animation(l);
}

/**
 * Sets whether the animation of an NPC should continue even when the game is suspended.
 * Argument 1 (string): name of the NPC
 * Argument 2 (boolean): true to ignore when the game is suspended
 */
int MapScript::l_npc_set_animation_ignore_suspend(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  bool ignore_suspend = lua_toboolean(l, 2) != 0;

  MapEntities *entities = script->map->get_entities();
  MapEntity *npc = entities->get_entity(INTERACTIVE_ENTITY, name);
  npc->set_animation_ignore_suspend(ignore_suspend);

  return 0;
}

/**
 * Sets the direction of an NPC's sprite.
 * Argument 1 (string): name of the NPC
 * Argument 2 (integer): the sprite's direction between 0 and 3
 */
int MapScript::l_npc_set_direction(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  int direction = lua_tointeger(l, 2);

  if (direction < 0 || direction >= 4) {
    DIE("Invalid NPC direction: " << direction);
  }

  MapEntities *entities = script->map->get_entities();
  InteractiveEntity *npc = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  npc->set_sprite_direction(direction);

  return 0;
}

/**
 * Removes an NPC from the map.
 * Argument 1 (string): name of the NPC
 */
int MapScript::l_npc_remove(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map->get_entities();
  entities->remove_entity(INTERACTIVE_ENTITY, name);

  return 0;
}

/**
 * Sets the chest open or closed.
 * Only the chest sprite is affected (use give_treasure to give a treasure to the player).
 * This function is useful for chests whose content is managed by the script.
 * Argument 1 (string): name of the chest
 * Argument 2 (boolean): true to make the chest open, false to make it closed
 */
int MapScript::l_chest_set_open(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  bool open = lua_toboolean(l, 2) != 0;

  MapEntities *entities = script->map->get_entities();
  Chest *chest = (Chest*) entities->get_entity(CHEST, name);
  chest->set_open(open);

  return 0;
}

/**
 * Hides or unhides a chest.
 * If the chest is already open, hiding it has not effect.
 * Argument 1 (string): name of the chest
 * Argument 2 (boolean): true to make the chest hidden, false to make it appear
 */
int MapScript::l_chest_set_hidden(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  bool hidden = lua_toboolean(l, 2) != 0;

  MapEntities *entities = script->map->get_entities();
  Chest *chest = (Chest*) entities->get_entity(CHEST, name);
  chest->set_hidden(hidden);

  return 0;
}

/**
 * Returns whether a chest is hidden.
 * Argument 1 (string): name of the chest
 * Return value (boolean): true if this chest is hidden
 */
int MapScript::l_chest_is_hidden(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map->get_entities();
  Chest *chest = (Chest*) entities->get_entity(CHEST, name);
  lua_pushboolean(l, chest->is_hidden() ? 1 : 0);

  return 1;
}

/**
 * Enables or disables a dynamic tile.
 * Argument 1 (string): name of the dynamic tile
 * Argument 2 (boolean): true to enable it, false to disable it
 */
int MapScript::l_tile_set_enabled(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  bool enable = lua_toboolean(l, 2) != 0;

  MapEntities *entities = script->map->get_entities();
  DynamicTile *dynamic_tile = (DynamicTile*) entities->get_entity(DYNAMIC_TILE, name);
  dynamic_tile->set_enabled(enable);

  return 0;
}

/**
 * Enables or disables a set of dynamic tiles.
 * Argument 1 (string): prefix of the name of the dynamic tiles to disable
 * Argument 2 (boolean): true to enable them, false to disable them
 */
int MapScript::l_tiles_set_enabled(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &prefix = lua_tostring(l, 1);
  bool enable = lua_toboolean(l, 2) != 0;

  MapEntities *entities = script->map->get_entities();
  std::list<MapEntity*> *dynamic_tiles = entities->get_entities_with_prefix(DYNAMIC_TILE, prefix);

  std::list<MapEntity*>::iterator it;
  for (it = dynamic_tiles->begin(); it != dynamic_tiles->end(); it++) {
    DynamicTile *dynamic_tile = (DynamicTile*) (*it);
    dynamic_tile->set_enabled(enable);
  }

  delete dynamic_tiles;
  return 0;
}

/**
 * Returns whether a dynamic tile is enabled.
 * Argument 1 (string): name of the dynamic tile
 * Return value (boolean): true if this tile is enabled
 */
int MapScript::l_tile_is_enabled(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map->get_entities();
  DynamicTile *dynamic_tile = (DynamicTile*) entities->get_entity(DYNAMIC_TILE, name);
  lua_pushboolean(l, dynamic_tile->is_enabled() ? 1 : 0);

  return 1;
}

/**
 * Replaces a block at its initial position.
 * Argument 1 (string): name of the block to reset
 */
int MapScript::l_reset_block(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &block_name = lua_tostring(l, 1);

  MapEntities *entities = script->map->get_entities();
  Block *block = (Block*) entities->get_entity(BLOCK, block_name);
  block->reset();

  return 0;
}

/**
 * Replaces all blocks of the map at their initial position.
 */
int MapScript::l_reset_blocks(lua_State *l) {

  MapScript *script;
  called_by_script(l, 0, &script);

  MapEntities *entities = script->map->get_entities();
  std::list<MapEntity*> *blocks = entities->get_entities(BLOCK);

  std::list<MapEntity*>::iterator i;
  for (i = blocks->begin(); i != blocks->end(); i++) {
    ((Block*) (*i))->reset();
  }

  delete blocks;

  return 0;
}

/**
 * Returns the current animation of an interactive entity's sprite.
 * Argument 1 (string): name of the interactive entity
 * Return value (string): name of the current animation
 */
int MapScript::l_interactive_entity_get_animation(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map->get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  std::string animation = entity->get_sprite()->get_current_animation();

  lua_pushstring(l, animation.c_str());

  return 1;
}

/**
 * Returns the animation speed of an interactive entity's sprite.
 * Argument 1 (string): name of the interactive entity
 * Return value (integer): delay between two frames in milliseconds
 */
int MapScript::l_interactive_entity_get_animation_delay(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map->get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  uint32_t delay = entity->get_sprite()->get_frame_delay();

  lua_pushinteger(l, delay);

  return 1;
}

/**
 * Returns the current animation frame of an interactive entity's sprite.
 * Argument 1 (string): name of the interactive entity
 * Return value (integer): frame number
 */
int MapScript::l_interactive_entity_get_animation_frame(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map->get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  int frame = entity->get_sprite()->get_current_frame();

  lua_pushinteger(l, frame);

  return 1;
}

/**
 * Returns the current direction of an interactive entity's sprite.
 * Argument 1 (string): name of the interactive entity
 * Return value (integer): the direction between 0 and 3
 */
int MapScript::l_interactive_entity_get_direction(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map->get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  int frame = entity->get_sprite()->get_current_direction();

  lua_pushinteger(l, frame);

  return 1;
}

/**
 * Returns whether the animation of an interactive entity's sprite is paused.
 * Argument 1 (string): name of the interactive entity
 * Return value (boolean): true if the animation is paused
 */
int MapScript::l_interactive_entity_is_animation_paused(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map->get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  bool paused = entity->get_sprite()->is_paused();

  lua_pushboolean(l, paused ? 1 : 0);

  return 1;
}

/**
 * Sets the animation of an interactive entity's sprite.
 * Argument 1 (string): name of the interactive entity
 * Argument 2 (string): name of the animation to set
 */
int MapScript::l_interactive_entity_set_animation(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  const std::string &animation = lua_tostring(l, 2);

  MapEntities *entities = script->map->get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  entity->get_sprite()->set_current_animation(animation);
  entity->get_sprite()->restart_animation();

  return 0;
}

/**
 * Sets the animation speed of an interactive entity's sprite.
 * Argument 1 (string): name of the interactive entity
 * Argument 2 (integer): delay between two frames in milliseconds
 */
int MapScript::l_interactive_entity_set_animation_delay(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  uint32_t delay = lua_tointeger(l, 2);

  MapEntities *entities = script->map->get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  entity->get_sprite()->set_frame_delay(delay);

  return 0;
}

/**
 * Sets the current animation frame of an interactive entity's sprite.
 * Argument 1 (string): name of the interactive entity
 * Argument 2 (integer): frame number
 */
int MapScript::l_interactive_entity_set_animation_frame(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  int frame = lua_tointeger(l, 2);

  MapEntities *entities = script->map->get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  entity->get_sprite()->set_current_frame(frame);

  return 0;
}

/**
 * Sets the direction of an interactive entity's sprite.
 * Argument 1 (string): name of the interactive entity
 * Argument 2 (integer): the sprite's direction between 0 and 3
 */
int MapScript::l_interactive_entity_set_direction(lua_State *l) {

  return l_npc_set_direction(l);
}

/**
 * Pauses or resumes the animation of an interactive entity's sprite.
 * Argument 1 (string): name of the interactive entity
 * Argument 2 (boolean): true to pause, false to resume
 */
int MapScript::l_interactive_entity_set_animation_paused(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  bool paused = lua_toboolean(l, 2) != 0;

  MapEntities *entities = script->map->get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  entity->get_sprite()->set_paused(paused);

  return 0;
}

/**
 * Starts a fade-in or a fade-out effect on an interactive entity
 * Argument 1 (string): name of the interactive entity
 * Argument 2 (integer): direction of the effect: 0 for fade-in, 1 for fade-out
 */
int MapScript::l_interactive_entity_fade(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  int direction = lua_tointeger(l, 2);

  MapEntities *entities = script->map->get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  entity->start_fading(direction);

  return 0;
}

/**
 * Sets whether the animation of an interactive entity should continue even when the game is suspended.
 * Argument 1 (string): name of the interactive entity
 * Argument 2 (boolean): true to ignore when the game is suspended
 */
int MapScript::l_interactive_entity_set_animation_ignore_suspend(lua_State *l) {
  return l_npc_set_animation_ignore_suspend(l);
}

/**
 * Removes an interactive entity from the map.
 * Argument 1 (string): name of the interactive entity
 */
int MapScript::l_interactive_entity_remove(lua_State *l) {
  return l_npc_remove(l);
}

/**
 * Removes a shop item from the map.
 * Argument 1 (string): name of the shop item
 */
int MapScript::l_shop_item_remove(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  script->map->get_entities()->remove_entity(SHOP_ITEM, name);

  return 0;
}

/**
 * Returns whether a switch is currently enabled.
 * Argument 1 (string): name of the switch
 * Return value (boolean): true if the switch is enabled
 */
int MapScript::l_switch_is_enabled(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map->get_entities();
  Switch *sw = (Switch*) entities->get_entity(SWITCH, name);

  lua_pushboolean(l, sw->is_enabled());

  return 1;
}

/**
 * Enables or disables a switch.
 * Argument 1 (string): name of the switch
 * Argument 2 (boolean): true to enable the switch, false to disable it
 */
int MapScript::l_switch_set_enabled(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  bool enable = lua_toboolean(l, 2) != 0;

  MapEntities *entities = script->map->get_entities();
  Switch *sw = (Switch*) entities->get_entity(SWITCH, name);
  sw->set_enabled(enable);

  return 0;
}

/**
 * Locks a switch in its current state or unlocks it.
 * Argument 1 (string): name of the switch
 * Argument 2 (boolean): true to lock the switch, false to unlock it
 */
int MapScript::l_switch_set_locked(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  bool lock = lua_toboolean(l, 2) != 0;

  MapEntities *entities = script->map->get_entities();
  Switch *sw = (Switch*) entities->get_entity(SWITCH, name);
  sw->set_locked(lock);

  return 0;
}

/**
 * Returns whether an enemy is dead.
 * An enemy is considered as dead if it is not present on the map.
 * Argument 1 (string): name of the enemy
 * Return value (boolean): true if the enemy is not on the map, false if it is alive
 */
int MapScript::l_enemy_is_dead(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map->get_entities();
  Enemy *enemy = (Enemy*) entities->find_entity(ENEMY, name);

  lua_pushboolean(l, (enemy == NULL));

  return 1;
}

/**
 * Returns whether a set of enemies are dead.
 * An enemy is considered as dead if it is not present on the map.
 * Argument 1 (string): prefix of the name of the enemies to check
 * Return value (boolean): true if there is no enemy left with this prefix on the map,
 * false if at least one of them is alive
 */
int MapScript::l_enemies_are_dead(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &prefix = lua_tostring(l, 1);

  MapEntities *entities = script->map->get_entities();
  std::list<MapEntity*> *enemies = entities->get_entities_with_prefix(ENEMY, prefix);

  lua_pushboolean(l, enemies->empty());

  delete enemies;
  return 1;
}

/**
 * Enables or disables an enemy.
 * A normal enemy is enabled by default. A boss or a miniboss is disabled by default.
 * Argument 1 (string): name of the enemy
 * Argument 2 (boolean): true to enable the enemy, false to disable it
 */
int MapScript::l_enemy_set_enabled(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  bool enable = lua_toboolean(l, 2) != 0;

  MapEntities *entities = script->map->get_entities();
  Enemy *enemy = (Enemy*) entities->get_entity(ENEMY, name);
  enemy->set_enabled(enable);

  return 0;
}

/**
 * Starts the battle against a boss.
 * Calling this function enables the boss if he is alive and plays the appropriate music.
 * If the boss was already killed, nothing happens.
 * Argument 1 (string): name of the boss
 */
int MapScript::l_boss_start_battle(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map->get_entities();
  Enemy *enemy = (Enemy*) entities->find_entity(ENEMY, name); 
  entities->start_boss_battle(enemy);

  return 0;
}

/**
 * Ends the battle against a boss.
 * Calling this function plays the appropriate music and freezes the hero.
 * The next step is usually to start the dungeon end sequence.
 */
int MapScript::l_boss_end_battle(lua_State *l) {

  MapScript *script;
  called_by_script(l, 0, &script);

  script->map->get_entities()->end_boss_battle();

  return 0;
}

/**
 * Starts the battle against a miniboss.
 * Calling this function enables the miniboss if he is alive and plays the appropriate music.
 * If the miniboss was already killed, nothing happens.
 * Argument 1 (string): name of the miniboss
 */
int MapScript::l_miniboss_start_battle(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map->get_entities();
  Enemy *enemy = (Enemy*) entities->find_entity(ENEMY, name); 
  entities->start_miniboss_battle(enemy);

  return 0;
}

/**
 * Ends the battle against a miniboss.
 * Calling this function plays the appropriate music.
 */
int MapScript::l_miniboss_end_battle(lua_State *l) {

  MapScript *script;
  called_by_script(l, 0, &script);

  script->map->get_entities()->end_miniboss_battle();

  return 0;
}

/**
 * Removes a sensor from the map.
 * Argument 1 (string): name of the sensor
 */
int MapScript::l_sensor_remove(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  script->map->get_entities()->remove_entity(SENSOR, name);

  return 0;
}

/**
 * Opens one or several doors.
 * The doors must be normal, closed door
 * (not doors for keys or bombs).
 * Argument 1 (string): prefix of the name of the doors to open
 */
int MapScript::l_door_open(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &prefix = lua_tostring(l, 1);

  MapEntities *entities = script->map->get_entities();
  std::list<MapEntity*> *doors = entities->get_entities_with_prefix(DOOR, prefix);
  std::list<MapEntity*>::iterator it;
  for (it = doors->begin(); it != doors->end(); it++) {
    Door *door = (Door*) (*it);
    door->open();
  }
  script->game->play_sound("door_open");
  delete doors;

  return 0;
}

/**
 * Closes one or several doors.
 * The doors must be normal, open door
 * (not doors for keys or bombs).
 * Argument 1 (string): prefix of the name of the doors to close
 */
int MapScript::l_door_close(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &prefix = lua_tostring(l, 1);

  MapEntities *entities = script->map->get_entities();
  std::list<MapEntity*> *doors = entities->get_entities_with_prefix(DOOR, prefix);
  std::list<MapEntity*>::iterator it;
  for (it = doors->begin(); it != doors->end(); it++) {
    Door *door = (Door*) (*it);
    door->close();
  }
  script->game->play_sound("door_closed");
  delete doors;

  return 0;
}

/**
 * Returns whether a door is open
 * Argument 1 (string): name of the door
 * Return value (boolean): true if this door is open
 */
int MapScript::l_door_is_open(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);
  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map->get_entities();
  Door *door = (Door*) entities->get_entity(DOOR, name);
  lua_pushboolean(l, door->is_open() ? 1 : 0);

  return 1;
}

/**
 * Makes one or several doors open or closed.
 * Argument 1 (string): prefix of the name of the doors to close
 * Argument 2 (boolean): true to make them open, false to make them closed
 */
int MapScript::l_door_set_open(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &prefix = lua_tostring(l, 1);
  bool open = lua_toboolean(l, 2) != 0;

  MapEntities *entities = script->map->get_entities();
  std::list<MapEntity*> *doors = entities->get_entities_with_prefix(DOOR, prefix);
  std::list<MapEntity*>::iterator it;
  for (it = doors->begin(); it != doors->end(); it++) {
    Door *door = (Door*) (*it);
    door->set_open(open);
  }
  delete doors;

  return 0;
}

// event functions, i.e. functions called by the C++ engine to notify the map script that something happened

/**
 * Notifies the script that the map has just been started.
 * @param destination_point_name name of the destination point where the hero is
 */
void MapScript::event_map_started(const std::string &destination_point_name) {
  call_script_function("event_map_started",  destination_point_name);
}

/**
 * Notifies the script that the opening transition of the map has just finished.
 * @param destination_point_name name of the destination point where the hero is
 */
void MapScript::event_map_opening_transition_finished(const std::string &destination_point_name) {
  call_script_function("event_map_opening_transition_finished", destination_point_name);
}

/**
 * Notifies the script that a switch has just been enabled.
 * @param switch_name name of the switch
 */
void MapScript::event_switch_enabled(const std::string &switch_name) {
  call_script_function("event_switch_enabled", switch_name);
}

/**
 * Notifies the script that a switch has just been disabled.
 * @param switch_name name of the switch
 */
void MapScript::event_switch_disabled(const std::string &switch_name) {
  call_script_function("event_switch_disabled", switch_name);
}

/**
 * Notifies the script that a switch has just been left by the entity that was on it.
 * The fact that the switch is enabled or disabled does not matter here.
 * @param switch_name name of the switch
 */
void MapScript::event_switch_left(const std::string &switch_name) {
  call_script_function("event_switch_left", switch_name);
}

/**
 * Notifies the script that the victory sequence of the hero has just finished.
 */
void MapScript::event_hero_victory_sequence_finished(void) {
  call_script_function("event_hero_victory_sequence_finished");
}

/**
 * Notifies the script that the hero is overlapping a sensor.
 * @param sensor_name name of the sensor
 */
void MapScript::event_hero_on_sensor(const std::string &sensor_name) {
  call_script_function("event_hero_on_sensor", sensor_name);
  this->hero->reset_movement();
}

/**
 * Notifies the script that the player has just pressed the action
 * key in front of an interactive entity.
 * @param entity_name name of the interactive entity
 */
void MapScript::event_hero_interaction(const std::string &entity_name) {
  call_script_function("event_hero_interaction", entity_name);
}

/**
 * Notifies the script that the hero is using an inventory item
 * in front of a interactive entity.
 * This event is called only for inventory items that want to use it
 * (e.g. a key that is being used in front of a door).
 * @param entity_name name of the interactive entity the hero is facing
 * @param item_id id of the inventory item that is being used
 * @param variant variant of this inventory item
 * @return true if the script has handled the event
 */
bool MapScript::event_hero_interaction_item(const std::string &entity_name, InventoryItemId item_id, int variant) {

  bool exists = call_script_function("event_hero_interaction_item", entity_name, item_id, variant);
  bool interaction = lua_toboolean(context, 1);

  return exists && interaction;
}

/**
 * Notifies the script that the player has just pressed the action
 * key in front an NPC.
 * @param npc_name name of the NPC
 */
void MapScript::event_npc_dialog(const std::string &npc_name) {
  call_script_function("event_npc_dialog", npc_name);
}

/**
 * Notifies the script that an NPC has just finished its movement.
 * @param npc_name name of the NPC
 */
void MapScript::event_npc_movement_finished(const std::string &npc_name) {
  call_script_function("event_npc_movement_finished", npc_name);
}

/**
 * Notifies the script that the player has just open an empty chest.
 * What happens next is controlled by your script if it handles this event.
 * The hero is in state FREEZE
 * so if you do something else than giving the player a treasure,
 * don't forget to call hero_unfreeze() once you have finished.
 * The script function does not have to return any value.
 * @param chest_name name of the chest
 * @return true if the script has handled the event, i.e. if the
 * event_chest_empty exists in the script
 */
bool MapScript::event_chest_empty(const std::string &chest_name) {
  return call_script_function("event_chest_empty", chest_name);
}

/**
 * Notifies the script that the player has just bought an item in a shop.
 * @param shop_item_name name of the item bought
 */
void MapScript::event_shop_item_bought(const std::string &shop_item_name) {
  call_script_function("event_shop_item_bought", shop_item_name);
}

/**
 * Notifies the script that an enemy has just been killed.
 * @param enemy_name name of the enemy
 */
void MapScript::event_enemy_dead(const std::string &enemy_name) {
  call_script_function("event_enemy_dead", enemy_name);
}

