-- Link's house

bed_sprite = nil

function event_map_started(destination_point_name)

  bed_sprite = sol.map.npc_get_sprite("bed")
  if destination_point_name == "from_intro" then
    -- the intro scene is playing
    sol.map.hud_set_enabled(true)
    sol.map.hud_set_pause_enabled(false)
    sol.map.dialog_set_style(0)
    local snores_sprite = sol.map.npc_get_sprite("snores")
    snores_sprite:set_ignore_suspend(true)
    bed_sprite:set_animation("hero_sleeping")
    sol.map.hero_freeze()
    sol.map.hero_set_visible(false)
    sol.timer.start(2000, sahasrahla_dream)
  else
    sol.map.npc_remove("snores")
  end
end

function sahasrahla_dream()
  sol.map.dialog_start("link_house.dream")
  sol.map.dialog_set_variable("link_house.dream", sol.map.get_game():get_player_name())
end

function event_dialog_finished(dialog_id)
  
  if dialog_id == "link_house.dream" then
    sol.timer.start(1000, wake_up)
  end
end

function wake_up()
  sol.map.npc_remove("snores")
  bed_sprite:set_animation("hero_waking")
  sol.timer.start(500, jump_from_bed)
end

function jump_from_bed()
  sol.map.hero_set_visible(true)
  sol.map.hero_start_jumping(4, 24, true)
  sol.map.hud_set_pause_enabled(true)
  bed_sprite:set_animation("empty_open")
  sol.audio.play_sound("hero_lands");
end

function event_door_open(door_name)

  if door_name == "weak_wall" then
    sol.audio.play_sound("secret")
  end
end


