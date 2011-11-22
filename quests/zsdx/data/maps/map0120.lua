-- Bone key cave

function event_hero_on_sensor(sensor_name)

  if sensor_name == "close_eye_sensor" then

    if sol.map.tile_is_enabled("laser_eye_open") then
      sol.map.tile_set_enabled("laser_eye_open", false)
      sol.map.tile_set_enabled("laser_eye_closed", true)
      sol.map.enemy_set_enabled("laser", false)
      sol.main.play_sound("secret")
    end
  end
end

