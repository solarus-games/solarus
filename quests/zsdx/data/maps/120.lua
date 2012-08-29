local map = ...
-- Bone key cave

function close_eye_sensor:on_activated()

  if laser_eye_open:is_enabled() then
    laser_eye_open:set_enabled(false)
    laser_eye_closed:set_enabled(true)
    laser:set_enabled(false)
    sol.audio.play_sound("secret")
  end
end

function save_solid_ground_sensor:on_activated()
  hero:save_solid_ground()
end

