local map = ...
-- Outside world C3

local fighting_boss = false
local arrows_timer

function map:on_started(destination_point)

  local new_music = nil
  if destination_point:get_name() == "from_ending" then
    -- game ending sequence
    hero:freeze()
    hero:set_visible(false)
    map:get_game():set_hud_enabled(false)
    map:set_entities_enabled("", false)
    new_music = "fanfare"
    map:set_entities_enabled("roof_entrance", false)
  else
    -- enable dark world
    if map:get_game():get_boolean(905) then
      new_music = "dark_mountain"
      map:set_tileset(13)
    else
      new_music = "overworld"
    end

    -- boss fight
    if destination_point:get_name() == "from_dungeon_10_5f" then

      if not map:get_game():get_boolean(299) then
	-- boss not killed yet
        new_music = "none"
        map:set_entities_enabled("", false) -- disable all simple enemies
      elseif not map:get_game():get_boolean(298) then
	-- boss killed but sword not got yet
	local variant = 2
	if map:get_game():get_ability("sword") >= 2 then
	  -- the player already has the second one: give the third one instead
	  variant = 3
	end
	map:create_pickable("sword", variant, 298, 440, 157, 1)
      end
    else
      map:set_entities_enabled("roof_entrance", false)
    end
  end

  sol.audio.play_music(new_music)
end

function map:on_opening_transition_finished(destination_point)

  if destination_point:get_name() == "from_ending" then
    map:start_dialog("credits_6")
    map:move_camera(440, 96, 20, function() end, 1e6)
  end
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "start_boss_sensor"
      and not map:get_game():get_boolean(299)
      and not fighting_boss then

    -- boss fight
    hero:freeze()
    map:set_entities_enabled("roof_entrance", false)
    roof_stairs:set_enabled(false)
    roof_teletransporter:set_enabled(false)
    sol.audio.play_sound("door_closed")
    sol.timer.start(1000, start_boss)
  end
end

local function start_boss()

  sol.audio.play_music("boss")
  boss:set_enabled(true)
  hero:unfreeze()
  fighting_boss = true
  arrows_timer = sol.timer.start(20000, repeat_give_arrows)
end

local function repeat_give_arrows()

  -- give arrows if necessary during the boss fight
  if map:get_game():get_item("bow"):get_amount() == 0 then
    local positions = {
      { x = 408, y = 189 },
      { x = 472, y = 189 },
    }
    arrow_xy = positions[math.random(#positions)]
    map:create_pickable("arrow", 3, -1, arrow_xy.x, arrow_xy.y, 1)
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
    map:create_pickable("sword", variant, 298, 440, 189, 1)
    if arrows_timer ~= nil then
      arrows_timer:stop()
      arrows_timer = nil
    end
  end
end

function hero:on_obtaining_treasure(item_name, variant, savegame_variable)

  if item_name == "sword" then
    sol.audio.play_music("excalibur")
  end
end

function hero:on_obtained_treasure(item_name, variant, savegame_variable)

  if item_name == "sword" then
    hero:start_victory()
  end
end

function hero:on_victory_finished()

  map:get_game():set_dungeon_finished(10)
  hero:teleport(119, "from_dungeon_10")
  map:set_entities_enabled("", true) -- enable simple enemies back

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

local function ending_next()
  hero:teleport(131, "from_ending")
end

