local map = ...
-- Castle B1

function map:on_started(destination_point)

  if destination_point:get_name() == "from_outside_hole" then
    hero:set_direction(2)
  end
end

function weak_wall_a:on_open()
  sol.audio.play_sound("secret")
end

function weak_wall_b:on_open()
  sol.audio.play_sound("secret")
end

