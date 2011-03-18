-- The end

function event_map_started(destination_point_name)

  sol.map.hud_set_enabled(false)
  sol.map.hero_freeze()
  sol.game.save()
  sol.main.timer_start(7000, "credits", false)
  sol.main.play_sound("hero_dying")
end

function credits()
  sol.map.dialog_start("the_end.credits")
end

function event_dialog_finished(first_message_id)

  if first_message_id == "the_end.credits" then
    sol.game.reset()
  end
end
