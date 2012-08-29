local map = ...
-- Bomb cave 1F

local function are_all_torches_on()

  return torch_1:exists()
      and torch_1:get_sprite():get_animation() == "lit"
      and torch_2:get_sprite():get_animation() == "lit"
      and torch_3:get_sprite():get_animation() == "lit"
      and torch_4:get_sprite():get_animation() == "lit"
end

local function lock_torches()

  torch_1:remove()
  torch_2:remove()
  torch_3:remove()
  torch_4:remove()
end

function map:on_started(destination)

  if destination:get_name() == "from_2F" then
    map:set_doors_open("door", true)
  else
    close_door_sensor:set_enabled(false)
  end
end

function map:on_update()

  local door_open = door:is_open()
  if not door_open and are_all_torches_on() then

    lock_torches()
    sol.audio.play_sound("secret")
    map:open_doors("door")
  end
end

function close_door_sensor:on_activated()

  if door:is_open() then
    map:close_doors("door")
  end
end

