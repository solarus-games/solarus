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

  -- Inferno
  if not sol.game.is_dungeon_finished(5) then
    sol.map.npc_remove("inferno")
  else
    local sprite = sol.map.npc_get_sprite("inferno")
    sol.main.sprite_set_animation_ignore_suspend(sprite, true)
    if sol.game.savegame_get_boolean(914) then
      inferno_set_open()
    end
  end
  if not sol.game.savegame_get_boolean(914) then
    sol.map.teletransporter_set_enabled("to_dungeon_6", false)
  end
  sol.map.sensor_set_enabled("inferno_sensor", false)
end

function remove_iron_lock()
  sol.map.npc_remove("iron_lock")
  sol.map.tile_set_group_enabled("iron_lock_tile", false)
end

function remove_wooden_lock()
  sol.map.npc_remove("wooden_lock")
  sol.map.tile_set_group_enabled("wooden_lock_tile", false)
end

function event_npc_interaction(npc_name)

  if npc_name == "iron_lock" then

    -- open the door if the player has the iron key
    if sol.game.has_item("iron_key") then
      sol.main.play_sound("door_open")
      sol.main.play_sound("secret")
      sol.game.savegame_set_boolean(193, true)
      remove_iron_lock()
    else
      sol.map.dialog_start("outside_world.iron_key_required")
    end

  elseif npc_name == "wooden_lock" then

    -- open the door if the player has the wooden key
    if sol.game.has_item("wooden_key") then
      sol.main.play_sound("door_open")
      sol.main.play_sound("secret")
      sol.game.savegame_set_boolean(194, true)
      remove_wooden_lock()
    else
      sol.map.dialog_start("outside_world.wooden_key_required")
    end

  elseif npc_name == "inferno" then

    if not sol.game.savegame_get_boolean(915) then
      -- first time
      sol.map.dialog_start("inferno.first_time")
      sol.game.savegame_set_boolean(915, true)
    elseif not sol.game.savegame_get_boolean(914) then
      -- not open yet
      if sol.game.get_item_amount("fire_stones_counter") < 3 then
        sol.map.dialog_start("inferno.find_fire_stones")
      else
        sol.map.dialog_start("inferno.found_fire_stones")
      end
    end
  end
end

function inferno_open()

  sol.map.sensor_set_enabled("inferno_sensor", true)
  sol.map.hero_walk("66", false, false)
end

function event_hero_on_sensor(sensor_name)

  -- Inferno
  if sensor_name == "inferno_sensor" then
    local sprite = sol.map.npc_get_sprite("inferno")
    sol.main.sprite_set_animation(sprite, "opening")
    sol.main.timer_start(inferno_open_finish, 1050)
    sol.map.hero_freeze()
    sol.map.hero_set_direction(1)
    sol.map.sensor_set_enabled("inferno_sensor", false)
  end
end

function event_hero_still_on_sensor(sensor_name)

  -- Witch hut entrance
  if sensor_name == "potion_shop_door_sensor" then
    if sol.map.hero_get_direction() == 1
        and sol.map.tile_is_enabled("potion_shop_door") then
      sol.map.tile_set_enabled("potion_shop_door", false)
      sol.main.play_sound("door_open")
    end
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

function event_dialog_finished(first_message_id, answer)

  if first_message_id == "inferno.found_fire_stones" then

    if answer == 0 then
      -- black stones
      sol.map.dialog_start("inferno.want_black_stones")
    else
      -- 100 rupees
      if not sol.game.savegame_get_boolean(916) then
        sol.map.dialog_start("inferno.want_rupees")
      else
        sol.map.dialog_start("inferno.want_rupees_again")
      end
    end 
  elseif first_message_id == "inferno.want_rupees" then
    sol.map.treasure_give("rupee", 5, 916)
  elseif first_message_id == "inferno.want_black_stones" then
    inferno_open()
  end
end

