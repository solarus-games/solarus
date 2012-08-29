local map = ...

-- Intro script.

local fresco_index = 0  -- Index of the current fresco.
local fresco_sprite = nil

local function next_fresco()

  if fresco_index < 6 then
    fresco_index = fresco_index + 1
    map:start_dialog("intro" .. fresco_index, function()
      fresco_sprite:fade_out()
      sol.timer.start(600, next_fresco)
    end)
    fresco_sprite:set_animation(fresco_index)
    fresco_sprite:fade_in()
  else
    map:set_dialog_style(0)
    hero:teleport(28, "from_intro")
  end
end

function map:on_started(destination_point)
  hero:freeze()
  map:get_game():set_hud_enabled(false)
  map:set_pause_enabled(false)
  map:set_dialog_style(1)
  fresco_sprite = fresco:get_sprite()
  fresco_sprite:set_ignore_suspend(true)
  map:start_dialog("intro0", function()
    black_screen:set_enabled(false)
    sol.audio.play_music("legend")
    next_fresco()
  end)
end

