#ifndef ZSDX_MAP_SCRIPT_H
#define ZSDX_MAP_SCRIPT_H

#include "Common.h"
#include "Treasure.h"
#include <list>

struct lua_State;

/**
 * Represents the Lua script of a map.
 * This class makes the interface between the engine C++ code and the map Lua script.
 */
class MapScript {

 private:

  typedef int (FunctionAvailableToScript) (lua_State *l); /**< functions that can be called by the Lua script */

  Map *map;                   /**< the map associated to this script */
  lua_State* context;         /**< the execution context of the Lua script */
  std::list<Timer*> timers;   /**< the timers currently running for this script */

  bool call_lua_function(const char *function_name);
  bool call_lua_function(const char *function_name, int nb_arguments, ...);

  // C++ functions that can be called by the script
  static FunctionAvailableToScript l_freeze;
  static FunctionAvailableToScript l_unfreeze;
  static FunctionAvailableToScript l_play_sound;
  static FunctionAvailableToScript l_start_message;
  static FunctionAvailableToScript l_set_message_variable;
  static FunctionAvailableToScript l_give_treasure;
  static FunctionAvailableToScript l_give_treasure_with_amount;
  static FunctionAvailableToScript l_savegame_get_string;
  static FunctionAvailableToScript l_savegame_get_integer;
  static FunctionAvailableToScript l_savegame_get_boolean;
  static FunctionAvailableToScript l_savegame_set_string;
  static FunctionAvailableToScript l_savegame_set_integer;
  static FunctionAvailableToScript l_savegame_set_boolean;
  static FunctionAvailableToScript l_start_timer;
  static FunctionAvailableToScript l_stop_timer;
  static FunctionAvailableToScript l_move_camera;
  static FunctionAvailableToScript l_restore_camera;
  static FunctionAvailableToScript l_npc_walk;
  static FunctionAvailableToScript l_npc_random_walk;
  static FunctionAvailableToScript l_npc_set_direction;
  static FunctionAvailableToScript l_npc_remove;
  static FunctionAvailableToScript l_set_chest_open;
  static FunctionAvailableToScript l_get_rupees;
  static FunctionAvailableToScript l_remove_rupees;
  static FunctionAvailableToScript l_disable_tile;
  static FunctionAvailableToScript l_enable_tile;
  static FunctionAvailableToScript l_is_tile_enabled;
  static FunctionAvailableToScript l_reset_block;
  static FunctionAvailableToScript l_reset_blocks;
  static FunctionAvailableToScript l_interactive_entity_get_animation;
  static FunctionAvailableToScript l_interactive_entity_get_animation_delay;
  static FunctionAvailableToScript l_interactive_entity_get_animation_frame;
  static FunctionAvailableToScript l_interactive_entity_is_animation_paused;
  static FunctionAvailableToScript l_interactive_entity_set_animation;
  static FunctionAvailableToScript l_interactive_entity_set_animation_delay;
  static FunctionAvailableToScript l_interactive_entity_set_animation_frame;
  static FunctionAvailableToScript l_interactive_entity_set_animation_paused;

  static void check_nb_arguments(lua_State *context, int nb_arguments);
  void register_c_functions(void);
  void add_timer(Timer *timer);
  void remove_timer(string callback_name);

 public:

  // loading and closing a script
  MapScript(Map *map);
  ~MapScript(void);
  void initialize(void);

  // update functions
  void update(void);
  void set_suspended(bool suspended);

  // C++ functions that call script functions
  void event_map_started(void);
  void event_message_started(MessageId message_id);
  void event_message_sequence_finished(MessageId first_message_id, int answer);
  void event_switch_enabled(Switch *sw);
  void event_switch_disabled(Switch *sw);
  void event_camera_reached_target(void);
  void event_interaction(string entity_name);
  void event_npc_dialog(string npc_name);
  void event_npc_path_finished(string npc_name);
  bool event_open_empty_chest(string chest_name);
  void event_got_treasure(Treasure::Content content, int savegame_variable);
};

#endif
