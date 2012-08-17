local map = ...
-- Castle 1F

function map:on_started(destination_point_name)

  -- make the NPCs walk
  random_walk("walking_npc_1")
  random_walk("walking_npc_2")

  if destination_point_name == "from_ending" then
    -- game ending sequence
    map:hero_freeze()
    map:hero_set_visible(false)
    map:get_game():set_hud_enabled(false)
    map:enemy_set_group_enabled("", false)
    sol.audio.play_music("fanfare")
  else
    map:npc_set_enabled("tom", false)
  end
end

function map:on_map_opening_transition_finished(destination_point_name)

  if destination_point_name == "from_ending" then
    map:dialog_start("credits_4")
    map:camera_move(288, 96, 25, function() end, 1e6)
  end
end

function random_walk(npc_name)

  local m = sol.movement.create("random_path")
  m:set_speed(32)
  map:npc_start_movement(npc_name, m)
  map:npc_get_sprite(npc_name):set_animation("walking")
end

function map:on_npc_interaction(npc_name)
  
  if npc_name == "forbidden_door_npc" then

    local hero_x = map:hero_get_position()
    if hero_x < 832 then
      map:dialog_start("castle.forbidden_door")
    else
      map:dialog_start("castle.forbidden_door_wtf")
    end
  end
end

function map:on_dialog_finished(dialog_id)

  if dialog_id == "castle.forbidden_door_wtf" then
    local m = sol.movement.create("path")
    m:set_path{2,2}
    m:set_speed(32)
    local npc_name = "forbidden_door_npc"
    local sprite = map:npc_get_sprite(npc_name)
    map:npc_start_movement(npc_name, m)
    sprite:set_animation("walking")
    map:hero_freeze()
  elseif dialog_id == "credits_4" then
   sol.timer.start(2000, ending_next)
  end
end

function map:on_npc_movement_finished(npc_name)

  if npc_name == "forbidden_door_npc" then

    map:hero_unfreeze()
    local sprite = map:npc_get_sprite(npc_name)
    if sprite:get_direction() == 1 then
      sprite:set_direction(3)
      map:hero_walk("4444", false, false)
    end
  end
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "forbidden_door_sensor" then

    local npc_name = "forbidden_door_npc"
    x, y = map:npc_get_position(npc_name)
    if y < 877 then
      local sprite = map:npc_get_sprite(npc_name)
      local m = sol.movement.create("path")
      m:set_path{6,6}
      m:set_speed(32)
      map:npc_start_movement(npc_name, m)
      sprite:set_animation("walking")
      map:hero_freeze()
    end
  end
end

function ending_next()
  map:hero_set_map(119, "from_ending", 1)
end

