local map = ...
-- Bone key cave

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "close_eye_sensor" then

    if map:tile_is_enabled("laser_eye_open") then
      map:tile_set_enabled("laser_eye_open", false)
      map:tile_set_enabled("laser_eye_closed", true)
      map:enemy_set_enabled("laser", false)
      sol.audio.play_sound("secret")
    end

  elseif sensor_name:find("^save_solid_ground_sensor") then
    map:get_hero():save_solid_ground()
  end
end

