-------------------------
-- Link's house script --
-------------------------

function event_map_started(destination_point_name)

  if destination_point_name == "from_intro" then
    -- the intro scene is playing
    sol.map.hud_set_enabled(true)
    sol.map.hud_set_pause_enabled(false)
    sol.map.dialog_set_style(0)
    sol.map.interactive_entity_set_animation_ignore_suspend("snores", true)
    sol.map.interactive_entity_set_animation("bed", "hero_sleeping")
    sol.map.hero_freeze()
    sol.map.hero_set_visible(false)
    sol.main.timer_start(2000, "sahasrahla_dream", false)
  else
    sol.map.interactive_entity_remove("snores")
  end
end

function sahasrahla_dream()
  sol.map.dialog_start("link_house.dream")
  sol.map.dialog_set_variable("link_house.dream", sol.game.savegame_get_name())
end

function event_dialog_finished(first_message_id)
  
  if first_message_id == "link_house.dream" then
    sol.main.timer_start(1000, "wake_up", false)
  end
end

function wake_up()
  sol.map.interactive_entity_remove("snores")
  sol.map.interactive_entity_set_animation("bed", "hero_waking")
  sol.main.timer_start(500, "jump_from_bed", false)
end

function jump_from_bed()
  sol.map.hero_set_visible(true)
  sol.map.hero_jump(4, 24, true)
  sol.map.interactive_entity_set_animation("bed", "empty_open")
  sol.map.hud_set_pause_enabled(true)
  sol.main.play_sound("hero_lands");
end


