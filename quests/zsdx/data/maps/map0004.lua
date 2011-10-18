-- Outside world B3

-- Function called when the map starts
function event_map_started(destination_point_name)

  -- enable dark world
  if sol.game.savegame_get_boolean(905) then
    sol.main.play_music("dark_world.spc")
    sol.map.tileset_set(13)
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

  -- broken rupee house
  if sol.game.savegame_get_boolean(155) then
    sol.map.teletransporter_set_enabled("to_rupee_house", false)
  else
    sol.map.teletransporter_set_enabled("to_broken_rupee_house", false)
    sol.map.tile_set_group_enabled("broken_rupee_house", false)
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

  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "waterfall_sensor" then
    sol.map.hero_start_jumping(6, 288, true)
    sol.main.play_sound("jump")
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

