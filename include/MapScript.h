#ifndef ZSDX_MAP_SCRIPT_H
#define ZSDX_MAP_SCRIPT_H

#include "Common.h"
#include <list>

struct lua_State;

/**
 * Represents the Lua script of a map.
 * This class makes the interface between the engine C++ code and the map Lua script.
 */
class MapScript {

 private:

  lua_State* context;         /**< the execution context of the Lua script */
  std::list<Timer*> timers;   /**< the timers currently running for this script */

  void register_c_functions(void);

  void call_lua_function(const char *function_name);
  void call_lua_function(const char *function_name, int nb_arguments, ...);

 public:

  MapScript(Map *map);
  ~MapScript(void);

  void update(void);
  void set_suspended(bool suspended);
  void add_timer(Timer *timer);

  void event_map_started(void);
  void event_message_started(string MessageId);
  void event_entity_on_detector(Detector *detector, MapEntity *entity);
};

#endif
