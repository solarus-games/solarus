------------------------
-- Fairy cave script --
------------------------

function has_fairy_appeared()
  return sol.game.savegame_get_boolean(99)
end

function has_boomerang_from_fairy()
  return sol.game.savegame_get_boolean(100)
end

function show_fairy()
  sol.map.npc_set_position("great_fairy", 160, 77)
  sol.map.interactive_entity_remove("torch_1")
  sol.map.interactive_entity_remove("torch_2")
  sol.map.interactive_entity_remove("torch_3")
  sol.map.interactive_entity_remove("torch_4")
end
 
function event_map_started(destination_point_name)

  if has_fairy_appeared() then
    show_fairy()
  end
end

function event_hero_interaction_item_finished(entity_name, item_name, variant)

  if item_name == "lamp"
      and string.match(entity_name, "^torch")
      and not has_fairy_appeared()
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_1")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_2")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_3")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_4")) == "lit" then

    sol.main.play_sound("secret")
    sol.game.savegame_set_boolean(99, true)
    show_fairy()
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "fairy_sensor" and has_fairy_appeared() then

    if not has_boomerang_from_fairy() then
      sol.map.dialog_start("fairy_cave.first_time")
    else
      sol.map.dialog_start("fairy_cave.restore_health")
    end
    sol.map.sensor_set_enabled(sensor_name, false)
  end
end

function event_dialog_finished(first_message_id, answer)

  if first_message_id == "fairy_cave.first_time" then
    sol.map.treasure_give("boomerang", 1, 100)
  elseif first_message_id == "fairy_cave.restore_health" then
    sol.game.add_life(sol.game.get_max_life())
  end
end

