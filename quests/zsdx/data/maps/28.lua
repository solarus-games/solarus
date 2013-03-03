local map = ...
-- Link's house

local function jump_from_bed()
  hero:set_visible(true)
  hero:start_jumping(4, 24, true)
  map:set_pause_enabled(true)
  bed:get_sprite():set_animation("empty_open")
  sol.audio.play_sound("hero_lands")
end

local function wake_up()
  snores:remove()
  bed:get_sprite():set_animation("hero_waking")
  sol.timer.start(500, function()
    jump_from_bed()
  end)
end

function map:on_started(destination)

  if destination:get_name() == "from_intro" then
    -- the intro scene is playing
    map:get_game():set_hud_enabled(true)
    map:set_pause_enabled(false)
    map:set_dialog_style(0)
    snores:get_sprite():set_ignore_suspend(true)
    bed:get_sprite():set_animation("hero_sleeping")
    hero:freeze()
    hero:set_visible(false)
    sol.timer.start(2000, function()
      map:set_dialog_variable("link_house.dream", map:get_game():get_player_name())
      map:start_dialog("link_house.dream", function()
        sol.timer.start(1000, function()
          wake_up()
        end)
      end)
    end)
  else
    snores:remove()
  end
end

function weak_wall:on_opened()

  sol.audio.play_sound("secret")
end


