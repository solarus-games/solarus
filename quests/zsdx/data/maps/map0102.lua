-- Dungeon 7 2F

fighting_miniboss = false
code_nb_activated = 0
code_next_index = 1

function event_map_started(destination_point_name)

  -- pipe maze
  sol.map.obstacle_set_group_enabled("pipe_border", false)

  -- door of the pots and pikes
  sol.map.door_set_open("door_f", true)
  sol.map.switch_set_activated("door_f_switch", true)

  -- saved door D
  if sol.game.savegame_get_boolean(615) then
    sol.map.switch_set_activated("door_d_switch", true)
  end

  -- torches
  if sol.game.savegame_get_boolean(618) then
    lock_torches()
  end

  -- weak floor
  if sol.game.savegame_get_boolean(619) then
    sol.map.tile_set_group_enabled("weak_floor", false)
    sol.map.sensor_set_enabled("weak_floor_sensor", false)
  else
    sol.map.teletransporter_set_enabled("weak_floor_teletransporter", false)
  end

  -- miniboss
  sol.map.door_set_open("miniboss_door", true)
  sol.map.enemy_set_group_enabled("miniboss", false)

  -- save the north-west door from 1F
  if destination_point_name == "from_1f_ne" then 
    sol.game.savegame_set_boolean(621, true)
  end
end

function event_hero_on_sensor(sensor_name)

  -- close door F
  if sensor_name == "open_door_f_sensor" then
    sol.map.door_set_open("door_f", false)
    sol.map.switch_set_activated("door_f_switch", false)
 
  -- miniboss
  elseif sensor_name == "start_miniboss_sensor"
      and not sol.game.savegame_get_boolean(620)
      and not fighting_miniboss then

    sol.map.hero_freeze()
    sol.map.door_close("miniboss_door")
    fighting_miniboss = true
    sol.main.timer_start(function()
      sol.main.play_music("boss.spc")
      sol.map.enemy_set_group_enabled("miniboss", true)
      sol.map.hero_unfreeze()
    end,
    1000)

  -- pipes
  else
    pipe = string.match(sensor_name, "^pipe_in_([a-z])_sensor")
    if pipe ~= nil then
      -- entering a pipe
      sol.map.obstacle_set_group_enabled("pipe_border_"..pipe, true)
      sol.map.hero_set_visible(true)
    else
      pipe = string.match(sensor_name, "^pipe_out_([a-z])_sensor")
      if pipe ~= nil then
	-- leaving a pipe
	sol.map.obstacle_set_group_enabled("pipe_border_"..pipe, false)
      elseif string.find(sensor_name, "^hide_hero_sensor") then
	-- hide the hero
	sol.map.hero_set_visible(false)
      elseif string.find(sensor_name, "^unhide_hero_sensor") then
	-- unhide the hero
	sol.map.hero_set_visible(true)
      end
    end
  end
end

function event_switch_activated(switch_name)

  -- door F
  if switch_name == "door_f_switch" then
    sol.map.camera_move(1040, 760, 250, function()
      sol.main.play_sound("secret")
      sol.map.door_open("door_f")
    end)

  -- door A
  elseif switch_name == "door_a_switch" then
    sol.main.play_sound("secret")
    sol.map.door_open("door_a")

  -- code
  else
    local index = tonumber(string.match(switch_name, "^code_switch_([1-8])$"))
    if index ~= nil then
      if index == code_next_index then
	code_next_index = code_next_index + 1
      else
	code_next_index = 1
      end
      code_nb_activated = code_nb_activated + 1
      if code_nb_activated == 8 then
	-- the 8 switches are activated
        if code_next_index == 9 then
	  sol.map.camera_move(72, 552, 250, function()
	    sol.main.play_sound("secret")
	    sol.map.door_open("door_a")
	  end)
	else
	  sol.main.play_sound("wrong")
	  for i = 1, 8 do
	    sol.map.switch_set_activated("code_switch_" .. i, false)
	  end
	  code_nb_activated = 0

	  -- make sure the switch index won't get reactivated right now
	  sol.map.switch_set_locked(switch_name, true)
	  sol.main.timer_start(function()
	    sol.map.switch_set_locked(switch_name, false)
	  end, 500)
	end
      end
    end
  end
end

function event_door_open(door_name)

  if door_name == "door_b" then
    -- put the last enemy of the room on the hero's layer
    local enemy_name = "w_room_enemy_4"
    if not sol.map.enemy_is_dead(enemy_name) then
      local x, y = sol.map.enemy_get_position(enemy_name)
      sol.map.enemy_set_position(enemy_name, x, y, 1)
    end
  end
end

function event_enemy_dead(enemy_name)

  -- west enemy room
  if string.find(enemy_name, "^w_room_enemy")
    and sol.map.enemy_is_group_dead("w_room_enemy")
    and not sol.map.door_is_open("door_c") then
    sol.map.camera_move(264, 504, 250, function()
      sol.main.play_sound("secret")
      sol.map.door_open("door_c")
    end)

  -- miniboss
  elseif string.find(enemy_name, "^miniboss")
      and sol.map.enemy_is_group_dead("miniboss") then

    sol.main.play_music("dark_world_dungeon.spc")
    sol.map.door_open("miniboss_door")
    sol.game.savegame_set_boolean(620, true)
  end
end

-- Returns whether all torches are on
function are_all_torches_on()

  return sol.map.npc_exists("torch_1")
      and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch_1")) == "lit"
      and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch_2")) == "lit"
      and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch_3")) == "lit"
      and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch_4")) == "lit" 
end

-- Makes all torches on forever
function lock_torches()
  sol.map.npc_remove("torch_1")
  sol.map.npc_remove("torch_2")
  sol.map.npc_remove("torch_3")
  sol.map.npc_remove("torch_4")
end

function event_update()

  if not sol.game.savegame_get_boolean(618)
    and are_all_torches_on() then

    lock_torches()
    sol.map.camera_move(32, 120, 250, function()
      sol.main.play_sound("secret")
      sol.map.npc_set_position("nw_hint_stone", 32, 125)
    end)
  end
end

function event_sensor_collision_explosion(sensor_name)

  if sensor_name == "weak_floor_sensor"
      and sol.map.tile_is_enabled("weak_floor") then

    sol.map.tile_set_group_enabled("weak_floor", false)
    sol.map.sensor_set_enabled("weak_floor_sensor", false)
    sol.map.teletransporter_set_enabled("weak_floor_teletransporter", true)
    sol.main.play_sound("secret")
    sol.game.savegame_set_boolean(619, true)
  end
end

