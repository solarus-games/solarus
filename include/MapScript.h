#ifndef ZSDX_MAP_SCRIPT_H
#define ZSDX_MAP_SCRIPT_H

#include "Common.h"

struct lua_State;

/**
 * Represents the Lua script of a map.
 * This class makes the interface between the engine C++ code and the map Lua script.
 */
class MapScript {

 private:

  lua_State* context;  /** the execution context of the Lua script */
  string lua_function; /** the */

  void register_c_functions(void);

  void call_lua_function(const char *value_function_name);
  void call_lua_function(const char *value_function_name, int nb_arguments, ...);

 public:

  MapScript(Map *map);
  ~MapScript(void);

  void event_map_started(void);
  void event_message_started(string MessageId);
};

#endif
