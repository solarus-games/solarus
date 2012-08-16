local map = ...
-- Bomb cave 1F

function are_all_torches_on()

  return map:npc_exists("torch_1")
      and map:npc_get_sprite("torch_1"):get_animation() == "lit"
      and map:npc_get_sprite("torch_2"):get_animation() == "lit"
      and map:npc_get_sprite("torch_3"):get_animation() == "lit"
      and map:npc_get_sprite("torch_4"):get_animation() == "lit"
end

function lock_torches()

  map:npc_remove("torch_1")
  map:npc_remove("torch_2")
  map:npc_remove("torch_3")
  map:npc_remove("torch_4")
end

function map:on_started(destination_point_name)

  if destination_point_name == "from_2F" then
    map:door_set_open("door", true)
  else
    map:sensor_set_enabled("close_door_sensor", false)
  end
end

function map:on_update()

  local door_open = map:door_is_open("door")
  if not door_open and are_all_torches_on() then

    lock_torches()
    sol.audio.play_sound("secret")
    map:door_open("door")
  end
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "close_door_sensor"
      and map:door_is_open("door") then
    map:door_close("door")
  end
end

