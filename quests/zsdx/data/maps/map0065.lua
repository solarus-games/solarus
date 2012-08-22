local map = ...
-- Dungeon 5 B1

sol.main.do_file("maps/prison_guard")

local prison_2_nb_messages = 0
local door_sets = { -- possible doors open when going to prison
  { "door_b", "door_d", "door_f", "door_h", "door_g" },
  { "door_b", "door_d", "door_f", "door_h", "door_g" },
  { "door_b", "door_d", "door_e", "door_f", "door_g" },
  { "door_b", "door_d", "door_e", "door_f", "door_g" },
  { "door_a", "door_c", "door_e", "door_d", "door_f", "door_h" },
}

function map:on_started(destination_point_name)

  if destination_point_name == "from_outside_hole" then
    map:hero_set_direction(2)
  end

  if not map:get_game():get_boolean(907) then
    -- this door is open until the main entrance door of
    -- the castle is open
    map:set_doors_open("n_door", true)
  end

  if map:get_game():get_boolean(511) then
    map:npc_remove("prison_1_lock")
  end

  if map:get_game():get_boolean(512) then
    prison_2_nb_messages = 3
  end

  if not map:get_game():get_boolean(519) then
    map:chest_set_enabled("boss_key_chest", false)
  end

  -- bomb bag 2 or 3
  local variant = 2
  if map:get_game():get_boolean(938) then
    -- already has the other one
    variant = 3
  end
  map:create_destructible("pot", 320, 245, 1, {
    treasure_item = "bomb_bag",
    treasure_variant = variant,
    treasure_savegame_variable = 510
  })

  -- initialize doors
  local i = 1
  if destination_point_name == "prison" then
    i = math.random(#door_sets)
  end
  for _, door in ipairs(door_sets[i]) do
    map:set_doors_open(door, true)
  end

  init_guards()
end

function init_guards()

  init_guard("guard_1", 296, 573, 2)
  init_guard("guard_2", 296, 685, 2)
  init_guard("guard_3", 288, 821, 2,
      {4,4,4,4,4,4,4,4,4,
      6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
      4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4})
  init_guard("guard_4", 776, 917, 2)
  init_guard("guard_5", 920, 725, 2,
      {4,4,4,4,4,4,4,4,
      2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
      4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4})
  init_guard("guard_6", 1080, 597, 2,
      {2,2,2,2,2,2,2,2,
      4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
      6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2})
end

function init_prison()

  map:npc_set_position("prison_2_lock", 648, 325) 
end

function init_guard(guard_name, x, y, direction, path)

  map:npc_stop_movement(guard_name)
  map:npc_set_position(guard_name, x, y)
  local sprite = map:npc_get_sprite(guard_name)
  if path ~= nil then
    local m = sol.movement.create("path")
    m:set_path(path)
    m:set_speed(72)
    m:set_loop(true)
    m:set_ignore_obstacles(true)
    map:npc_start_movement(guard_name, m)
    sprite:set_animation("walking")
  else
    sprite:set_animation("stopped")
    sprite:set_direction(direction)
  end
end

function map:on_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside_hole" then
    map:start_dialog("dungeon_5.welcome")
  end
end

function map:on_hero_on_sensor(sensor_name)

  sensor_check_guard(sensor_name)

  if sensor_name == "prison_sensor" then
    init_guards()

    -- change the configuration of the doors
    local i = math.random(#door_sets)
    map:set_doors_open("door", false)
    for _, door in ipairs(door_sets[i]) do
      map:set_doors_open(door, true)
    end
  end
end

function map:on_hero_still_on_sensor(sensor_name)

  sensor_check_guard(sensor_name)
end

function map:on_dialog_finished(dialog_id, answer)

  dialog_check_guard(dialog_id)

  if dialog_id == "dungeon_5.prison_1_use_iron_key" then
    sol.audio.play_sound("secret")
    sol.audio.play_sound("door_open")
    map:npc_remove("prison_1_lock")
    map:get_game():set_boolean(511, true)
  end
end

function map:on_npc_interaction(npc_name)

  if npc_name == "prison_1_lock" then

    if not map:get_game():has_item("iron_key") then
      map:start_dialog("dungeon_5.prison_1_locked")
    else
      map:start_dialog("dungeon_5.prison_1_use_iron_key")
    end
  elseif npc_name == "prison_2_lock" then
    
    prison_2_nb_messages = prison_2_nb_messages + 1
    if prison_2_nb_messages <= 3 then
      map:start_dialog("dungeon_5.prison_2_locked_"..prison_2_nb_messages)
    else
      sol.audio.play_sound("secret")
      sol.audio.play_sound("door_open")
      map:npc_set_position("prison_2_lock", 648, -32)
      map:get_game():set_boolean(512, true)
    end
  end
end

function map:on_door_open(door_name)

  if door_name:find("^weak_wall") then
    sol.audio.play_sound("secret")
  end
end

function map:on_obtained_treasure(item_name, variant, savegame_variable)

  if item_name == "boss_key" then
    -- the hero was unfreezed by the chest, so cancel a possible previous guard
    -- (putting chests in the area of guards is probably not a good idea)
    cancel_prison()
  end
end

