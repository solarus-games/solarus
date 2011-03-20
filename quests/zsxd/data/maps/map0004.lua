-- Outside SE

last_yoda_quote = 0

function event_map_started(destination_point_name)

  if sol.game.savegame_get_boolean(65) then
    sol.map.tile_set_enabled("temple_door_tile", false)
    sol.map.interactive_entity_remove("temple_door")
  end

  yoda_sprite = sol.map.npc_get_sprite("yoda")
  sol.main.sprite_set_animation(yoda_sprite, "walking")
end

function event_hero_interaction(entity_name)

  if entity_name == "temple_door" then
    if sol.game.has_item("bone_key") then
      sol.main.play_sound("door_open")
      sol.main.play_sound("secret")
      sol.map.tile_set_enabled("temple_door_tile", false)
      sol.map.interactive_entity_remove("temple_door")
      sol.game.savegame_set_boolean(65, true)
    else
      sol.main.play_sound("wrong")
      sol.map.dialog_start("outside_fields_SE.temple_door_closed")
    end
  end
end

function event_hero_on_sensor(sensor_name)

  if string.match(sensor_name, "^yoda_sensor") then

    -- choose a random quote
    repeat -- make sure the same quote is not picked again
      index = math.random(9)
    until index ~= last_yoda_quote
    sol.map.dialog_start("outside_fields_SE.yoda_quote_"..index)
    last_yoda_quote = index
  end
end

function event_npc_dialog(npc_name)

  if npc_name == "yoda" then
    if not sol.game.savegame_get_boolean(66) then
      sol.map.dialog_start("outside_fields_SE.yoda_give_sword")
    else
      sol.map.dialog_start("outside_fields_SE.yoda_finished")
    end
  end
end

function event_dialog_finished(first_message_id)

  if first_message_id == "outside_fields_SE.yoda_give_sword" then
    sol.map.treasure_give("sword", 2, 66)
  end
end

