local map = ...
-- Bone key cave

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "close_eye_sensor" then

    if laser_eye_open:is_enabled() then
      laser_eye_open:set_enabled(false)
      laser_eye_closed:set_enabled(true)
      laser:set_enabled(false)
      sol.audio.play_sound("secret")
    end

  elseif sensor_name:find("^save_solid_ground_sensor") then
    hero:save_solid_ground()
  end
end

