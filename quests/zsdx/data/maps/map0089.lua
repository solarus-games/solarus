-- Castle 1F

function event_map_started(destination_point_name)

  -- make the NPCs walk
  random_walk("walking_npc_1")
  random_walk("walking_npc_2")
end

function random_walk(npc_name)

  local m = sol.main.random_path_movement_create(32)
  sol.map.npc_start_movement(npc_name, m)
  sol.main.sprite_set_animation(sol.map.npc_get_sprite(npc_name), "walking")
end

function event_npc_dialog(npc_name)
  
  if npc_name == "forbidden_door_npc" then

    local hero_x = sol.map.hero_get_position()
    if hero_x < 832 then
      sol.map.dialog_start("castle.forbidden_door")
    else
      sol.map.dialog_start("castle.forbidden_door_wtf")
    end
  end
end

function event_dialog_finished(first_message_id)

  if first_message_id == "castle.forbidden_door_wtf" then
    local m = sol.main.path_movement_create("22", 32)
    local npc_name = "forbidden_door_npc"
    local sprite = sol.map.npc_get_sprite(npc_name)
    sol.map.npc_start_movement(npc_name, m)
    sol.main.sprite_set_animation(sprite, "walking")
    sol.map.hero_freeze()
  end
end

function event_npc_movement_finished(npc_name)

  if npc_name == "forbidden_door_npc" then

    sol.map.hero_unfreeze()
    local sprite = sol.map.npc_get_sprite(npc_name)
    if sol.main.sprite_get_direction(sprite) == 1 then
      sol.main.sprite_set_direction(sprite, 3)
      sol.map.hero_walk("4444", false, false)
    end
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "forbidden_door_sensor" then

    local npc_name = "forbidden_door_npc"
    x, y = sol.map.npc_get_position(npc_name)
    if y < 877 then
      local sprite = sol.map.npc_get_sprite(npc_name)
      local m = sol.main.path_movement_create("66", 32)
      sol.map.npc_start_movement(npc_name, m)
      sol.main.sprite_set_animation(sprite, "walking")
      sol.map.hero_freeze()
    end
  end
end

