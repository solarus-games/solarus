local map = ...
-- Outside world B2

function map:on_started(destination_point_name)

  -- enable dark world
  if map:get_game():get_boolean(905) then
    sol.audio.play_music("dark_world")
    map:tileset_set(13)
  else
    sol.audio.play_music("light_world")
  end

  -- remove the iron lock if open
  if map:get_game():get_boolean(193) then
    remove_iron_lock()
  end

  -- remove the wooden lock if open
  if map:get_game():get_boolean(194) then
    remove_wooden_lock()
  end

  -- Inferno
  if not map:get_game():is_dungeon_finished(5) then
    map:npc_remove("inferno")
  else
    local sprite = map:npc_get_sprite("inferno")
    sprite:set_ignore_suspend(true)
    if map:get_game():get_boolean(914) then
      inferno_set_open()
    end
  end
  if not map:get_game():get_boolean(914) then
    map:teletransporter_set_enabled("to_dungeon_6", false)
  end
  map:sensor_set_enabled("inferno_sensor", false)
end

function remove_iron_lock()
  map:npc_remove("iron_lock")
  map:tile_set_group_enabled("iron_lock_tile", false)
end

function remove_wooden_lock()
  map:npc_remove("wooden_lock")
  map:tile_set_group_enabled("wooden_lock_tile", false)
end

function map:on_npc_interaction(npc_name)

  if npc_name == "iron_lock" then

    -- open the door if the player has the iron key
    if map:get_game():has_item("iron_key") then
      sol.audio.play_sound("door_open")
      sol.audio.play_sound("secret")
      map:get_game():set_boolean(193, true)
      remove_iron_lock()
    else
      map:dialog_start("outside_world.iron_key_required")
    end

  elseif npc_name == "wooden_lock" then

    -- open the door if the player has the wooden key
    if map:get_game():has_item("wooden_key") then
      sol.audio.play_sound("door_open")
      sol.audio.play_sound("secret")
      map:get_game():set_boolean(194, true)
      remove_wooden_lock()
    else
      map:dialog_start("outside_world.wooden_key_required")
    end

  elseif npc_name == "inferno" then

    if not map:get_game():get_boolean(915) then
      -- first time
      map:dialog_start("inferno.first_time")
      map:get_game():set_boolean(915, true)
    elseif not map:get_game():get_boolean(914) then
      -- not open yet
      if map:get_game():get_item_amount("fire_stones_counter") < 3 then
        map:dialog_start("inferno.find_fire_stones")
      else
        map:dialog_start("inferno.found_fire_stones")
      end
    end
  end
end

function inferno_open()

  map:sensor_set_enabled("inferno_sensor", true)
  map:hero_walk("66", false, false)
end

function map:on_hero_on_sensor(sensor_name)

  -- Inferno
  if sensor_name == "inferno_sensor" then
    local sprite = map:npc_get_sprite("inferno")
    sprite:set_animation("opening")
    sol.timer.start(1050, inferno_open_finish)
    map:hero_freeze()
    map:hero_set_direction(1)
    map:sensor_set_enabled("inferno_sensor", false)
  end
end

function map:on_hero_still_on_sensor(sensor_name)

  -- Witch hut entrance
  if sensor_name == "potion_shop_door_sensor" then
    if map:hero_get_direction() == 1
        and map:tile_is_enabled("potion_shop_door") then
      map:tile_set_enabled("potion_shop_door", false)
      sol.audio.play_sound("door_open")
    end
  end
end

function inferno_open_finish()

  sol.audio.play_sound("secret")
  map:hero_unfreeze()
  map:get_game():set_boolean(914, true)
  inferno_set_open()
end

function inferno_set_open()

  local sprite = map:npc_get_sprite("inferno")
  sprite:set_animation("open")
  map:teletransporter_set_enabled("to_dungeon_6", true)
end

function map:on_dialog_finished(dialog_id, answer)

  if dialog_id == "inferno.found_fire_stones" then

    if answer == 0 then
      -- black stones
      map:dialog_start("inferno.want_black_stones")
    else
      -- 100 rupees
      if not map:get_game():get_boolean(916) then
        map:dialog_start("inferno.want_rupees")
      else
        map:dialog_start("inferno.want_rupees_again")
      end
    end 
  elseif dialog_id == "inferno.want_rupees" then
    map:treasure_give("rupee", 5, 916)
  elseif dialog_id == "inferno.want_black_stones" then
    inferno_open()
  end
end

