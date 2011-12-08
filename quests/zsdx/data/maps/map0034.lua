-- Bomb cave 1F

function are_all_torches_on()

  return sol.map.npc_exists("torch_1")
      and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch_1")) == "lit"
      and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch_2")) == "lit"
      and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch_3")) == "lit"
      and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch_4")) == "lit"
end

function lock_torches()

  sol.map.npc_remove("torch_1")
  sol.map.npc_remove("torch_2")
  sol.map.npc_remove("torch_3")
  sol.map.npc_remove("torch_4")
end

function event_map_started(destination_point_name)

  if destination_point_name == "from_2F" then
    sol.map.door_set_open("door", true)
  else
    sol.map.sensor_set_enabled("close_door_sensor", false)
  end
end

function event_update()

  local door_open = sol.map.door_is_open("door")
  if not door_open and are_all_torches_on() then

    lock_torches()
    sol.main.play_sound("secret")

    -- make sure the player cannot enter dungeon 3 if dungeon 2 is not finished
    if sol.game.is_dungeon_finished(2) then
      sol.map.door_open("door")
    end
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "close_door_sensor"
      and sol.map.door_is_open("door") then
    sol.map.door_close("door")
  end
end

