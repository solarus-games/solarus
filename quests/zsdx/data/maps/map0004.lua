local map = ...
-- Outside world B3

-- Function called when the map starts
function map:on_started(destination_point_name)

  -- game ending sequence
  if destination_point_name == "from_ending" then
    map:hero_freeze()
    map:hero_set_visible(false)
    map:get_game():set_hud_enabled(false)
    map:enemy_set_group_enabled("", false)
    sol.audio.play_music("fanfare")
    map:tile_set_group_enabled("broken_rupee_house", false)
  else
    -- enable dark world
    if map:get_game():get_boolean(905) then
      sol.audio.play_music("dark_world")
      map:tileset_set(13)
    else
      sol.audio.play_music("overworld")
    end

    -- broken rupee house
    if map:get_game():get_boolean(155) then
      map:teletransporter_set_enabled("to_rupee_house", false)
      map:tile_set_enabled("rupee_house_door", false)
    else
      map:teletransporter_set_enabled("to_broken_rupee_house", false)
      map:tile_set_group_enabled("broken_rupee_house", false)
    end
  end

  local m = sol.movement.create("random_path")
  m:set_speed(32)
  map:npc_start_movement("chignon_woman", m)
  map:npc_get_sprite("chignon_woman"):set_animation("walking")

  -- remove Tom's cave door if open
  if map:get_game():get_boolean(36) then
    remove_village_cave_door()
  end

  -- remove the stone lock if open
  if map:get_game():get_boolean(159) then
    remove_stone_lock()
  end

  -- NPC
  if map:get_game():is_dungeon_finished(1) then
    map:npc_remove("cliff_man")
  end
end

function map:on_map_opening_transition_finished(destination_point_name)

  if destination_point_name == "from_ending" then
    map:dialog_start("credits_2")
    map:camera_move(184, 80, 25, function() end, 1e6)
  end
end

-- Function called when the player presses the action key
-- while facing an interactive entity
function map:on_npc_interaction(npc_name)

  if npc_name == "tom_cave_door" then

    -- open the door if the player has the clay key
    if map:get_game():has_item("clay_key") then
      sol.audio.play_sound("door_open")
      sol.audio.play_sound("secret")
      map:get_game():set_boolean(36, true)
      remove_village_cave_door()
    else
      map:dialog_start("outside_world.village.clay_key_required")
    end

  elseif npc_name == "stone_lock" then

    -- open the door if the player has the stone key
    if map:get_game():has_item("stone_key") then
      sol.audio.play_sound("door_open")
      sol.audio.play_sound("secret")
      map:get_game():set_boolean(159, true)
      remove_stone_lock()
    else
      map:dialog_start("outside_world.stone_key_required")
    end

  elseif npc_name == "chignon_woman" then

    if map:get_game():is_dungeon_finished(2) then
      map:dialog_start("outside_world.village.chignon_woman_dungeons")
    else
      map:dialog_start("outside_world.village.chignon_woman")
    end
  end
end

function remove_village_cave_door()
  map:npc_remove("tom_cave_door")
  map:tile_set_enabled("tom_cave_door_tile", false)
end

function remove_stone_lock()
  map:npc_remove("stone_lock")
  map:tile_set_group_enabled("stone_lock_tile", false)
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "waterfall_sensor" then
    map:hero_start_jumping(6, 288, true)
    sol.audio.play_sound("jump")
  end
end

function map:on_hero_still_on_sensor(sensor_name)

  -- entrances of houses
  local entrances = {
    "rupee_house", "lyly"
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

function map:on_dialog_finished(dialog_id)

  if dialog_id == "credits_2" then
   sol.timer.start(2000, ending_next)
  end
end

function ending_next()
  map:hero_set_map(56, "from_ending", 1)
end

