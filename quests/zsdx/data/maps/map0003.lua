local map = ...
-- Outside world A3

monkey_sprite = nil
monkey_jumps = 0
monkey_jump_speed = 48

-- Function called when the map starts.
-- The NPCs are initialized.
function map:on_started(destination_point_name)

  -- enable dark world
  if map:get_game():get_boolean(905) then
    sol.audio.play_music("dark_world")
    map:tileset_set(13)
  else
    sol.audio.play_music("overworld")
  end

  if map:get_game():get_boolean(24) then
    -- remove the monkey from Link's house entrance
    map:npc_remove("monkey")
  else
    monkey_sprite = map:npc_get_sprite("monkey")
  end

  if map:get_game():get_boolean(89) then
    -- remove the dungeon 2 door
    remove_dungeon_2_door()
  end

  -- initialize NPCs
  random_walk("hat_man")
  random_walk("grand_son")

  if map:get_game():is_dungeon_finished(1) then
    map:npc_remove("how_to_save_npc")
  else
    random_walk("how_to_save_npc")
  end

  -- smith cave with thiefs
  if map:get_game():get_boolean(155) and not map:get_game():get_boolean(156) then
    map:teletransporter_set_enabled("to_smith_cave", false)
  else
    map:teletransporter_set_enabled("to_smith_cave_thiefs", false)
  end
end

function random_walk(npc_name)

  local m = sol.movement.create("random_path")
  m:set_speed(32)
  map:npc_start_movement(npc_name, m)
  map:npc_get_sprite(npc_name):set_animation("walking")
end

-- Function called when the player wants to talk to a non-playing character.
-- If the NPC is the monkey, then the monkey sound is played and the dialog starts.
function map:on_npc_interaction(npc_name)

  if string.find(npc_name, "monkey") then

    if not map:get_game():get_boolean(24) then
      -- monkey first dialog
      sol.audio.play_sound("monkey")
      map:dialog_start("outside_world.village.monkey")
    else
      sol.audio.play_sound("monkey")
      map:dialog_start("outside_world.dungeon_2_entrance.monkey")
    end
  
  elseif npc_name == "dungeon_2_door" then

    -- open the door if the player has the Rock Key
    if map:get_game():has_item("rock_key") then
      sol.audio.play_sound("door_open")
      sol.audio.play_sound("secret")
      map:get_game():set_boolean(89, true)
      remove_dungeon_2_door()
    else
      map:dialog_start("outside_world.rock_key_required")
    end

  elseif npc_name == "hat_man" then

    if map:get_game():is_dungeon_finished(1) then
      map:dialog_start("outside_world.village.hat_man_npc_waterfall")
    else
      map:dialog_start("outside_world.village.hat_man_npc")
    end
  end
end

-- Function called when the dialog box is being closed.
-- If the player was talking to the monkey, we do the appropriate action
function map:on_dialog_finished(dialog_id, answer)

  if dialog_id == "outside_world.village.monkey" then

    -- show another message depending on the shield
    if map:get_game():has_ability("shield") then
      map:dialog_start("outside_world.village.monkey.with_shield")
    else
      map:dialog_start("outside_world.village.monkey.without_shield")
     end

  elseif dialog_id == "outside_world.village.monkey.with_shield" then
    -- make the monkey leave
    map:hero_freeze()
    sol.audio.play_sound("monkey")
    local m = sol.movement.create("jump")
    m:set_direction8(1)
    m:set_distance(24)
    m:set_ignore_obstacles(true)
    m:set_speed(monkey_jump_speed)
    map:npc_start_movement("monkey", m)
    monkey_sprite:set_animation("jumping")
    monkey_jumps = 1
    map:get_game():set_boolean(24, true)

  elseif dialog_id == "outside_world.village.tree_woman" then
    map:treasure_give("rupee", 1, -1)
  end
end

-- Function called when the monkey has finished jumping
function map:on_npc_movement_finished(npc_name)

  if monkey_jumps == 1 then
    -- first jump finished: wait a little amount of time before jumping again
    sol.timer.start(300, monkey_timer)
  elseif monkey_jumps == 2 then
    -- second jump finished: start the last jump
    sol.audio.play_sound("monkey")
    local m = sol.movement.create("jump")
    m:set_direction8(1)
    m:set_distance(64)
    m:set_ignore_obstacles(true)
    m:set_speed(monkey_jump_speed)
    map:npc_start_movement("monkey", m)
    monkey_sprite:set_animation("jumping")
    monkey_jumps = 3
  else
    -- last jump finished: remove the monkey from the map and unfreeze the hero
    map:npc_remove("monkey")
    map:hero_unfreeze()
  end
end

-- Function called when the monkey timer expires.
function monkey_timer()
  -- start the second jump
  sol.audio.play_sound("monkey")
  local m = sol.movement.create("jump")
  m:set_direction8(2)
  m:set_distance(56)
  m:set_ignore_obstacles(true)
  m:set_speed(monkey_jump_speed)
  map:npc_start_movement("monkey", m)
  monkey_sprite:set_animation("jumping")
  monkey_jumps = 2
end

function map:on_door_open(door_name)

  if door_name == "dungeon_3_entrance_weak_block" then
    sol.audio.play_sound("secret") -- play the sound only once
  end
end

function remove_dungeon_2_door()
  map:npc_remove("dungeon_2_door")
  map:tile_set_enabled("dungeon_2_door_tile", false)
end

function map:on_hero_still_on_sensor(sensor_name)

  -- entrances of houses
  local entrances = {
    "sahasrahla", "shop", "cake_shop", "hero", "telepathic_booth"
  }
  for i = 1, #entrances do
    if sensor_name == entrances[i] .. "_door_sensor" then
      if map:hero_get_direction() == 1
          and map:tile_is_enabled(entrances[i] .. "_door") then
        map:tile_set_enabled(entrances[i] .. "_door", false)
        sol.audio.play_sound("door_open")
      end
      break
    end
  end
end

