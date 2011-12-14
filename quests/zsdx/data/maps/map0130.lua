-- Dungeon 9 boss

local torches_error = false
local torches_next = nil
local torches_nb_on = 0
local torches_delay = 15000

function event_map_started(destination_point_name)

  if not sol.game.savegame_get_boolean(881) then
    sol.main.play_music("ganon_appears.spc")
    sol.map.enemy_set_enabled("boss", true)
    sol.map.npc_set_enabled("zelda", false)
    sol.map.npc_set_group_enabled("child", false)
  end

  sol.map.switch_set_group_enabled("switch", false)
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
        check_torches()
      end, torches_delay)
    end
  end
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
      -- TODO
      sol.main.play_sound("wrong")
      --print("wrong")
    else
      -- TODO
      sol.main.play_sound("secret")
      --print("correct")
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

