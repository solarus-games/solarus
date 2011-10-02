-- Dungeon 5 2F

sol.main.include("maps/prison_guard")

function event_map_started(destination_point_name)

  init_guard("guard_3", "44444222222222220000066666666666")
end

function event_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_roof"
      or destination_point_name == "from_outside_w"
      or destination_point_name == "from_outside_e" then
    sol.map.dialog_start("dungeon_5.welcome")
  end
end

function init_guard(guard_name, path)

  local sprite = sol.map.npc_get_sprite(guard_name)
  local m = sol.main.path_movement_create(path, 48)
  sol.main.movement_set_property(m, "loop", true)
  sol.main.movement_set_property(m, "ignore_obstacles", true)
  sol.map.npc_start_movement(guard_name, m)
  sol.main.sprite_set_animation(sprite, "walking")
end

function event_hero_on_sensor(sensor_name)

  sensor_check_guard(sensor_name)
end

function event_hero_still_on_sensor(sensor_name)

  sensor_check_guard(sensor_name)
end

function event_dialog_finished(first_message_id, answer)

  dialog_check_guard(first_message_id)
end

