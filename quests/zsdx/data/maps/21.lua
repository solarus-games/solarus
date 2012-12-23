local map = ...
-- Lyriann's cave B1

function open_barrier_1_switch:on_activated()

  if barrier_1:is_enabled() then
    barrier_1:set_enabled(false)
    sol.audio.play_sound("door_open")
    close_barrier_1_switch:set_activated(false)
  end
end

function close_barrier_1_switch:on_activated()

  if not barrier_1:is_enabled() then
    barrier_1:set_enabled(true)
    sol.audio.play_sound("door_closed")
    open_barrier_1_switch:set_activated(false)
  end
end

function open_barrier_2_switch:on_activated()

  if barrier_2:is_enabled() then
    barrier_2:set_enabled(false)
    sol.audio.play_sound("door_open")
    close_barrier_2_switch:set_activated(false)
  end
end

function close_barrier_2_switch:on_activated()

  if not barrier_2:is_enabled() then
    barrier_2:set_enabled(true)
    sol.audio.play_sound("door_closed")
    open_barrier_2_switch:set_activated(false)
  end
end

