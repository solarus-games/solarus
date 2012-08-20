local map = ...
-- Fairy cave

function has_fairy_appeared()
  return map:get_game():get_boolean(99)
end

function has_boomerang_from_fairy()
  return map:get_game():get_boolean(100)
end

function show_fairy()

  map:npc_set_position("great_fairy", 160, 77)
  local sprite = map:npc_get_sprite("great_fairy")
  sprite:set_ignore_suspend(true)
  map:npc_remove("torch_1")
  map:npc_remove("torch_2")
  map:npc_remove("torch_3")
  map:npc_remove("torch_4")
end
 
function map:on_started(destination_point_name)

  if has_fairy_appeared() then
    show_fairy()
  end
end

-- Returns whether all four torches are on
function are_all_torches_on()

  return map:npc_exists("torch_1")
      and map:npc_get_sprite("torch_1"):get_animation() == "lit"
      and map:npc_get_sprite("torch_2"):get_animation() == "lit"
      and map:npc_get_sprite("torch_3"):get_animation() == "lit"
      and map:npc_get_sprite("torch_4"):get_animation() == "lit" 
end

function map:on_update()

  if not has_fairy_appeared()
      and are_all_torches_on() then

    sol.audio.play_sound("secret")
    map:get_game():set_boolean(99, true)
    show_fairy()
  end
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "fairy_sensor" and has_fairy_appeared() then

    map:sensor_set_enabled(sensor_name, false)
    map:hero_freeze()
    map:hero_set_direction(1)
    if not has_boomerang_from_fairy() then
      map:start_dialog("fairy_cave.first_time")
    else
      map:start_dialog("fairy_cave.restore_health")
    end
  end
end

function map:on_dialog_finished(dialog_id, answer)

  if dialog_id == "fairy_cave.first_time" then
    map:hero_unfreeze()
    map:hero_start_treasure("boomerang", 2, 100)
  elseif dialog_id == "fairy_cave.restore_health" then
    map:hero_unfreeze()
    map:get_game():add_life(map:get_game():get_max_life())
  end
end

