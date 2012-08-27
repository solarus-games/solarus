local map = ...
-- Castle 1F

function map:on_started(destination_point)

  -- make the NPCs walk
  random_walk("walking_npc_1")
  random_walk("walking_npc_2")

  if destination_point:get_name() == "from_ending" then
    -- game ending sequence
    hero:freeze()
    hero:set_visible(false)
    map:get_game():set_hud_enabled(false)
    map:set_entities_enabled("", false)
    sol.audio.play_music("fanfare")
  else
    tom:set_enabled(false)
  end
end

function map:on_opening_transition_finished(destination_point)

  if destination_point:get_name() == "from_ending" then
    map:start_dialog("credits_4")
    map:move_camera(288, 96, 25, function() end, 1e6)
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

    local hero_x = hero:get_position()
    if hero_x < 832 then
      map:start_dialog("castle.forbidden_door")
    else
      map:start_dialog("castle.forbidden_door_wtf")
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
    hero:freeze()
  elseif dialog_id == "credits_4" then
   sol.timer.start(2000, ending_next)
  end
end

function map:on_npc_movement_finished(npc_name)

  if npc_name == "forbidden_door_npc" then

    hero:unfreeze()
    local sprite = map:npc_get_sprite(npc_name)
    if sprite:get_direction() == 1 then
      sprite:set_direction(3)
      hero:walk("4444", false, false)
    end
  end
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "forbidden_door_sensor" then

    local npc_name = "forbidden_door_npc"
    x, y = map:get_entity(npc_name):get_position()
    if y < 877 then
      local sprite = map:npc_get_sprite(npc_name)
      local m = sol.movement.create("path")
      m:set_path{6,6}
      m:set_speed(32)
      map:npc_start_movement(npc_name, m)
      sprite:set_animation("walking")
      hero:freeze()
    end
  end
end

function ending_next()
  hero:teleport(119, "from_ending")
end

