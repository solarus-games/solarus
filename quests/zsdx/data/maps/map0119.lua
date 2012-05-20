-- Outside world C3

local fighting_boss = false
local arrows_timer

function event_map_started(destination_point_name)

  local new_music = nil
  if destination_point_name == "from_ending" then
    -- game ending sequence
    sol.map.hero_freeze()
    sol.map.hero_set_visible(false)
    sol.map.hud_set_enabled(false)
    sol.map.enemy_set_group_enabled("", false)
    new_music = "fanfare"
    sol.map.tile_set_group_enabled("roof_entrance", false)
  else
    -- enable dark world
    if sol.map.get_game():get_boolean(905) then
      new_music = "dark_mountain"
      sol.map.tileset_set(13)
    end

    -- boss fight
    if destination_point_name == "from_dungeon_10_5f" then

      if not sol.map.get_game():get_boolean(299) then
	-- boss not killed yet
        new_music = "none"
        sol.map.enemy_set_group_enabled("", false) -- disable all simple enemies
      elseif not sol.map.get_game():get_boolean(298) then
	-- boss killed but sword not got yet
	local variant = 2
	if sol.map.get_game():get_ability("sword") >= 2 then
	  -- the player already has the second one: give the third one instead
	  variant = 3
	end
	sol.map.pickable_item_create("sword", variant, 298, 440, 157, 1)
      end
    else
      sol.map.tile_set_group_enabled("roof_entrance", false)
    end
  end

  if new_music ~= nil then
    sol.audio.play_music(new_music)
  end
end

function event_map_opening_transition_finished(destination_point_name)

  if destination_point_name == "from_ending" then
    sol.map.dialog_start("credits_6")
    sol.map.camera_move(440, 96, 20, function() end, 1e6)
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "start_boss_sensor"
      and not sol.map.get_game():get_boolean(299)
      and not fighting_boss then

    -- boss fight
    sol.map.hero_freeze()
    sol.map.tile_set_group_enabled("roof_entrance", false)
    sol.map.stairs_set_enabled("roof_stairs", false)
    sol.map.teletransporter_set_enabled("roof_teletransporter", false)
    sol.audio.play_sound("door_closed")
    sol.main:start_timer(1000, start_boss)
  end
end

function start_boss()

  sol.audio.play_music("boss")
  sol.map.enemy_set_enabled("boss", true)
  sol.map.hero_unfreeze()
  fighting_boss = true
  arrows_timer = sol.main:start_timer(20000, repeat_give_arrows)
end

function repeat_give_arrows()

  -- give arrows if necessary during the boss fight
  if sol.map.get_game():get_item_amount("bow") == 0 then
    local positions = {
      { x = 408, y = 189 },
      { x = 472, y = 189 },
    }
    arrow_xy = positions[math.random(#positions)]
    sol.map.pickable_item_create("arrow", 3, -1, arrow_xy.x, arrow_xy.y, 1)
  end
  arrows_timer = sol.main:start_timer(20000, repeat_give_arrows)
end

function event_enemy_dead(enemy_name)

  if enemy_name == "boss" then
    -- give the second sword
    local variant = 2
    if sol.map.get_game():get_ability("sword") == 2 then
      -- the player already has the second one: give the third one instead
      variant = 3
    end
    sol.map.pickable_item_create("sword", variant, 298, 440, 189, 1)
    if arrows_timer ~= nil then
      arrows_timer:stop()
      arrows_timer = nil
    end
  end
end

function event_treasure_obtaining(item_name, variant, savegame_variable)

  if item_name == "sword" then
    sol.audio.play_music("excalibur")
  end
end

function event_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "sword" then
    sol.map.hero_start_victory_sequence()
  end
end

function event_hero_victory_sequence_finished()

  sol.map.get_game():set_dungeon_finished(10)
  sol.map.hero_set_map(119, "from_dungeon_10", 1)
  sol.map.enemy_set_group_enabled("", true) -- enable simple enemies back

  sol.main:start_timer(1000, function()
    if sol.map.get_game():get_boolean(905) then
      sol.audio.play_music("dark_mountain")
    else
      sol.audio.play_music("overworld")
    end
  end)
end

function event_dialog_finished(dialog_id)

  if dialog_id == "credits_6" then
   sol.main:start_timer(2000, ending_next)
  end
end

function ending_next()
  sol.map.hero_set_map(131, "from_ending", 1)
end

