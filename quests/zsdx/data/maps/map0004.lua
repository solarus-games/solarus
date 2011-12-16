-- Outside world B3

-- Function called when the map starts
function event_map_started(destination_point_name)

  -- game ending sequence
  if destination_point_name == "from_ending" then
    sol.map.hero_freeze()
    sol.map.hero_set_visible(false)
    sol.map.hud_set_enabled(false)
    sol.map.enemy_set_group_enabled("", false)
    sol.main.play_music("fanfare.spc")
    sol.map.tile_set_group_enabled("broken_rupee_house", false)
  else
    -- enable dark world
    if sol.game.savegame_get_boolean(905) then
      sol.main.play_music("dark_world.spc")
      sol.map.tileset_set(13)
    end

    -- broken rupee house
    if sol.game.savegame_get_boolean(155) then
      sol.map.teletransporter_set_enabled("to_rupee_house", false)
      sol.map.tile_set_enabled("rupee_house_door", false)
    else
      sol.map.teletransporter_set_enabled("to_broken_rupee_house", false)
      sol.map.tile_set_group_enabled("broken_rupee_house", false)
    end
  end

  local m = sol.main.random_path_movement_create(32)
  sol.map.npc_start_movement("chignon_woman", m)
  sol.main.sprite_set_animation(sol.map.npc_get_sprite("chignon_woman"), "walking")

  -- remove Tom's cave door if open
  if sol.game.savegame_get_boolean(36) then
    remove_village_cave_door()
  end

  -- remove the stone lock if open
  if sol.game.savegame_get_boolean(159) then
    remove_stone_lock()
  end

  -- NPC
  if sol.game.is_dungeon_finished(1) then
    sol.map.npc_remove("cliff_man")
  end
end

function event_map_opening_transition_finished(destination_point_name)

  if destination_point_name == "from_ending" then
    sol.map.dialog_start("credits_2")
    sol.map.camera_move(184, 80, 25, function() end, 1e6)
  end
end

-- Function called when the player presses the action key
-- while facing an interactive entity
function event_npc_interaction(npc_name)

  if npc_name == "tom_cave_door" then

    -- open the door if the player has the clay key
    if sol.game.has_item("clay_key") then
      sol.main.play_sound("door_open")
      sol.main.play_sound("secret")
      sol.game.savegame_set_boolean(36, true)
      remove_village_cave_door()
    else
      sol.map.dialog_start("outside_world.village.clay_key_required")
    end

  elseif npc_name == "stone_lock" then

    -- open the door if the player has the stone key
    if sol.game.has_item("stone_key") then
      sol.main.play_sound("door_open")
      sol.main.play_sound("secret")
      sol.game.savegame_set_boolean(159, true)
      remove_stone_lock()
    else
      sol.map.dialog_start("outside_world.stone_key_required")
    end

  elseif npc_name == "chignon_woman" then

    if sol.game.is_dungeon_finished(2) then
      sol.map.dialog_start("outside_world.village.chignon_woman_dungeons")
    else
      sol.map.dialog_start("outside_world.village.chignon_woman")
    end
  end
end

function remove_village_cave_door()
  sol.map.npc_remove("tom_cave_door")
  sol.map.tile_set_enabled("tom_cave_door_tile", false)
end

function remove_stone_lock()
  sol.map.npc_remove("stone_lock")
  sol.map.tile_set_group_enabled("stone_lock_tile", false)
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "waterfall_sensor" then
    sol.map.hero_start_jumping(6, 288, true)
    sol.main.play_sound("jump")
  end
end

function event_hero_still_on_sensor(sensor_name)

  -- entrances of houses
  local entrances = {
    "rupee_house", "lyly"
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

function event_dialog_finished(first_message_id)

  if first_message_id == "credits_2" then
   sol.main.timer_start(ending_next, 2000)
  end
end

function ending_next()
  sol.map.hero_set_map(56, "from_ending", 1)
end

