local map = ...
-- Outside world B2

function map:on_started(destination_point)

  -- enable dark world
  if map:get_game():get_boolean(905) then
    sol.audio.play_music("dark_world")
    map:set_tileset(13)
  else
    sol.audio.play_music("overworld")
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
    inferno:remove()
  else
    inferno:get_sprite():set_ignore_suspend(true)
    if map:get_game():get_boolean(914) then
      inferno_set_open()
    end
  end
  if not map:get_game():get_boolean(914) then
    to_dungeon_6:set_enabled(false)
  end
  inferno_sensor:set_enabled(false)
end

function remove_iron_lock()
  iron_lock:remove()
  map:set_entities_enabled("iron_lock_tile", false)
end

function remove_wooden_lock()
  wooden_lock:remove()
  map:set_entities_enabled("wooden_lock_tile", false)
end

function iron_lock:on_interaction()

  -- open the door if the player has the iron key
  if map:get_game():has_item("iron_key") then
    sol.audio.play_sound("door_open")
    sol.audio.play_sound("secret")
    map:get_game():set_boolean(193, true)
    remove_iron_lock()
  else
    map:start_dialog("outside_world.iron_key_required")
  end
end

function wooden_lock:on_interaction()

  -- open the door if the player has the wooden key
  if map:get_game():has_item("wooden_key") then
    sol.audio.play_sound("door_open")
    sol.audio.play_sound("secret")
    map:get_game():set_boolean(194, true)
    remove_wooden_lock()
  else
    map:start_dialog("outside_world.wooden_key_required")
  end
end

function inferno:on_interaction()

  if not map:get_game():get_boolean(915) then
    -- first time
    map:start_dialog("inferno.first_time")
    map:get_game():set_boolean(915, true)
  elseif not map:get_game():get_boolean(914) then
    -- not open yet
    if not map:get_game():get_item("fire_stones_counter"):has_amount(3) then
      map:start_dialog("inferno.find_fire_stones")
    else
      map:start_dialog("inferno.found_fire_stones", function(answer)
        if answer == 0 then
          -- black stones
          map:start_dialog("inferno.want_black_stones", function()
            inferno_open()
          end)
        else
          -- 100 rupees
          if not map:get_game():get_boolean(916) then
            map:start_dialog("inferno.want_rupees", function()
              map:get_hero():start_treasure("rupee", 5, 916)
            end)
          else
            map:start_dialog("inferno.want_rupees_again")
          end
        end
      end)
    end
  end
end

function inferno_open()

  inferno_sensor:set_enabled(true)
  map:get_hero():walk("66", false, false)
end

function inferno_sensor:on_activated()

  inferno:get_sprite():set_animation("opening")
  sol.timer.start(1050, inferno_open_finish)
  map:get_hero():freeze()
  map:get_hero():set_direction(1)
  inferno_sensor:set_enabled(false)
end

function potion_shop_door_sensor:on_activated_repeat()

  if map:get_hero():get_direction() == 1
    and potion_shop_door:is_enabled() then
    potion_shop_door:set_enabled(false)
    sol.audio.play_sound("door_open")
  end
end

function inferno_open_finish()

  sol.audio.play_sound("secret")
  map:get_hero():unfreeze()
  map:get_game():set_boolean(914, true)
  inferno_set_open()
end

function inferno_set_open()

  inferno:get_sprite():set_animation("open")
  to_dungeon_6:set_enabled(true)
end

