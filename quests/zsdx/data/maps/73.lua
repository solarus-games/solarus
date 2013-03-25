local map = ...
-- Lake shop

function map:on_started(destination)

  if destination:get_name() == "from_outside_n"
      and not door:is_open() then
    -- not supposed to be possible but just to be safe, don't start stuck in the wall
    map:set_doors_open("door", true)
  end
end

function door:on_opened()

  sol.audio.play_sound("secret")
end

