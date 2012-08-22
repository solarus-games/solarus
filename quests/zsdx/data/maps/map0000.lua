local map = ...
------------------
-- Intro script --
------------------

fresco = 0
fresco_sprite = nil

function map:on_started(destination_point_name)
  map:hero_freeze()
  map:get_game():set_hud_enabled(false)
  map:set_pause_enabled(false)
  map:set_dialog_style(1)
  fresco_sprite = map:npc_get_sprite("fresco")
  fresco_sprite:set_ignore_suspend(true)
  map:start_dialog("intro0")
end

function map:on_dialog_finished(dialog_id)

  if fresco == 0 then
    map:tile_set_enabled("black_screen", false)
    sol.audio.play_music("legend")
    next_fresco()
  else
    fresco_sprite:fade_out()
    sol.timer.start(600, next_fresco)
  end
end

function next_fresco()

  if fresco < 6 then
    fresco = fresco + 1
    map:start_dialog("intro"..fresco)
    fresco_sprite:set_animation(fresco)
    fresco_sprite:fade_in()
  else
    map:set_dialog_style(0)
    next_map()
  end
end


function next_map()
  map:get_hero():teleport(28, "from_intro")
end

