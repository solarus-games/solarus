local map = ...
-- Dungeon 9 boss

local torches_error = false
local torches_next = nil
local torches_nb_on = 0
local torches_delay = 20000
local torches_timers = {}
local allow_stone_creation = true
local pickables = {
  { x =  88, y = 141 },
  { x = 136, y = 93 },
  { x = 144, y = 149 },
  { x = 344, y = 93 },
  { x = 392, y = 141 },
  { x = 336, y = 149 },
  { x = 392, y = 349 },
  { x = 344, y = 397 },
  { x = 336, y = 341 },
  { x = 136, y = 397 },
  { x =  88, y = 349 },
  { x = 144, y = 341 },
--[[  { x = 192, y = 325 },
  { x = 288, y = 325 },
  { x = 160, y = 293 },
  { x = 192, y = 165 },
  { x = 288, y = 165 },
  { x = 160, y = 197 },
  { x = 320, y = 197 },
  { x = 320, y = 293 }, --]]
}
local bats = {
  { x =  88, y = 141 },
  { x = 144, y = 149 },
  { x = 392, y = 141 },
  { x = 392, y = 349 },
  { x = 336, y = 341 },
  { x =  88, y = 349 },
  { x = 192, y = 325 },
  { x = 160, y = 293 },
  { x = 288, y = 165 },
  { x = 320, y = 197 },
}
local nb_bats_created = 0
local bonuses_done = {}

function map:on_started(destination_point_name)

  if not map:get_game():get_boolean(881) then
    sol.audio.play_music("ganon_appears")
    map:enemy_set_enabled("boss", true)
    map:npc_set_enabled("zelda", false)
    map:npc_set_group_enabled("child", false)
    map:hero_save_solid_ground()
  end

  map:switch_set_group_enabled("switch", false)
  map:tile_set_group_enabled("switch_floor", false)
end

function map:on_map_opening_transition_finished(destination_point_name)

  if destination_point_name == "from_6f" then
    if not map:get_game():get_boolean(881) then
      map:start_dialog("dungeon_9.boss")
    else
      start_zelda_sequence()
    end
  end
end

function map:on_dialog_finished(dialog_id)

  if dialog_id == "dungeon_9.boss" then
    sol.audio.play_music("ganon_battle")
  elseif dialog_id == "dungeon_9.zelda" then
    sol.timer.start(1000, function()
      map:start_dialog("dungeon_9.zelda_children")
    end)
  elseif dialog_id == "dungeon_9.zelda_children" then
    sol.audio.stop_music()
    sol.audio.play_sound("world_warp")
    sol.timer.start(1000, function()
      for i = 1, 8 do
	map:npc_get_sprite("child_" .. i):fade_out()
      end
    end)
    sol.timer.start(5000, function()
      map:start_dialog("dungeon_9.zelda_end")
    end)
  elseif dialog_id == "dungeon_9.zelda_end" then
    sol.timer.start(2000, function()
      map:hero_set_map(8, "from_ending", 1)
    end)
  end
end

function map:on_enemy_dead(enemy_name)

  if enemy_name == "boss" then
    sol.timer.start(1000, start_final_sequence)
  end
end

function start_final_sequence()

  map:hero_freeze()
  map:hero_set_direction(3)
  sol.audio.play_music("victory")
  sol.timer.start(9000, function()
    map:hero_set_map(130, "from_boss", 1)
  end)
  sol.timer.start(9100, start_zelda_sequence)
end

function start_zelda_sequence()

  sol.audio.play_music("triforce")
  map:hero_freeze()
  map:hero_set_direction(1)
  map:npc_set_enabled("zelda", true)
  for i = 1, 8 do
    local npc_name = "child_" .. i
    map:npc_set_enabled(npc_name, true)
    local sprite = map:npc_get_sprite(npc_name)
    sprite:set_ignore_suspend(true)
    sprite:fade_in()
  end

  sol.timer.start(3000, function()
    map:start_dialog("dungeon_9.zelda")
    map:set_dialog_variable("dungeon_9.zelda", map:get_game():get_player_name())
  end)
end

-- Torches on this map interact with the map script
-- because we don't want usual behavior from items/lamp.lua:
-- we want a longer delay and special Ganon interaction 
function map:on_npc_interaction(npc_name)

  if string.find(npc_name, "^torch") then
    map:start_dialog("torch.need_lamp")
  end
end

-- Called when fire touches an NPC linked to this map
function map:on_npc_collision_fire(npc_name)

  if string.find(npc_name, "^torch") then
    
    local torch_sprite = map:npc_get_sprite(npc_name)
    if torch_sprite:get_animation() == "unlit" then
      -- temporarily light the torch up
      torch_sprite:set_animation("lit")
      check_torches()
      torches_timers[npc_name] = sol.timer.start(torches_delay, function()
        torch_sprite:set_animation("unlit")
	if map:switch_is_enabled("switch_1") then
	  map:tile_set_group_enabled("switch_floor", false)
	  map:switch_set_group_enabled("switch", false)
	  sol.audio.play_sound("door_closed")
	end
        check_torches()
      end)
    end
  end
end

function unlight_torches()

  for i = 1, 4 do
    map:npc_get_sprite("torch_" .. i):set_animation("unlit")
  end
  for _, t in ipairs(torches_timers) do t:stop() end
end

function check_torches()
  
  local states = {
    map:npc_get_sprite("torch_1"):get_animation() == "lit",
    map:npc_get_sprite("torch_2"):get_animation() == "lit",
    map:npc_get_sprite("torch_3"):get_animation() == "lit",
    map:npc_get_sprite("torch_4"):get_animation() == "lit"
  }
  local on = {}

  for i = 1, #states do
    if states[i] then
      on[#on + 1] = i
    end
  end

  if #on == torches_nb_on then
    -- no change
    return
  end

  --print("torches on:", #on)

  if #on == #states then
   -- all torches are on
    if torches_error then
      sol.audio.play_sound("wrong")
      torches_error = false
      torches_next = nil
      torches_nb_on = 0
      unlight_torches()
      --print("wrong")
    else
      torches_solved()
      torches_next = on[1] % #states + 1
    end

  elseif #on == 0 then
    -- no torch is on
    torches_error = false
    torches_next = nil
    --print("no torch is on")

  elseif #on == 1 then
    --print("a first torch is on: ", on[1])
    torches_error = false
    torches_next = on[1] % #states + 1
    --print("next should be ", torches_next)
      
  elseif not torches_error then

    if #on == torches_nb_on + 1 then
      -- a torch was just turned on
      if states[torches_next] then
        -- it's the correct one
        torches_next = torches_next % #states + 1
        --print("another torch is on, it's the correct one")
	--print("next should be ", torches_next)
      else
	torches_error = true
        --print("another torch is on, it's a wrong one")
      end
    end
  end

  torches_nb_on = #on
end

-- Creates a stone that the hero can lift and throw to Ganon.
function create_stone()

  -- we have to check the position of Ganon and the hero
  local x, y
  local boss_x, boss_y = map:enemy_get_position("boss")
  if boss_x < 240 then
    x = 280
  else
    x = 200
  end
  local hero_x, hero_y = map:hero_get_position()
  if hero_y < 240 then
    y = 285
  else
    y = 205
  end

  map:destructible_create("black_stone", x, y, 0, {
    treasure_item = "_none",
    destruction_callback = on_stone_destroyed})
  allow_stone_creation = false
end

function on_stone_destroyed()

  allow_stone_creation = true
end

function torches_solved()

  if map:tile_is_enabled("floor_down_1") then
    -- phase 1
    if allow_stone_creation then
      sol.audio.play_sound("secret")
      create_stone()
    end
  else
    -- phase 2
    sol.audio.play_sound("secret")
    sol.audio.play_sound("door_open")
    map:tile_set_group_enabled("switch_floor", true)
    map:switch_set_group_enabled("switch", true)
    for i = 1, 4 do
      map:switch_set_activated("switch_" .. i, false)
      bonuses_done[i] = nil
    end
  end
end

function map:on_switch_activated(switch_name)

  -- deterministic verion: local index = tonumber(switch_name:match("^switch_([1-4])$"))

  local index
  repeat
    index = math.random(4)
  until bonuses_done[index] == nil
  bonuses_done[index] = true

  if index == 1 then
    -- kill small enemies
    if map:enemy_get_group_count("boss_") > 0 then
      sol.audio.play_sound("enemy_killed")
      map:enemy_remove_group("boss_")
    end

  elseif index == 2 then
    -- create the stone that makes Ganon vulnerable
    if allow_stone_creation then
      sol.audio.play_sound("secret")
      create_stone()
    end

  elseif index == 3 then
    -- create pickable items
    sol.audio.play_sound("secret")
    create_pickables()

  else
    sol.audio.play_sound("wrong")
    create_bats()
  end
end

function create_pickables()

  for i, v in ipairs(pickables) do

    local i = math.random(100)
    if i <= 60 then
      item_name = "magic_flask"
      variant = 1
    elseif i <= 90 then
      item_name = "heart"
      variant = 1
    elseif i <= 95 then
      item_name = "magic_flask"
      variant = 2
    elseif i <= 99 then
      item_name = "arrow"
      variant = 2
    else
      item_name = "fairy"
      variant = 1
    end
    map:pickable_create(item_name, variant, -1, v.x, v.y, 0)
  end
end

function create_bats()

  for i, v in ipairs(bats) do
    nb_bats_created = nb_bats_created + 1
    map:enemy_create("bat_" .. nb_bats_created, "fire_bat", 0, v.x, v.y)
  end
end

