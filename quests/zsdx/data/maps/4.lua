local map = ...
-- Outside world B3

local function remove_village_cave_door()
  tom_cave_door:remove()
  tom_cave_door_tile:set_enabled(false)
end

local function remove_stone_lock()
  stone_lock:remove()
  map:set_entities_enabled("stone_lock_tile", false)
end

-- Function called when the map starts
function map:on_started(destination)

  -- game ending sequence
  if destination ~= nil and destination:get_name() == "from_ending" then
    hero:freeze()
    hero:set_visible(false)
    map:get_game():set_hud_enabled(false)
    map:set_entities_enabled("enemy", false)
    sol.audio.play_music("fanfare")
    map:set_entities_enabled("broken_rupee_house", false)
  else
    -- enable dark world
    if map:get_game():get_value("b905") then
      sol.audio.play_music("dark_world")
      map:set_tileset(13)
    else
      sol.audio.play_music("overworld")
    end

    -- broken rupee house
    if map:get_game():get_value("b155") then
      to_rupee_house:set_enabled(false)
      rupee_house_door:set_enabled(false)
    else
      to_broken_rupee_house:set_enabled(false)
      map:set_entities_enabled("broken_rupee_house", false)
    end
  end

  local m = sol.movement.create("random_path")
  m:set_speed(32)
  m:start(chignon_woman)
  chignon_woman:get_sprite():set_animation("walking")

  -- remove Tom's cave door if open
  if map:get_game():get_value("b36") then
    remove_village_cave_door()
  end

  -- remove the stone lock if open
  if map:get_game():get_value("b159") then
    remove_stone_lock()
  end

  -- NPC
  if map:get_game():is_dungeon_finished(1) then
    cliff_man:remove()
  end

  -- Entrances of houses.
  local entrance_names = {
    "rupee_house", "lyly"
  }
  for _, entrance_name in ipairs(entrance_names) do
    local sensor = map:get_entity(entrance_name .. "_door_sensor")
    local tile = map:get_entity(entrance_name .. "_door")

    sensor.on_activated_repeat = function()
      if hero:get_direction() == 1
	  and tile:is_enabled() then
	tile:set_enabled(false)
	sol.audio.play_sound("door_open")
      end
    end
  end
end

function map:on_opening_transition_finished(destination)

  if destination ~= nil and destination:get_name() == "from_ending" then
    map:start_dialog("credits_2", function()
      sol.timer.start(2000, function()
	hero:teleport(56, "from_ending")
      end)
    end)
    map:move_camera(184, 80, 25, function() end, 1e6)
  end
end

-- Function called when the player presses the action key
-- while facing an interactive entity
function tom_cave_door:on_interaction()

  -- open the door if the player has the clay key
  if map:get_game():has_item("clay_key") then
    sol.audio.play_sound("door_open")
    sol.audio.play_sound("secret")
    map:get_game():set_value("b36", true)
    remove_village_cave_door()
  else
    map:start_dialog("outside_world.village.clay_key_required")
  end
end

function stone_lock:on_interaction()

  -- open the door if the player has the stone key
  if map:get_game():has_item("stone_key") then
    sol.audio.play_sound("door_open")
    sol.audio.play_sound("secret")
    map:get_game():set_value("b159", true)
    remove_stone_lock()
  else
    map:start_dialog("outside_world.stone_key_required")
  end
end

function chignon_woman:on_interaction()

  if map:get_game():is_dungeon_finished(2) then
    map:start_dialog("outside_world.village.chignon_woman_dungeons")
  else
    map:start_dialog("outside_world.village.chignon_woman")
  end
end

function waterfall_sensor:on_activated()

  hero:start_jumping(6, 288, true)
  sol.audio.play_sound("jump")
end

