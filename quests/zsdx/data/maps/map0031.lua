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
  end
end

function start_boss()

  fighting_boss = true
  sol.map.enemy_set_enabled("boss", true)
  sol.map.door_close("boss_door")
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

  sol.map.door_open("final_room_door")
  sol.main.play_sound("secret")
  sol.map.hero_unfreeze()
end

