-- Dungeon 2 B1

-- correct order of the switches
switches_puzzle_order = {
  switch_a = 1,
  switch_b = 2,
  switch_c = 3,
  switch_d = 4,
  switch_e = 5,
  switch_f = 6
}

switches_puzzle_nb_enabled = 0
switches_puzzle_correct = true

-- boss
local boss_arrows = {
  [13] = { x = 864, y = 477, created = false },
  [37] = { x = 896, y = 341, created = false },
  [55] = { x = 1024, y = 341, created = false },
  [80] = { x = 1056, y = 477, created = false },
}
fighting_boss = false

function event_map_started(destination_point)

  sol.map.chest_set_enabled("boss_key_chest", false)

  if sol.game.savegame_get_boolean(81) then
    -- boss key chest already found
    for k,v in pairs(switches_puzzle_order) do
      sol.map.switch_set_activated(k, true)
    end
  end

  sol.map.door_set_open("boss_door", true)
  if destination_point_name == "from_final_room"
      or sol.game.savegame_get_boolean(103) then
    sol.map.door_set_open("final_room_door", true)
  end

  if sol.game.savegame_get_boolean(103) then
    sol.map.tile_set_enabled("boss_killed_floor", true)
  end
end

function event_switch_activated(switch_name)

  local order = switches_puzzle_order[switch_name]
  if order ~= nil then 

    switches_puzzle_nb_enabled = switches_puzzle_nb_enabled + 1
    if switches_puzzle_nb_enabled ~= order then
      switches_puzzle_correct = false
    end

    if switches_puzzle_nb_enabled == 6 then

      if switches_puzzle_correct then
	sol.map.camera_move(240, 328, 250, boss_key_chest_timer)
      else
	sol.main.play_sound("wrong")
	switches_puzzle_nb_enabled = 0
	switches_puzzle_correct = true
	sol.map.switch_set_locked(switch_name, true)
	for k,v in pairs(switches_puzzle_order) do
	  sol.map.switch_set_activated(k, false)
	end
      end
    end
  end
end

function event_switch_left(switch_name)

  if switches_puzzle_nb_enabled == 0 then
    for k,v in pairs(switches_puzzle_order) do
      sol.map.switch_set_locked(k, false)
    end
  end
end

function boss_key_chest_timer()
  sol.map.chest_set_enabled("boss_key_chest", true)
  sol.main.play_sound("chest_appears")
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "start_boss_sensor"
      and not sol.game.savegame_get_boolean(93)
      and not fighting_boss then
    start_boss()

  elseif sensor_name == "close_boss_door_sensor"
      and sol.map.door_is_open("boss_door")
      and not sol.game.savegame_get_boolean(93)
      and not fighting_boss then
    sol.main.play_music("none")
    sol.map.door_close("boss_door")

  elseif sensor_name == "save_solid_ground_sensor" then
    sol.map.hero_save_solid_ground(960, 525, 0)

  elseif sensor_name == "boss_floor_sensor_1" then
    if fighting_boss
      and sol.map.tile_is_enabled("boss_floor_1") then

      sol.map.sensor_set_group_enabled("boss_floor_sensor", false)
      boss_restore_floor(true)
      boss_change_floor(1, 92, 1, false)
      sol.main.timer_start(function()
        sol.map.sensor_set_group_enabled("boss_floor_sensor", true)
	boss_change_floor(92, 1, -1, true)
      end, 10000)
    end

  elseif sensor_name == "boss_floor_sensor_2" then
    if fighting_boss
      and sol.map.tile_is_enabled("boss_floor_92") then

      sol.map.sensor_set_group_enabled("boss_floor_sensor", false)
      boss_restore_floor(true)
      boss_change_floor(92, 1, -1, false)
      sol.main.timer_start(function()
        sol.map.sensor_set_group_enabled("boss_floor_sensor", true)
	boss_change_floor(1, 92, 1, true)
      end, 10000)
    end
  end
end

function start_boss()

  fighting_boss = true
  sol.map.enemy_set_enabled("boss", true)
  sol.main.play_music("boss.spc")
end

function event_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    sol.main.timer_start(open_final_room, 9000)
    sol.main.play_music("victory.spc")
    sol.map.hero_freeze()
    sol.map.hero_set_direction(3)
  end
end

function open_final_room()

  sol.main.play_sound("secret")
  sol.map.door_open("final_room_door")
  sol.map.tile_set_enabled("boss_killed_floor", true)
  sol.map.hero_unfreeze()
end

function boss_change_floor(first, last, inc, enable)

  local index = first
  local delay
  if enable then
    delay = 30
    for i, v in ipairs(boss_arrows) do
      v.created = false
    end
  else
    delay = 70
  end

  function repeat_change()
    if (enable and index % 10 == 0)
      or (not enable and index % 5 == 0) then
      sol.main.play_sound("stone")
    end
    
    -- enable/disable the tile
    sol.map.tile_set_enabled("boss_floor_" .. index, enable)

    -- create an arrow with some tiles
    if enable and boss_arrows[index] ~= nil then
      sol.map.pickable_item_create("arrow", 1, -1,
          boss_arrows[index].x, boss_arrows[index].y, 0)
      boss_arrows[index].created = true
    end

    if index ~= last then
      sol.main.timer_start(repeat_change, delay)
    end
    index = index + inc
  end
  repeat_change()
end

function boss_restore_floor(with_arrows)

  -- restore the whole floor immediately
  sol.map.tile_set_group_enabled("boss_floor", true)
  sol.main.timer_stop_all()

  if with_arrows then
    for i, v in ipairs(boss_arrows) do
      if not v.created then
	sol.map.pickable_item_create("arrow", 1, -1, v.x, v.y, 0)
	v.created = true
      end
    end
  end
end

function event_enemy_dying(enemy_name)

  if enemy_name == "boss" then
    boss_restore_floor(false)
    sol.map.sensor_set_group_enabled("boss_floor_sensor", false)
  end
end

function event_enemy_dead(enemy_name)

  if enemy_name == "boss" then
    -- create the heart container manually to be sure it won't be in lava
    sol.map.pickable_item_create("heart_container", 1, 103, 960, 437, 0)
    sol.main.timer_stop_all()
  end
end

