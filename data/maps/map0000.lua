------------------
-- Intro script --
------------------

fresco = 0

function event_map_started(destination_point_name)
  hud_set_enabled(false)
  dialog_set_style(1)
  start_message("intro0")
end

function event_message_sequence_finished(first_message_id)

  if fresco == 0 then
    tile_set_enabled("black_screen", false)
  end

  if fresco < 6 then
    fresco = fresco + 1
    interactive_entity_set_animation("fresco", fresco)
    start_message("intro"..fresco)
  else
    dialog_set_style(0)
    hud_set_enabled(true)
    hero_set_map(28, "from_intro", 1)
  end
end

