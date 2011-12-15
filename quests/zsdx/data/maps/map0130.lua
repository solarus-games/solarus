-- Dungeon 9 boss

local torches_error = false
local torches_next = nil
local torches_nb_on = 0
local torches_delay = 20000
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
  { x = 192, y = 325 },
  { x = 288, y = 325 },
  { x = 160, y = 293 },
  { x = 192, y = 165 },
  { x = 288, y = 165 },
  { x = 160, y = 197 },
  { x = 320, y = 197 },
  { x = 320, y = 293 },
}
local nb_bats_created = 0
local bonuses_done = {}

function event_map_started(destination_point_name)

  if not sol.game.savegame_get_boolean(881) then
    sol.main.play_music("ganon_appears.spc")
    sol.map.enemy_set_enabled("boss", true)
    sol.map.npc_set_enabled("zelda", false)
    sol.map.npc_set_group_enabled("child", false)
    sol.map.hero_save_solid_ground()
  end

  sol.map.switch_set_group_enabled("switch", false)
  sol.map.tile_set_group_enabled("switch_floor", false)
end

function event_map_opening_transition_finished(destination_point_name)

  if destination_point_name == "from_6f" then
    if not sol.game.savegame_get_boolean(881) then
      sol.map.dialog_start("dungeon_9.boss")
    else
      start_zelda_sequence()
    end
  end
end

function event_dialog_finished(first_message_id)

  if first_message_id == "dungeon_9.boss" then
    sol.main.play_music("ganon_battle.spc")
  elseif first_message_id == "dungeon_9.zelda" then
    sol.main.timer_start(function()
      sol.map.dialog_start("dungeon_9.zelda_children")
    end, 1000)
  elseif first_message_id == "dungeon_9.zelda_children" then
    sol.main.play_music("none")
    sol.main.play_sound("world_warp")
    sol.main.timer_start(function()
      for i = 1, 8 do
	sol.main.sprite_fade(sol.map.npc_get_sprite("child_" .. i), 1)
      end
    end, 1000)
    sol.main.timer_start(function()
      sol.map.dialog_start("dungeon_9.zelda_end")
    end, 5000)
  elseif first_message_id == "dungeon_9.zelda_end" then
    sol.main.timer_start(function()
      sol.map.hero_set_map(8, "from_ending", 1)
    end, 2000)
  end
end

function event_enemy_dead(enemy_name)

  if enemy_name == "boss" then
    sol.main.timer_start(start_final_sequence, 1000)
  end
end

function start_final_sequence()

  sol.map.hero_freeze()
  sol.map.hero_set_direction(3)
  sol.main.play_music("victory.spc")
  sol.main.timer_start(function()
    sol.map.hero_set_map(130, "from_boss", 1)
  end, 9000)
  sol.main.timer_start(start_zelda_sequence, 9100)
end

function start_zelda_sequence()

  sol.main.play_music("triforce.spc")
  sol.map.hero_freeze()
  sol.map.hero_set_direction(1)
  sol.map.npc_set_enabled("zelda", true)
  for i = 1, 8 do
    local npc_name = "child_" .. i
    sol.map.npc_set_enabled(npc_name, true)
    local sprite = sol.map.npc_get_sprite(npc_name)
    sol.main.sprite_set_animation_ignore_suspend(sprite, true)
    sol.main.sprite_fade(sprite, 0)
  end

  sol.main.timer_start(function()
    sol.map.dialog_start("dungeon_9.zelda")
    sol.map.dialog_set_variable("dungeon_9.zelda", sol.game.savegame_get_name())
  end, 3000)
end

-- Torches on this map interact with the map script
-- because we don't want usual behavior from items/lamp.lua:
-- we want a longer delay and special Ganon interaction 
function event_npc_interaction(npc_name)

  if string.find(npc_name, "^torch") then
    sol.map.dialog_start("torch.need_lamp")
  end
end

-- Called when fire touches an NPC linked to this map
function event_npc_collision_fire(npc_name)

  if string.find(npc_name, "^torch") then
    
    local torch_sprite = sol.map.npc_get_sprite(npc_name)
    if sol.main.sprite_get_animation(torch_sprite) == "unlit" then
      -- temporarily light the torch up
      sol.main.sprite_set_animation(torch_sprite, "lit")
      check_torches()
      sol.main.timer_start(function()
        sol.main.sprite_set_animation(torch_sprite, "unlit")
	if sol.map.switch_is_enabled("switch_1") then
	  sol.map.tile_set_group_enabled("switch_floor", false)
	  sol.map.switch_set_group_enabled("switch", false)
	  sol.main.play_sound("door_closed")
	end
        check_torches()
      end, torches_delay)
    end
  end
end

function unlight_torches()

  for i = 1, 4 do
    sol.main.sprite_set_animation(sol.map.npc_get_sprite("torch_" .. i), "unlit")
  end
  sol.main.timer_stop_all()
end

function check_torches()
  
  local states = {
    sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch_1")) == "lit",
    sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch_2")) == "lit",
    sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch_3")) == "lit",
    sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch_4")) == "lit"
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
      sol.main.play_sound("wrong")
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
  local boss_x, boss_y = sol.map.enemy_get_position("boss")
  if boss_x < 240 then
    x = 280
  else
    x = 200
  end
  local hero_x, hero_y = sol.map.hero_get_position()
  if hero_y < 240 then
    y = 285
  else
    y = 205
  end

  sol.map.destructible_item_create(4, x, y, 0, "_none", 1, -1)
end

function torches_solved()

  if sol.map.tile_is_enabled("floor_down_1") then
    -- phase 1
    sol.main.play_sound("secret")
    create_stone()
  else
    -- phase 2
    sol.main.play_sound("secret")
    sol.main.play_sound("door_open")
    sol.map.tile_set_group_enabled("switch_floor", true)
    sol.map.switch_set_group_enabled("switch", true)
    for i = 1, 4 do
      sol.map.switch_set_activated("switch_" .. i, false)
      bonuses_done[i] = nil
    end
  end
end

function event_switch_activated(switch_name)

  -- deterministic verion: local index = tonumber(switch_name:match("^switch_([1-4])$"))

  local index
  repeat
    index = math.random(4)
  until bonuses_done[index] == nil
  bonuses_done[index] = true

  if index == 1 then
    -- kill small enemies
    if sol.map.enemy_get_group_count("boss_") > 0 then
      sol.main.play_sound("enemy_killed")
      sol.map.enemy_remove_group("boss_")
    end

  elseif index == 2 then
    -- create the stone that makes Ganon vulnerable
    sol.main.play_sound("secret")
    create_stone()

  elseif index == 3 then
    -- create pickable items
    sol.main.play_sound("secret")
    create_pickables()

  else
    sol.main.play_sound("wrong")
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
    sol.map.pickable_item_create(item_name, variant, -1, v.x, v.y, 0)
  end
end

function create_bats()

  for i, v in ipairs(bats) do
    nb_bats_created = nb_bats_created + 1
    sol.map.enemy_create("bat_" .. nb_bats_created, "fire_bat", 0, v.x, v.y)
  end
end

