-- Link's house

bed_sprite = nil

function event_map_started(destination_point_name)

  bed_sprite = sol.map.interactive_entity_get_sprite("bed")
  if destination_point_name == "from_intro" then
    -- the intro scene was playing
    snores_sprite = sol.map.interactive_entity_get_sprite("snores")
    sol.main.sprite_set_animation_ignore_suspend(snores_sprite, true)
    sol.main.sprite_set_animation(bed_sprite, "hero_sleeping")
    sol.map.hero_freeze()
    sol.map.hero_set_visible(false)
    sol.main.play_music("triforce.spc")
    sol.main.timer_start(3000, "intro", false)
  else
    sol.map.interactive_entity_remove("snores")
    sol.main.play_music("village.spc")
    sol.map.npc_set_position("bed", 56, 101)
  end
end

function intro()
  sol.map.dialog_start("link_house.intro")
end

function event_dialog_finished(first_message_id)

  if first_message_id == "link_house.intro" then
    sol.main.timer_start(2000, "wake_up", false)
  end
end

function wake_up()
  sol.main.play_music("village.spc")
  sol.map.interactive_entity_remove("snores")
  sol.main.sprite_set_animation(bed_sprite, "hero_waking")
  sol.main.timer_start(500, "jump_from_bed", false)
end

function jump_from_bed()
  sol.map.hero_set_visible(true)
  sol.map.hero_start_jumping(0, 8, true)
  sol.map.hud_set_pause_enabled(true)
  sol.main.sprite_set_animation(bed_sprite, "empty_open")
  sol.main.play_sound("hero_lands")
  sol.map.npc_set_position("bed", 56, 101)
end

