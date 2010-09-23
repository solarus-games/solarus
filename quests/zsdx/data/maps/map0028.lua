-------------------------
-- Link's house script --
-------------------------

function event_map_started(destination_point_name)

  if destination_point_name == "from_intro" then
    -- the intro scene is playing
    hud_set_enabled(true)
    hud_set_pause_enabled(false)
    dialog_set_style(0)
    interactive_entity_set_animation_ignore_suspend("snores", true)
    interactive_entity_set_animation("bed", "hero_sleeping")
    hero_freeze()
    hero_set_visible(false)
    timer_start(2000, "sahasrahla_dream", false)
  else
    interactive_entity_remove("snores")
  end
end

function sahasrahla_dream()
  dialog_start("link_house.dream")
  dialog_set_variable("link_house.dream", savegame_get_name())
end

function event_dialog_finished(first_message_id)
  
  if first_message_id == "link_house.dream" then
    timer_start(1000, "wake_up", false)
  end
end

function wake_up()
  interactive_entity_remove("snores")
  interactive_entity_set_animation("bed", "hero_waking")
  timer_start(500, "jump_from_bed", false)
end

function jump_from_bed()
  hero_set_visible(true)
  hero_jump(4, 24, true)
  interactive_entity_set_animation("bed", "empty_open")
  hud_set_pause_enabled(true)
  play_sound("hero_lands");
end


