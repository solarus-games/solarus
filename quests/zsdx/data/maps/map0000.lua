------------------
-- Intro script --
------------------

fresco = 0
fresco_sprite = nil

function event_map_started(destination_point_name)
  sol.map.hero_freeze()
  sol.map.hud_set_enabled(false)
  sol.map.hud_set_pause_enabled(false)
  sol.map.dialog_set_style(1)
  fresco_sprite = sol.map.npc_get_sprite("fresco")
  fresco_sprite:set_ignore_suspend(true)
  sol.map.dialog_start("intro0")
end

function event_dialog_finished(dialog_id)

  if fresco == 0 then
    sol.map.tile_set_enabled("black_screen", false)
    sol.audio.play_music("legend")
    next_fresco()
  else
    fresco_sprite:fade(1)
    sol.main.timer_start(next_fresco, 600)
  end
end

function next_fresco()

  if fresco < 6 then
    fresco = fresco + 1
    sol.map.dialog_start("intro"..fresco)
    fresco_sprite:set_animation(fresco)
    fresco_sprite:fade(0)
  else
    sol.map.dialog_set_style(0)
    next_map()
  end
end


function next_map()
  sol.map.hero_set_map(28, "from_intro", 1)
end

