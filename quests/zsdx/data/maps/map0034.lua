------------------
-- Bomb cave 1F --
------------------

keep_door_open = false

function are_all_torches_on()
  return sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_1")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_2")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_3")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_4")) == "lit" 
end

function event_map_started(destination_point_name)

  if destination_point_name == "from_2F" then
    sol.map.door_set_open("door", true)
    keep_door_open = true
  end
end

function event_hero_interaction_item_finished(entity_name, item_name, variant)

  if item_name == "lamp"
      and string.match(entity_name, "^torch")
      and not sol.map.door_is_open("door")
      and are_all_torches_on() then

    sol.main.play_sound("secret")
    sol.map.door_open("door")
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "close_door_sensor" then
    keep_door_open = false
  end
end

function event_update()

  if sol.map.door_is_open("door")
      and not keep_door_open
      and not are_all_torches_on() then

    sol.map.door_close("door")
  end

end

