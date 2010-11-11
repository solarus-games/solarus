-------------------------
-- Link's house script --
-------------------------

bed_sprite = nil

function event_map_started(destination_point_name)

  bed_sprite = sol.map.interactive_entity_get_sprite("bed")
  if destination_point_name == "from_intro" then
    -- the intro scene is playing
    sol.map.hud_set_enabled(true)
    sol.map.hud_set_pause_enabled(false)
    sol.map.dialog_set_style(0)
    snores_sprite = sol.map.interactive_entity_get_sprite("snores")
    sol.main.sprite_set_animation_ignore_suspend(snores_sprite, true)
    sol.main.sprite_set_animation(bed_sprite, "hero_sleeping")
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
  sol.main.sprite_set_animation(bed_sprite, "hero_waking")
  sol.main.timer_start(500, "jump_from_bed", false)
end

function jump_from_bed()
  sol.map.hero_set_visible(true)
  sol.map.hero_start_jumping(4, 24, true)
  sol.map.hud_set_pause_enabled(true)
  sol.main.sprite_set_animation(bed_sprite, "empty_open")
  sol.main.play_sound("hero_lands");
end


