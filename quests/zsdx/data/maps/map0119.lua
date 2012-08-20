local map = ...
-- Outside world C3

local fighting_boss = false
local arrows_timer

function map:on_started(destination_point_name)

  local new_music = nil
  if destination_point_name == "from_ending" then
    -- game ending sequence
    map:hero_freeze()
    map:hero_set_visible(false)
    map:get_game():set_hud_enabled(false)
    map:enemy_set_group_enabled("", false)
    new_music = "fanfare"
    map:tile_set_group_enabled("roof_entrance", false)
  else
    -- enable dark world
    if map:get_game():get_boolean(905) then
      new_music = "dark_mountain"
      map:set_tileset(13)
    else
      new_music = "overworld"
    end

    -- boss fight
    if destination_point_name == "from_dungeon_10_5f" then

      if not map:get_game():get_boolean(299) then
	-- boss not killed yet
        new_music = "none"
        map:enemy_set_group_enabled("", false) -- disable all simple enemies
      elseif not map:get_game():get_boolean(298) then
	-- boss killed but sword not got yet
	local variant = 2
	if map:get_game():get_ability("sword") >= 2 then
	  -- the player already has the second one: give the third one instead
	  variant = 3
	end
	map:pickable_create("sword", variant, 298, 440, 157, 1)
      end
    else
      map:tile_set_group_enabled("roof_entrance", false)
    end
  end

  sol.audio.play_music(new_music)
end

function map:on_map_opening_transition_finished(destination_point_name)

  if destination_point_name == "from_ending" then
    map:start_dialog("credits_6")
    map:move_camera(440, 96, 20, function() end, 1e6)
  end
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "start_boss_sensor"
      and not map:get_game():get_boolean(299)
      and not fighting_boss then

    -- boss fight
    map:hero_freeze()
    map:tile_set_group_enabled("roof_entrance", false)
    map:stairs_set_enabled("roof_stairs", false)
    map:teletransporter_set_enabled("roof_teletransporter", false)
    sol.audio.play_sound("door_closed")
    sol.timer.start(1000, start_boss)
  end
end

function start_boss()

  sol.audio.play_music("boss")
  map:enemy_set_enabled("boss", true)
  map:hero_unfreeze()
  fighting_boss = true
  arrows_timer = sol.timer.start(20000, repeat_give_arrows)
end

function repeat_give_arrows()

  -- give arrows if necessary during the boss fight
  if map:get_game():get_item_amount("bow") == 0 then
    local positions = {
      { x = 408, y = 189 },
      { x = 472, y = 189 },
    }
    arrow_xy = positions[math.random(#positions)]
    map:pickable_create("arrow", 3, -1, arrow_xy.x, arrow_xy.y, 1)
  end
  arrows_timer = sol.timer.start(20000, repeat_give_arrows)
end

function map:on_enemy_dead(enemy_name)

  if enemy_name == "boss" then
    -- give the second sword
    local variant = 2
    if map:get_game():get_ability("sword") == 2 then
      -- the player already has the second one: give the third one instead
      variant = 3
    end
    map:pickable_create("sword", variant, 298, 440, 189, 1)
    if arrows_timer ~= nil then
      arrows_timer:stop()
      arrows_timer = nil
    end
  end
end

function map:on_treasure_obtaining(item_name, variant, savegame_variable)

  if item_name == "sword" then
    sol.audio.play_music("excalibur")
  end
end

function map:on_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "sword" then
    map:hero_start_victory_sequence()
  end
end

function map:on_hero_victory_sequence_finished()

  map:get_game():set_dungeon_finished(10)
  map:hero_set_map(119, "from_dungeon_10", 1)
  map:enemy_set_group_enabled("", true) -- enable simple enemies back

  sol.timer.start(1000, function()
    if map:get_game():get_boolean(905) then
      sol.audio.play_music("dark_mountain")
    else
      sol.audio.play_music("overworld")
    end
  end)
end

function map:on_dialog_finished(dialog_id)

  if dialog_id == "credits_6" then
   sol.timer.start(2000, ending_next)
  end
end

function ending_next()
  map:hero_set_map(131, "from_ending", 1)
end

