------------------
-- Intro script --
------------------

fresco = 0

function event_map_started(destination_point_name)
  freeze()
  hud_set_enabled(false)
  player_set_pause_enabled(false)
  dialog_set_style(1)
  interactive_entity_set_animation_ignore_suspend("fresco", true)
  start_message("intro0")
end

function event_message_sequence_finished(first_message_id)

  if fresco == 0 then
    tile_set_enabled("black_screen", false)
    play_music("legend.spc")
    next_fresco()
  else
    interactive_entity_fade("fresco", 1)
    start_timer(600, "next_fresco", false)
  end
end

function next_fresco()

  if fresco < 6 then
    fresco = fresco + 1
    start_message("intro"..fresco)
    interactive_entity_set_animation("fresco", fresco)
    interactive_entity_fade("fresco", 0)
  else
    dialog_set_style(0)
    next_map()
  end
end


function next_map()
  hero_set_map(28, "from_intro", 1)
end

