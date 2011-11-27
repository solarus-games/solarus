-- North Fairy Fountain

function has_fairy_appeared()
  return sol.game.savegame_get_boolean(929)
end

function has_tiger_scrolls()
  return sol.game.savegame_get_boolean(930)
end

function show_fairy()

  sol.map.npc_set_position("great_fairy", 160, 77)
  local sprite = sol.map.npc_get_sprite("great_fairy")
  sol.main.sprite_set_animation_ignore_suspend(sprite, true)
  sol.map.npc_remove("torch_1")
  sol.map.npc_remove("torch_2")
  sol.map.npc_remove("torch_3")
  sol.map.npc_remove("torch_4")
end
 
function event_map_started(destination_point_name)

  if has_fairy_appeared() then
    show_fairy()
  end
end

function are_all_torches_on()

  return sol.map.npc_exists("torch_1")
      and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch_1")) == "lit"
      and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch_2")) == "lit"
      and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch_3")) == "lit"
end

function event_update()

  if not has_fairy_appeared()
      and are_all_torches_on() then

    sol.main.play_sound("secret")
    sol.game.savegame_set_boolean(929, true)
    show_fairy()
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "fairy_sensor" and has_fairy_appeared() then

    sol.map.sensor_set_enabled(sensor_name, false)
    sol.map.hero_freeze()
    sol.map.hero_set_direction(1)
    if not has_tiger_scrolls() then
      sol.map.dialog_start("north_fairy_fountain.first_time")
    else
      sol.map.dialog_start("north_fairy_fountain.restore_health")
    end
  end
end

function event_dialog_finished(first_message_id, answer)

  if first_message_id == "north_fairy_fountain.first_time" then
    sol.map.hero_unfreeze()
    sol.map.treasure_give("level_4_way", 4, 930)
  elseif first_message_id == "fairy_cave.restore_health" then
    sol.map.hero_unfreeze()
    sol.game.add_life(sol.game.get_max_life())
  end
end

