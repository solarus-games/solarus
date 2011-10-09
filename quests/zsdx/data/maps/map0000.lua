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
  fresco_sprite = sol.map.interactive_entity_get_sprite("fresco")
  sol.main.sprite_set_animation_ignore_suspend(fresco_sprite, true)
  sol.map.dialog_start("intro0")
end

function event_dialog_finished(first_message_id)

  if fresco == 0 then
    sol.map.tile_set_enabled("black_screen", false)
    sol.main.play_music("legend.spc")
    next_fresco()
  else
    sol.main.sprite_fade(fresco_sprite, 1)
    sol.main.timer_start(next_fresco, 600)
  end
end

function next_fresco()

  if fresco < 6 then
    fresco = fresco + 1
    sol.map.dialog_start("intro"..fresco)
    sol.main.sprite_set_animation(fresco_sprite, fresco)
    sol.main.sprite_fade(fresco_sprite, 0)
  else
    sol.map.dialog_set_style(0)
    next_map()
  end
end


function next_map()
  sol.map.hero_set_map(28, "from_intro", 1)
end

