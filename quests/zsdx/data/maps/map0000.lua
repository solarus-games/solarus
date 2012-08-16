local map = ...
------------------
-- Intro script --
------------------

fresco = 0
fresco_sprite = nil

function map:on_started(destination_point_name)
  map:hero_freeze()
  map:hud_set_enabled(false)
  map:hud_set_pause_enabled(false)
  map:dialog_set_style(1)
  fresco_sprite = map:npc_get_sprite("fresco")
  fresco_sprite:set_ignore_suspend(true)
  map:dialog_start("intro0")
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
    map:dialog_start("intro"..fresco)
    fresco_sprite:set_animation(fresco)
    fresco_sprite:fade_in()
  else
    map:dialog_set_style(0)
    next_map()
  end
end


function next_map()
  map:hero_set_map(28, "from_intro", 1)
end

