-------------------------
-- Link's house script --
-------------------------

function event_map_started(destination_point_name)

  if destination_point_name == "from_intro" then
    -- the intro scene is playing
    hud_set_enabled(true)
    player_set_pause_enabled(false)
    dialog_set_style(0)
    interactive_entity_set_animation_ignore_suspend("snores", true)
    interactive_entity_set_animation("bed", "hero_sleeping")
    freeze()
    hero_set_visible(false)
    start_timer(2000, "sahasrahla_dream", false)
  else
    interactive_entity_remove("snores")
  end
end

function sahasrahla_dream()
  start_message("link_house.dream")
  set_message_variable("link_house.dream", player_get_name())
end

function event_message_sequence_finished(first_message_id)
  
  if first_message_id == "link_house.dream" then
    start_timer(1000, "wake_up", false)
  end
end

function wake_up()
  interactive_entity_remove("snores")
  interactive_entity_set_animation("bed", "hero_waking")
  start_timer(500, "jump_from_bed", false)
end

function jump_from_bed()
  hero_set_visible(true)
  hero_jump(4, 24, false)
  interactive_entity_set_animation("bed", "empty_open")
  player_set_pause_enabled(true)
  play_sound("hero_lands");
end


