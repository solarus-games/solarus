-- Dungeon 5 B1

sol.main.include("maps/prison_guard")

prison_2_nb_messages = 0

function event_map_started(destination_point_name)

  if destination_point_name == "from_outside_hole" then
    sol.map.hero_set_direction(2)
  end

  if not sol.game.savegame_get_boolean(907) then
    -- this door is open until the main entrance door of
    -- the castle is open
    sol.map.door_set_open("n_door", true)
  end

  if sol.game.savegame_get_boolean(511) then
    sol.map.interactive_entity_remove("prison_1_lock")
  end

  if sol.game.savegame_get_boolean(512) then
    prison_2_nb_messages = 2
  end

  if not sol.game.savegame_get_boolean(519) then
    sol.map.chest_set_enabled("boss_key_chest", false)
  end

  init_guards()
end

function init_guards()

  init_guard("guard_1", 296, 573, 2)
  init_guard("guard_2", 296, 685, 2)
  init_guard("guard_3", 288, 821, 2,
      "44444444466666666666666666666666600000000000000000000000000002222222222222222222222224444444444444444444")
  init_guard("guard_4", 776, 917, 2)
  init_guard("guard_5", 920, 725, 2,
      "44444444222222222222222222222222000000000000000000000000000066666666666666666666666644444444444444444444")
  init_guard("guard_6", 1080, 597, 2,
      "22222222444444444444444444444444444466666666666666666666666600000000000000000000000000002222222222222222")
end

function init_prison()
  sol.map.npc_set_position("prison_2_lock", 648, 325)
end

function init_guard(guard_name, x, y, direction, path)

  sol.map.npc_stop_movement(guard_name)
  sol.map.npc_set_position(guard_name, x, y)
  local sprite = sol.map.npc_get_sprite(guard_name)
  if path ~= nil then
    local m = sol.main.path_movement_create(path, 72)
    sol.main.movement_set_property(m, "loop", true)
    sol.main.movement_set_property(m, "ignore_obstacles", true)
    sol.map.npc_start_movement(guard_name, m)
    sol.main.sprite_set_animation(sprite, "walking")
  else
    sol.main.sprite_set_animation(sprite, "stopped")
    sol.main.sprite_set_direction(sprite, direction)
  end
end

function event_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside_hole" then
    sol.map.dialog_start("dungeon_5.welcome")
  end
end

function event_hero_on_sensor(sensor_name)

  sensor_check_guard(sensor_name)

  if sensor_name == "prison_sensor" then
    init_guards()
  end
end

function event_hero_still_on_sensor(sensor_name)

  sensor_check_guard(sensor_name)
end

function event_dialog_finished(first_message_id, answer)

  dialog_check_guard(first_message_id)

  if first_message_id == "dungeon_5.prison_1_use_bone_key" then
    sol.main.play_sound("secret")
    sol.main.play_sound("door_open")
    sol.map.interactive_entity_remove("prison_1_lock")
    sol.game.savegame_set_boolean(511, true)
  end
end

function event_hero_interaction(entity_name)

  if entity_name == "prison_1_lock" then

    if not sol.game.has_item("bone_key") then
      sol.map.dialog_start("dungeon_5.prison_1_locked")
    else
      sol.map.dialog_start("dungeon_5.prison_1_use_bone_key")
    end
  elseif entity_name == "prison_2_lock" then
    
    prison_2_nb_messages = prison_2_nb_messages + 1
    if prison_2_nb_messages <= 3 then
      sol.map.dialog_start("dungeon_5.prison_2_locked_"..prison_2_nb_messages)
    else
      sol.main.play_sound("secret")
      sol.main.play_sound("door_open")
      sol.map.npc_set_position("prison_2_lock", 648, -32)
      sol.game.savegame_set_boolean(512, true)
    end
  end
end

