-- Outside world A3

monkey_sprite = nil
monkey_jumps = 0
monkey_jump_speed = 48

-- Function called when the map starts.
-- The NPCs are initialized.
function event_map_started(destination_point_name)

  -- enable dark world
  if sol.game.savegame_get_boolean(905) then
    sol.main.play_music("dark_world.spc")
    sol.map.tileset_set(13)
  end

  if sol.game.savegame_get_boolean(24) then
    -- remove the monkey from Link's house entrance
    sol.map.npc_remove("monkey")
  else
    monkey_sprite = sol.map.npc_get_sprite("monkey")
  end

  if sol.game.savegame_get_boolean(89) then
    -- remove the dungeon 2 door
    remove_dungeon_2_door()
  end

  -- make the NPCs walk
  random_walk("hat_man")
  random_walk("how_to_save_npc")
  random_walk("grand_son")

  -- smith cave with thiefs
  if sol.game.savegame_get_boolean(155) and not sol.game.savegame_get_boolean(156) then
    sol.map.teletransporter_set_enabled("to_smith_cave", false)
  else
    sol.map.teletransporter_set_enabled("to_smith_cave_thiefs", false)
  end
end

function random_walk(npc_name)

  local m = sol.main.random_path_movement_create(32)
  sol.map.npc_start_movement(npc_name, m)
  sol.main.sprite_set_animation(sol.map.npc_get_sprite(npc_name), "walking")
end

-- Function called when the player wants to talk to a non-playing character.
-- If the NPC is the monkey, then the monkey sound is played and the dialog starts.
function event_npc_interaction(npc_name)

  if string.find(npc_name, "monkey") then

    if not sol.game.savegame_get_boolean(24) then
      -- monkey first dialog
      sol.main.play_sound("monkey")
      sol.map.dialog_start("outside_world.village.monkey")
    else
      sol.main.play_sound("monkey")
      sol.map.dialog_start("outside_world.dungeon_2_entrance.monkey")
    end
  
  elseif npc_name == "dungeon_2_door" then

    -- open the door if the player has the Rock Key
    if sol.game.has_item("rock_key") then
      sol.main.play_sound("door_open")
      sol.main.play_sound("secret")
      sol.game.savegame_set_boolean(89, true)
      remove_dungeon_2_door()
    else
      sol.map.dialog_start("outside_world.rock_key_required")
    end
  end
end

-- Function called when the dialog box is being closed.
-- If the player was talking to the monkey, we do the appropriate action
function event_dialog_finished(first_message_id, answer)

  if first_message_id == "outside_world.village.monkey" then

    -- show another message depending on the shield
    if sol.game.has_ability("shield") then
      sol.map.dialog_start("outside_world.village.monkey.with_shield")
    else
      sol.map.dialog_start("outside_world.village.monkey.without_shield")
     end

  elseif first_message_id == "outside_world.village.monkey.with_shield" then
    -- make the monkey leave
    sol.map.hero_freeze()
    sol.main.play_sound("monkey")
    local m = sol.main.jump_movement_create(1, 24)
    sol.main.movement_set_property(m, "ignore_obstacles", true)
    sol.main.movement_set_property(m, "speed", monkey_jump_speed)
    sol.map.npc_start_movement("monkey", m)
    sol.main.sprite_set_animation(monkey_sprite, "jumping")
    monkey_jumps = 1
    sol.game.savegame_set_boolean(24, true)

  elseif first_message_id == "outside_world.village.tree_woman" then
    sol.map.treasure_give("rupee", 1, -1)
  end
end

-- Function called when the monkey has finished jumping
function event_npc_movement_finished(npc_name)

  if monkey_jumps == 1 then
    -- first jump finished: wait a little amount of time before jumping again
    sol.main.timer_start(monkey_timer, 300)
  elseif monkey_jumps == 2 then
    -- second jump finished: start the last jump
    sol.main.play_sound("monkey")
    local m = sol.main.jump_movement_create(1, 64)
    sol.main.movement_set_property(m, "ignore_obstacles", true)
    sol.main.movement_set_property(m, "speed", monkey_jump_speed)
    sol.map.npc_start_movement("monkey", m)
    sol.main.sprite_set_animation(monkey_sprite, "jumping")
    monkey_jumps = 3
  else
    -- last jump finished: remove the monkey from the map and unfreeze the hero
    sol.map.npc_remove("monkey")
    sol.map.hero_unfreeze()
  end
end

-- Function called when the monkey timer expires.
function monkey_timer()
  -- start the second jump
  sol.main.play_sound("monkey")
  local m = sol.main.jump_movement_create(2, 56)
  sol.main.movement_set_property(m, "ignore_obstacles", true)
  sol.main.movement_set_property(m, "speed", monkey_jump_speed)
  sol.map.npc_start_movement("monkey", m)
  sol.main.sprite_set_animation(monkey_sprite, "jumping")
  monkey_jumps = 2
end

function event_door_open(door_name)

  if door_name == "dungeon_3_entrance_weak_block" then
    sol.main.play_sound("secret") -- play the sound only once
  end
end

function remove_dungeon_2_door()
  sol.map.npc_remove("dungeon_2_door")
  sol.map.tile_set_enabled("dungeon_2_door_tile", false)
end

function event_hero_still_on_sensor(sensor_name)

  -- entrances of houses
  local entrances = {
    "sahasrahla", "shop", "cake_shop", "hero", "telepathic_booth"
  }
  for i = 1, #entrances do
    if sensor_name == entrances[i] .. "_door_sensor" then
      if sol.map.hero_get_direction() == 1
          and sol.map.tile_is_enabled(entrances[i] .. "_door") then
        sol.map.tile_set_enabled(entrances[i] .. "_door", false)
        sol.main.play_sound("door_open")
      end
      break
    end
  end
end

