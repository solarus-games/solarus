-- Outside world B2

function event_map_started(destination_point_name)

  -- enable dark world
  if sol.game.savegame_get_boolean(905) then
    sol.main.play_music("dark_world.spc")
    sol.map.tileset_set(13)
  end

  -- remove the iron lock if open
  if sol.game.savegame_get_boolean(193) then
    remove_iron_lock()
  end

  -- remove the wooden lock if open
  if sol.game.savegame_get_boolean(194) then
    remove_wooden_lock()
  end

  -- dungeon 6 entrance
  if sol.game.savegame_get_boolean(914) then
    inferno_set_open()
  else
    sol.map.teletransporter_set_enabled("to_dungeon_6", false)
  end
  sol.map.sensor_set_enabled("inferno_sensor", false)
end

function remove_iron_lock()
  sol.map.interactive_entity_remove("iron_lock")
  sol.map.tile_set_group_enabled("iron_lock_tile", false)
end

function remove_wooden_lock()
  sol.map.interactive_entity_remove("wooden_lock")
  sol.map.tile_set_group_enabled("wooden_lock_tile", false)
end


function event_hero_interaction(entity_name)

  if entity_name == "iron_lock" then

    -- open the door if the player has the iron key
    if sol.game.has_item("iron_key") then
      sol.main.play_sound("door_open")
      sol.main.play_sound("secret")
      sol.game.savegame_set_boolean(193, true)
      remove_iron_lock()
    else
      sol.map.dialog_start("outside_world.iron_key_required")
    end
  elseif entity_name == "wooden_lock" then

    -- open the door if the player has the wooden key
    if sol.game.has_item("wooden_key") then
      sol.main.play_sound("door_open")
      sol.main.play_sound("secret")
      sol.game.savegame_set_boolean(194, true)
      remove_wooden_lock()
    else
      sol.map.dialog_start("outside_world.wooden_key_required")
    end
  end
end

function event_npc_dialog(npc_name)

  if npc_name == "inferno" then
    inferno_open()
  end
end

function inferno_open()

  sol.map.sensor_set_enabled("inferno_sensor", true)
  sol.map.hero_walk("66", false, false)
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "inferno_sensor" then
    local sprite = sol.map.npc_get_sprite("inferno")
    sol.main.sprite_set_animation(sprite, "opening")
    sol.main.timer_start(1200, "inferno_open_finish")
    sol.map.hero_freeze()
    sol.map.hero_set_direction(1)
    sol.map.sensor_set_enabled("inferno_sensor", false)
  end
end

function inferno_open_finish()

  sol.main.play_sound("secret")
  sol.map.hero_unfreeze()
  sol.game.savegame_set_boolean(914, true)
  inferno_set_open()
end

function inferno_set_open()

  local sprite = sol.map.npc_get_sprite("inferno")
  sol.main.sprite_set_animation(sprite, "open")
  sol.map.teletransporter_set_enabled("to_dungeon_6", true)
end

